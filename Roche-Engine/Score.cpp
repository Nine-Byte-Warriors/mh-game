#include "stdafx.h"
#include "Score.h"
#include "Input/Input.h"
#include "stdio.h"
#include "Utility/Helper/JsonLoading.h"

using json = nlohmann::json;

void HighScoreTable::AddToEvent() noexcept
{
	EventSystem::Instance()->AddClient(EVENTID::AddScoreToPlayer, this);
}

void HighScoreTable::RemoveFromEvent() noexcept
{
	EventSystem::Instance()->RemoveClient(EVENTID::AddScoreToPlayer, this);
}

void HighScoreTable::HandleEvent(Event* event)
{
	switch (event->GetEventID())
	{
	case EVENTID::AddScoreToPlayer:
		AddScoreToPlayerFunct(m_iAddedScore);
		break;
	}
}

void HighScoreTable::AddScoreToPlayerFunct(int m_ithisScore)
{
	m_iScore += m_ithisScore;
	}

void HighScoreTable::AddScoreToPlayerFunct(int m_ithisScore)
{
	m_iScore += m_ithisScore;
}

void HighScoreTable::SaveScore(const std::vector<Score>& players, const std::string& file_name)
{
	json j;

	std::vector<Score> score1;

	for (const auto& player : players)
	{
		j.push_back({ {"name", player.name},{"score", player.score} });
	}

	std::ofstream ofs(file_name);
	ofs << j.dump(4) << std::endl;
}

std::vector<Score> load_score_fromFile(const std::string& file_name)
{
	std::vector<Score> scores;

	std::ifstream ifs(file_name);
	if (ifs.good())
	{
		json j;
		ifs >> j;

		for (const auto& element : j)
		{
			scores.push_back({ element["name"], element["score"] });
		}
	}
	return scores;
}


void HighScoreTable::AddScoreCalculation() {

	HighScoreTable scoreM;

	std::vector<Score> scores = load_score_fromFile(scoreM.m_sFilePath);

	std::string player_name;
	int playerScore;

	std::cout << "Enter player name: ";
	std::cin >> m_sName;

	scores.push_back({ m_sName,m_iScore });
	std::sort(scores.begin(), scores.end());

	scoreM.SaveScore(scores, scoreM.m_sFilePath);

	std::cout << "Leaderboard:" << std::endl;
	for (const auto& player : scores) {
		std::cout << player.name << ": " << player.score << std::endl;
	}
}






