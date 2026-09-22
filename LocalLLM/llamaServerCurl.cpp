#include "llamaServerCurl.h"

#include <curl/curl.h>
#include <nlohmann/json.hpp>

#define LLAMA_SERVER_URL "http://192.168.0.13:8081/v1/chat/completions"
#define LLAMA_SERVER_API_KEY "aac7cad858b5df7eea54979d29f76ff96b3d37d4b1f9c7f7"
#define LLAMA_SERVER_MODEL "gemma-4-E4B-it-q4_0"

namespace
{
	// curl_easy_setopt(CURLOPT_WRITEFUNCTION, ...) に渡すコールバック。
	// libcurlは受信したボディを断片(chunk)単位でこの関数に渡してくるので、userdataに毎回追記していく。
	// 戻り値は「実際に受け取ったバイト数」。size*nmembと異なる値を返すとlibcurlはエラー扱いにする。
	size_t WriteToString(char* ptr, size_t size, size_t nmemb, void* userdata)
	{
		size_t bytes = size * nmemb;
		static_cast<std::string*>(userdata)->append(ptr, bytes);
		return bytes;
	}
}

std::string LocalLLM::CallLlamaServerCurl(const std::string& utf8UserMessage)
{
	// curl_global_init はプロセス全体で1回だけ、かつ他スレッドが動いていない状態で呼ぶ必要がある
	// (スレッドセーフではない)。C++11のローカルstatic初期化はスレッドセーフに1回だけ実行されるので、
	// この関数が最初に呼ばれたタイミングで安全に初期化できる。
	static bool curlInitialized = []()
		{
			curl_global_init(CURL_GLOBAL_DEFAULT);
			return true;
		}();
	(void)curlInitialized;

	nlohmann::json requestBody =
	{
			{"model", LLAMA_SERVER_MODEL},
			{"chat_template_kwargs", {{"enable_thinking", false}}},
			{"messages", nlohmann::json::array({
				{{"role", "system"}, {"content", reinterpret_cast<const char*>(
					u8"日本語で応答してください。") }},
				{{"role", "user"}, {"content", utf8UserMessage}}
			})}
	};
	std::string body = requestBody.dump();

	// 1回の呼び出しにつきハンドルを作って使い捨てる、一番素朴な使い方。
	// (使い回して高速化する場合はcurl_easy_resetやマルチハンドルを使うが、まずはこの形で動きを覚える)
	CURL* curl = curl_easy_init();
	if (!curl)return "ERROR: curl_easy_init failed";

	// リクエストヘッダはcurl_slist(単方向リンクリスト)で組み立ててCURLOPT_HTTPHEADERに渡す。
	struct curl_slist* headers = nullptr;
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "Authorization: Bearer " LLAMA_SERVER_API_KEY);

	std::string responseBody;

	curl_easy_setopt(curl, CURLOPT_URL, LLAMA_SERVER_URL);
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)body.size());
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToString);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseBody);
	// 接続確立と全体それぞれにタイムアウトを分けて設定できる(WinHTTP版は読み取り単位のタイムアウトのみ)。
	curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT_MS, 30000L);
	curl_easy_setopt(curl, CURLOPT_TIMEOUT_MS, 120000L);

	CURLcode res = curl_easy_perform(curl);

	std::string result;
	if (res != CURLE_OK)
	{
		result = std::string("ERROR: curl_easy_perform failed - ") + curl_easy_strerror(res);
	}
	else
	{
		try
		{
			nlohmann::json responseJson = nlohmann::json::parse(responseBody);
			result = responseJson["choices"][0]["message"]["content"].get<std::string>();
		}
		catch (const std::exception& e)
		{
			result = std::string("Error: JSON parse failed - ") + e.what() + " / body=" + responseBody;
		}
	}

	curl_slist_free_all(headers);
	curl_easy_cleanup(curl);
	return result;
}
