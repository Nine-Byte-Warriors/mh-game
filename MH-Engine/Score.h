#pragma once
#include "Utility/Helper/JsonLoading.h"
#include <iostream>
#include <set>
#include <string>

struct Score
{
	std::string m_sName;
	int m_iScore;

};
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Score, m_sName, m_iScore)

class HighScoreTable
{
private:

	std::vector<Score> score1;

	int m_iNumberofNames;

	std::string m_sFilePath = "Resources\\HighScore\\jsonHighScore.json";

public:


	//last name that was written
	std::string m_sName;
	int m_iScore;


	std::multiset<Score, std::greater<Score>> Scores;

	void NewScore(std::string m_sName, int m_iScore, int m_iEnemy);

	void SaveScore(std::string m_sfilePath);

	void SaveScoreButton();
	void DisplayScores(std::string m_sNamesArray[], int m_iScoresArray[], int m_iCount);
	void SortScores(std::string m_sNamesArray[], int m_iScoresArray[], int m_iCount);
	void AddScore(std::string m_sNamesArray[], int m_iScoresArray[], int& m_iCount, int m_iSize, std::string m_sName, int m_iScore);
	int FindName(std::string m_sNames, std::string m_sNamesArray[], int m_iCount);

	int m_iHighScore(int m_iscore);

	HighScoreTable() {}
};
