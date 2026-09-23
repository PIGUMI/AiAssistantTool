#include "History.h"
#include "../Helper/Helper.h"

History::History()
{
	m_history.clear();
	m_count = 1;
	m_history["messages"][0]["role"] = "system";
	m_history["messages"][0]["content"] = Helper::AnsiToUtf8("“ú–{Œê‚Å•Ô“š‚µ‚Ä‚­‚¾‚³‚¢");
}

History::~History()
{
}

void History::AddHistory(const std::string& role, const std::string& content)
{
	nlohmann::json message;
	message["role"] = role;
	message["content"] = content;
	m_history["messages"][m_count] = message;
	m_count++;
}

nlohmann::json History::GetHistoryJson() const
{
	return m_history["messages"];
}
