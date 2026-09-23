#pragma once
/**
 * @brief 履歴管理クラス
 */

#include <vector>
#include <nlohmann/json.hpp>

class History
{
public:
	History();
	~History();
public:

	/**
	 * @brief 履歴を追加する
	 * @param role ロール
	 * @param content コンテンツ
	 */
	void AddHistory(const std::string& role, const std::string& content);


	nlohmann::json GetHistoryJson() const;


	int GetCount() const { return m_count; }


protected:
	nlohmann::json m_history;
	int m_count = 0;
};

