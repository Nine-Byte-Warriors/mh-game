#pragma once
#include "Utility/Helper/JsonLoading.h"
#include "EventSystem.h"
#include <iostream>
#include <set>
#include <vector>
#include <string>
#include <fstream>

struct Score
{
	std::string name;
	int score;

	bool operator<(const Score& other)const { return score > other.score; }
};

class HighScoreTable : Listener
{
private:

	std::vector<std::string> log_lines;

	std::vector<Score> score1;

	const int MAX_NAMES = 0;



	void AddToEvent() noexcept;
	void RemoveFromEvent() noexcept;
	void HandleEvent(Event* event) override;


public:
	int m_iAddedScore;
	std::string m_sFilePath = "Resources\\HighScore.json";

	// The scores
	std::multiset<Score, std::greater<Score> > Scores;

	//last name that was written
	std::string m_sName;
	int m_iScore = 1;


	void SaveScore(const std::vector<Score>& players, const std::string& file_name);

	void SaveScoreButton();

	void AddScoreToPlayerFunct(int m_ithisScore);

	void AddScoreCalculation();

	int m_iHighScore(int m_iscore);

	HighScoreTable() {};
};
