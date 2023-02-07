#include "stdafx.h"
#include "GameManager.h"

void GameManager::Initialize()
{
	m_currentState = GameState::Unpaused;
	m_currentPhase = Phase::DayPhase;
	m_currentDay = 1;
}

void GameManager::SetCurrentState(GameState state)
{
	m_currentState = state;
	EventSystem::Instance()->AddEvent(EVENTID::CurrentState, &m_currentState);
}

void GameManager::SetNextDay()
{
	if (m_currentDay <= 5)
		m_currentDay++;
	else
		m_currentDay = 1;

	EventSystem::Instance()->AddEvent(EVENTID::CurrentDay, &m_currentDay);
}

void GameManager::SetPhase(Phase phase)
{
	m_currentPhase = phase;
	EventSystem::Instance()->AddEvent(EVENTID::CurrentPhase, &m_currentPhase);
}



GameManager::GameManager()
{
}

GameManager::~GameManager()
{
	EventSystem::Instance()->RemoveClient(EVENTID::GameLevelChangeEvent, this);
	EventSystem::Instance()->RemoveClient(EVENTID::ChangePhase, this);
	EventSystem::Instance()->RemoveClient(EVENTID::NextDay, this);
	EventSystem::Instance()->RemoveClient(EVENTID::GameRestartEvent, this);
}

void GameManager::HandleEvent(Event* event)
{
	switch (event->GetEventID())
	{
	case EVENTID::GameLevelChangeEvent:
		SetCurrentState(*static_cast<GameState*>(event->GetData()));
		break;
	case EVENTID::ChangePhase:
		SetPhase(*static_cast<Phase*>(event->GetData()));
		break;
	case EVENTID::NextDay:
		SetNextDay();
		break;
	case EVENTID::GameRestartEvent:
		Initialize(); // reinitialize game manager
		break;
	default:
		break;
	}
}

void GameManager::AddToEvent() noexcept
{
	EventSystem::Instance()->AddClient(EVENTID::GameLevelChangeEvent, this);
	EventSystem::Instance()->AddClient(EVENTID::ChangePhase, this);
	EventSystem::Instance()->AddClient(EVENTID::NextDay, this);
	EventSystem::Instance()->AddClient(EVENTID::GameRestartEvent, this);
}
