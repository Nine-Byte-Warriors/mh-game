#include "stdafx.h"
#include "Health.h"
#include "AudioEngine.h"
#include "Collider.h"

#define PLAYER "Player"
#define ENEMY "EnemyCarrot"


Health::Health(std::string type, int entityNum, std::shared_ptr<Collider> collider,std::string name)
{
	m_sType = type;
	m_iEntityNum = entityNum;
	m_collider = collider;
	FilterName(name);
	m_sName = name;
	std::function<void(Collider&)> f = std::bind(&Health::Hit, this, std::placeholders::_1);
	collider->AddOnEnterCallback(f);
	AddToEvent();
}

void Health::SetHealth( float maxHealth )
{
	m_fMaxHealth = maxHealth;
	m_fCurrentHealth = m_fMaxHealth;
}

void Health::TakeDamage( float damageAmount )
{
	m_fCurrentHealth -= damageAmount;
#if _DEBUG
	std::string currhealth = std::to_string(m_fCurrentHealth) + "\n";
	OutputDebugStringA(currhealth.c_str());
#endif

	if ( m_fCurrentHealth <= 0 && !m_bIsDead)
	{
		m_fCurrentHealth = 0;
		if (m_sType == "Player")
		{
			AudioEngine::GetInstance()->PlayAudio(PLAYER, "EntityDeath", SFX);
			EventSystem::Instance()->AddEvent(EVENTID::PlayerDeath, this);
		}
		else if (m_sType == "Enemy")
		{
			AudioEngine::GetInstance()->PlayAudio(ENEMY, "EntityDeath", SFX);
#if _DEBUG
			std::string outputdead = m_sType + " " + std::to_string(m_iEntityNum) + " Has Died! \n";
			OutputDebugStringA(outputdead.c_str());
#else
			EventSystem::Instance()->AddEvent( EVENTID::EnemyDeath, &m_iEntityNum );
#endif
			
			EventSystem::Instance()->AddEvent(EVENTID::UpdateScore, &m_iEnemyScoreReward);
			EventSystem::Instance()->AddEvent(EVENTID::GainCoins, &m_iEnemyMoneyReward);
		}

		if(m_sName == "Corn")
			EventSystem::Instance()->AddEvent(EVENTID::CharacterDead, &m_sName);

		m_bIsDead = true;
	}
}

void Health::Heal( float healAmount )
{
	OutputDebugStringA("HEAL");
	m_fCurrentHealth += healAmount;
	if ( m_fCurrentHealth >= m_fMaxHealth )
		m_fCurrentHealth = m_fMaxHealth;
}

void Health::Hit(Collider& collider)
{
	if (collider.EntityType() == "PlayerProjectile" && m_sType == "Enemy")
	{
		AudioEngine::GetInstance()->PlayAudio(ENEMY, "EntityHit", SFX);
		TakeDamage(1);
	}

	if (collider.EntityType() == "EnemyProjectile" && m_sType == "Player")
	{
		AudioEngine::GetInstance()->PlayAudio(PLAYER, "EntityHit", SFX);
		TakeDamage(1);
	}
	if (collider.EntityType() == "Enemy" && m_sType == "Player")
	{
		AudioEngine::GetInstance()->PlayAudio(PLAYER, "EntityHit", SFX);
		TakeDamage(1);
	}
}

void Health::SetEntityNum(int num)
{
	m_iEntityNum = num;
}

void Health::FilterName(std::string name)
{
	if (name.contains("Carrot"))
	{
		m_iEnemyMoneyReward =3;
		m_iEnemyScoreReward = 20;
		return;
	}
	if (name.contains("Potato"))
	{
		m_iEnemyMoneyReward = 10;
		m_iEnemyScoreReward = 80;
		return;
	}
	if (name.contains("Bean"))
	{
		m_iEnemyMoneyReward = 9;
		m_iEnemyScoreReward = 50;
		return;
	}
	if (name.contains("Onion"))
	{
		m_iEnemyMoneyReward = 5;
		m_iEnemyScoreReward = 30;
		return;
	}
	if (name.contains("Cauliflower"))
	{
		m_iEnemyMoneyReward = 5;
		m_iEnemyScoreReward = 40;
		return;
	}
	if (name.contains("Tomato"))
	{
		m_iEnemyMoneyReward = 8;
		m_iEnemyScoreReward = 50;
		return;
	}
	if (name.contains("Corn"))
	{
		m_iEnemyMoneyReward = 30;
		m_iEnemyScoreReward = 1000;
		return;
	}
}

void Health::AddToEvent() noexcept
{
	EventSystem::Instance()->AddClient( EVENTID::PlayerDamage, this );
	EventSystem::Instance()->AddClient( EVENTID::PlayerHeal, this );
	EventSystem::Instance()->AddClient(EVENTID::SavePlayerHealth, this);
	EventSystem::Instance()->AddClient(EVENTID::GetPlayerHealth, this);
}

void Health::RemoveFromEvent() noexcept
{
	EventSystem::Instance()->RemoveClient( EVENTID::PlayerDamage, this );
	EventSystem::Instance()->RemoveClient( EVENTID::PlayerHeal, this );
	EventSystem::Instance()->RemoveClient(EVENTID::SavePlayerHealth, this);
	EventSystem::Instance()->RemoveClient(EVENTID::GetPlayerHealth, this);
}

void Health::HandleEvent( Event* event )
{
	switch ( event->GetEventID() )
	{
	case EVENTID::PlayerDamage:
		if ( m_sType == "Player" )
			TakeDamage( 1 );
		break;
	case EVENTID::PlayerHeal:
		if ( m_sType == "Player" )
			Heal( 0.5 );
		break;
	case EVENTID::SavePlayerHealth:
		if (m_sType == "Player")
		{
			EventSystem::Instance()->AddEvent(EVENTID::SetPlayerHealth, &m_fCurrentHealth);
		}
		break;
	case EVENTID::GetPlayerHealth:
		if (m_sType == "Player")
		{
			m_fCurrentHealth = *static_cast<float*>(event->GetData());
		}
		break;
	}
}