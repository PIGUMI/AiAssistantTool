#pragma once
/*
* クラス名 : Controller
* curlを使用して、llama.cppのAPIを呼び出すクラス
* 制作者：秋野翔太
*/

#include <curl/curl.h>
#include <string>

class History;


class Controller
{
public:
	/**
	 * @brief コンストラクタ
	 */
	Controller();

	/**
	 * @brief デストラクタ
	 */
	~Controller();

	std::string Run(std::string message);

protected:
	CURL* m_pCurl = nullptr;
	History* m_pHistory = nullptr;
};

