#include "Controller.h"
#include <iostream>
#include <nlohmann/json.hpp>
#include "../Helper/Helper.h"
#include "History.h"

const char url[] = "http://192.168.0.13:8081/v1/chat/completions";
const char model[] = "gemma-4-E4B-it-q4_0";

size_t write_callback(char* ptr, size_t size, size_t nmemb, void* userdata)
{
	// 型を変換して、ユーザーデータを取得する
	std::string* responseData = static_cast<std::string*>(userdata);
	responseData->append(ptr, size * nmemb);

	return size * nmemb;
}

Controller::Controller()
{
	curl_global_init(CURL_GLOBAL_DEFAULT);
	m_pCurl = curl_easy_init();
	m_pHistory = new History();
}

Controller::~Controller()
{
	curl_easy_cleanup(m_pCurl);
	curl_global_cleanup();
}


std::string Controller::Run(std::string message)
{
	// 履歴にユーザーのメッセージを追加する

	m_pHistory->AddHistory("user", Helper::AnsiToUtf8(message));
	
	// URLを設定する
	curl_easy_setopt(m_pCurl, CURLOPT_URL, url);
	// POSTリクエストを送信するための設定
	curl_easy_setopt(m_pCurl, CURLOPT_POST, 1L);

	nlohmann::json requestBody;
	requestBody["model"] = model;
	// システムメッセージを設定する
	requestBody["messages"] = m_pHistory->GetHistoryJson();
	
	std::string body = requestBody.dump();

	// POSTデータを設定する
	curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDS, body.c_str());
	curl_easy_setopt(m_pCurl, CURLOPT_POSTFIELDSIZE, body.size());

	std::string responseData;
	// レスポンスを標準出力に出力するためのコールバック関数を設定する
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEFUNCTION, write_callback);
	curl_easy_setopt(m_pCurl, CURLOPT_WRITEDATA, &responseData);

	CURLcode log = curl_easy_perform(m_pCurl);

	if (log != CURLE_OK)
	{
		std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(log) << std::endl;
		return "ERROR";
	}

	nlohmann::json responseJson = nlohmann::json::parse(responseData);
	std::string content = responseJson["choices"][0]["message"]["content"].get<std::string>();

	m_pHistory->AddHistory("assistant", content);

	return content;
}