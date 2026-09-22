#pragma once

#include <string>


/*
* llama-serverのOpenAI互換エンドポイントにCurl(libcurl)でプロンプトを投げて応答を受け取る。
* llamaServer.h (WinHTTP版) とは独立した実装。まずは非ストリーミングの素朴な呼び出しのみ。
*/

namespace LocalLLM
{
	/**
	 * @brief llama-serverのOpenAI互換エンドポイントにCurlでプロンプトを投げて応答を受け取る
	 * @param utf8UserMessage
	 * @return
	 */
	std::string CallLlamaServerCurl(const std::string& utf8UserMessage);
}
