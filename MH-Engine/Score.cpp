#include "stdafx.h"
#include "Score.h"
#include "Input/Input.h"


//void HighScoreTable::AddScore(int m_iScoreToAdd)
//{
//	Score m_sScore;
//
//	 
//
//	m_iScoreToAdd = 100;
//
//	m_sScore.m_iScore += m_iScoreToAdd;
//
//	std::cout << "Score" << m_sScore.m_iScore;
//};
//
//int HighScoreTable::m_iHighScore(int m_iscore)
//{
//	int m_iPos = 1;
//	for (auto i = Scores.begin(); i != Scores.end() && m_iscore <= i->m_iScore; i++, m_iPos++);
//
//	return (m_iPos <= m_iNumberofNames ? m_iPos : 0);
//}

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
		AddScoreToPlayerFunct(m_iScore);
		break;
	}
}

void HighScoreTable::SaveScore(std::string m_sfilePath)
{
	std::vector<Score> score1;
	score1.push_back({ "Lef", 100 });
	JsonLoading::SaveJson(score1, m_sfilePath);
}



void HighScoreTable::AddScoreToPlayerFunct(int m_ithisScore)
{
	m_iScore += m_ithisScore;
}

void HighScoreTable::DisplayScores(std::string m_sNamesArray[], int m_iScoresArray[], int m_iCount)
{
	EventSystem::Instance()->AddEvent(EVENTID::AddScoreToPlayer);

	std::cout << "Leaderboard" << std::endl;
	for (int i = m_iCount - 1; i >= 0; i--)
	{
		std::cout << m_sNamesArray[i] << " " << m_iScoresArray[i] << std::endl;
	}
}

void HighScoreTable::SortScores(std::string m_sNamesArray[], int m_iScoresArray[], int m_iCount)
{



	for (int i = 1; i < m_iCount; i++)
	{
		std::string m_sName = m_sNamesArray[i];
		int m_iScore = m_iScoresArray[i];

		int m_iArrayback = i - 1;
		while (m_iArrayback >= 0 && m_iScoresArray[m_iArrayback] > m_iScore)
		{
			m_sNamesArray[m_iArrayback + 1] = m_sNamesArray[m_iArrayback];
			m_iScoresArray[m_iArrayback + 1] = m_iScoresArray[m_iArrayback];
			m_iArrayback--;
		}

		m_sNamesArray[m_iArrayback + 1] = m_sName;
		m_iScoresArray[m_iArrayback + 1] = m_iScore;

	}
}

void HighScoreTable::AddScore(std::string m_sNamesArray[], int m_iScoresArray[], int& m_iCount, int m_iSize, std::string m_sName, int m_iScore)
{
	int m_iIndex;
	m_iIndex = FindName(m_sName, m_sNamesArray, m_iCount);

	if (m_iIndex >= 0)
	{
		if (m_iScoresArray[m_iIndex] < m_iScore)
		{
			m_iScoresArray[m_iIndex] = m_iScore;
			SortScores(m_sNamesArray, m_iScoresArray, m_iCount);
		}
	}
	else
	{
		if (m_iCount < 1)
		{
			m_iScoresArray[0] = m_iScore;
			m_sNamesArray[0] = m_sName;
			m_iCount++;
		}
		else
		{
			if (m_iCount = m_iSize)
			{
				if (m_iScore > m_iScoresArray[0])
				{
					m_iScoresArray[0] = m_iScore;
					m_sNamesArray[0] = m_sName;
				}
			}
			else
			{
				m_iScoresArray[m_iCount] = m_iScore;
				m_sNamesArray[m_iCount] = m_sName;
				m_iCount++;
			}

			SortScores(m_sNamesArray, m_iScoresArray, m_iCount);

		}
	}

}

int HighScoreTable::FindName(std::string m_sName, std::string m_sNamesArray[], int m_iCount)
{
	int m_iIndex = -1;

	for (int i = 0; i < m_iCount; i++)
	{
		if (m_sName.compare(m_sNamesArray[i]) == 0) m_iIndex = i;
	}

	return m_iIndex;
}
