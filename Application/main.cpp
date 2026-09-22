#include <Window.h>
#include <iostream>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
//{
//}

size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
	// 型を変換して、ユーザーデータを取得する
	std::string* responseData = static_cast<std::string*>(userdata);
	responseData->append(ptr, size * nmemb);
	return size * nmemb;
}

int main()
{
	const char url[] = "http://192.168.0.13:8081/v1/chat/completions";

	curl_global_init(CURL_GLOBAL_DEFAULT);
	CURL* CurlPtr = curl_easy_init();

	// URLを設定する
	curl_easy_setopt(CurlPtr, CURLOPT_URL, url);

	// POSTリクエストを送信するための設定
	curl_easy_setopt(CurlPtr, CURLOPT_POST, 1L);

	nlohmann::json requestBody = {
		{"model", "gemma-4-E4B-it-q4_0"},
		{"messages", {{{"role", "user"}, {"content", "Hello"}}}}
	};

	std::string body = requestBody.dump();


	curl_easy_setopt(CurlPtr, CURLOPT_POSTFIELDS, body.c_str());
	curl_easy_setopt(CurlPtr, CURLOPT_POSTFIELDSIZE, body.size());

	// レスポンスを標準出力に出力するためのコールバック関数を設定する
	curl_easy_setopt(CurlPtr, CURLOPT_WRITEFUNCTION, write_callback); // レスポンスを無視する場合はnullptrを指定
	// レスポンスを標準出力に出力するためのユーザーデータを設定する
	std::string responseData;
	// レスポンスを標準出力に出力するためのユーザーデータを設定する
	curl_easy_setopt(CurlPtr, CURLOPT_WRITEDATA, &responseData); // レスポンスを無視する場合はnullptrを指定

	// API リクエストの実行
	CURLcode log = curl_easy_perform(CurlPtr);


	if (log != CURLE_OK)
	{
		std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(log) << std::endl;
	}


	// データの取り出し
	nlohmann::json responseJson = nlohmann::json::parse(responseData);
	std::string content = responseJson["choices"][0]["message"]["content"].get<std::string>();

	std::cout << "Content: " << content << std::endl;



	curl_easy_cleanup(CurlPtr);

	curl_global_cleanup();

	return 0;

}