#include "stdafx.h"
#include "Inventory.h"
#include "AudioEngine.h"

#define SHOPSOUNDS "ShopSounds"
#define PLAYER "Player"
Inventory::Inventory(std::string type)
{
	m_sType = type;
	AddToEvent();
	m_vSeedOptions.emplace(std::pair<std::string, int>{ "Bean", 0 });
	m_vSeedOptions.emplace( std::pair<std::string, int>{ "Carrot", 0 } );
	m_vSeedOptions.emplace(std::pair<std::string, int>{ "Cauliflower", 0 });
	m_vSeedOptions.emplace( std::pair<std::string, int>{ "Onion", 0 } );
	m_vSeedOptions.emplace( std::pair<std::string, int>{ "Potato", 0 } );
	m_vSeedOptions.emplace( std::pair<std::string, int>{ "Tomato", 0 } );
	m_iCurrentSeed = 0;
	m_iCoinAmount = 20;
}

Inventory::~Inventory() { RemoveFromEvent(); }

void Inventory::SetActiveSeedPacket( int currSeed )
{
	m_iCurrentSeed = currSeed;
	std::fill( m_vSelectedSeeds.begin(), m_vSelectedSeeds.end(), false );
	m_vSelectedSeeds[m_iCurrentSeed] = true;
}

std::string Inventory::GetTexture()
{
	std::string texture = "Resources\\Textures\\Tiles\\Full" + GetName() + ".png";
	return texture;
}

std::string Inventory::GetName()
{
	int index = 0;
	for (const auto& [key, value] : m_vSeedOptions)
	{
		if (index == m_iCurrentSeed)
		{
			return key;
		}
		index++;
	}
	return "None";
}

void Inventory::UpdateActiveSeedPacket( int currSeed )
{
	if ( currSeed > m_vSeedOptions.size() - 1 )
		m_iCurrentSeed = 0;
	if ( currSeed < 0 )
		m_iCurrentSeed = m_vSeedOptions.size() - 1;
	SetActiveSeedPacket( m_iCurrentSeed );
}

bool Inventory::UpdateActiveSeedPacketCount()
{
	// Check that is a seed to be planted
	auto it = m_vSeedOptions.begin();
	std::advance( it, m_iCurrentSeed );
	if ( it->second > 0 ) return false;

	for ( auto& [key, value] : m_vSeedOptions )
	{
		if ( it->first == key )
		{
			value -= 1;
			return true;
		}
	}

	return true;
}

void Inventory::PlantSeedFromPacket( std::string& seedName, int amountPlanted )
{
	auto it = m_vSeedOptions.begin();
	std::advance( it, m_iCurrentSeed );
	if ( it->second <= 0 )
		return;

	AudioEngine::GetInstance()->PlayAudio(PLAYER, "Planting", SFX);

	ChangeSeedPacketValue( seedName, -amountPlanted );
	EventSystem::Instance()->AddEvent( EVENTID::PlantSeed, &seedName );
}

void Inventory::BuySeedPacket( const std::string& seedName, int amountBought )
{
	AudioEngine::GetInstance()->PlayAudio(SHOPSOUNDS, "ShopPurchase", SFX);
	ChangeSeedPacketValue(seedName, amountBought);
	EventSystem::Instance()->AddEvent(EVENTID::SetPlayerInventory, &m_vSeedOptions);
}

void Inventory::ChangeSeedPacketValue( const std::string& seedName, int amountToChange )
{
	for ( auto& [key, value] : m_vSeedOptions )
	{
		if ( seedName == key)
		{
			value += amountToChange;
			return;
		}
	}
}

void Inventory::UpdateCoins(int amountToChange)
{
	m_iCoinAmount += amountToChange;
}

void Inventory::LoadInventory()
{
	EventSystem::Instance()->AddEvent(EVENTID::LoadPlayerInventory);
}

void Inventory::AddToEvent() noexcept
{
	EventSystem::Instance()->AddClient( EVENTID::IncrementSeedPacket, this );
	EventSystem::Instance()->AddClient( EVENTID::DecrementSeedPacket, this );
	EventSystem::Instance()->AddClient( EVENTID::PlantSeedAttempt, this );
	EventSystem::Instance()->AddClient( EVENTID::UpdateSeed, this );
	EventSystem::Instance()->AddClient(EVENTID::GainCoins, this);
	EventSystem::Instance()->AddClient(EVENTID::BuyPotion, this);
	EventSystem::Instance()->AddClient(EVENTID::SavePlayerInventory, this);
	EventSystem::Instance()->AddClient(EVENTID::SavePlayerMoney, this);
	EventSystem::Instance()->AddClient(EVENTID::GetPlayerMoney, this);
}

void Inventory::RemoveFromEvent() noexcept
{
	EventSystem::Instance()->RemoveClient( EVENTID::IncrementSeedPacket, this );
	EventSystem::Instance()->RemoveClient( EVENTID::DecrementSeedPacket, this );
	EventSystem::Instance()->RemoveClient( EVENTID::PlantSeedAttempt, this );
	EventSystem::Instance()->RemoveClient( EVENTID::UpdateSeed, this );
	EventSystem::Instance()->RemoveClient(EVENTID::GainCoins, this);
	EventSystem::Instance()->RemoveClient(EVENTID::BuyPotion, this);
	EventSystem::Instance()->RemoveClient(EVENTID::SavePlayerInventory, this);
	EventSystem::Instance()->RemoveClient(EVENTID::SavePlayerMoney, this);
	EventSystem::Instance()->RemoveClient(EVENTID::GetPlayerMoney, this);
}

void Inventory::HandleEvent( Event* event )
{
	switch ( event->GetEventID() )
	{
	case EVENTID::IncrementSeedPacket:
		m_iCurrentSeed++;
		UpdateActiveSeedPacket( m_iCurrentSeed );
		break;
	case EVENTID::DecrementSeedPacket:
		m_iCurrentSeed--;
		UpdateActiveSeedPacket( m_iCurrentSeed );
		break;
	case EVENTID::PlantSeedAttempt:
	{
		std::pair<std::string, int>* seedsPlanted = static_cast<std::pair<std::string, int>*>( event->GetData() );
		PlantSeedFromPacket( seedsPlanted->first, seedsPlanted->second );
	}
	break;
	case EVENTID::UpdateSeed:
	{
		std::pair<std::string, int>* seedsBought = static_cast<std::pair<std::string, int>*>( event->GetData() );

		if (seedsBought->first.contains("Carrot") && m_iCoinAmount >= 1)
		{
			UpdateCoins(-1);
			BuySeedPacket("Carrot", seedsBought->second);
		}
		if (seedsBought->first.contains("Potato") && m_iCoinAmount >= 5)
		{
			UpdateCoins(-5);
			BuySeedPacket("Potato", seedsBought->second);
		}
		if (seedsBought->first.contains("Bean") && m_iCoinAmount >= 5)
		{
			UpdateCoins(-5);
			BuySeedPacket("Bean", seedsBought->second);
		}
		if (seedsBought->first.contains("Onion") && m_iCoinAmount >= 2)
		{
			UpdateCoins(-2);
			BuySeedPacket("Onion", seedsBought->second);
		}
		if (seedsBought->first.contains("Cauliflower") && m_iCoinAmount >= 2)
		{
			UpdateCoins(-2);
			BuySeedPacket("Cauliflower", seedsBought->second);
		}
		if (seedsBought->first.contains("Tomato") && m_iCoinAmount >= 4)
		{
			UpdateCoins(-4);
			BuySeedPacket("Tomato", seedsBought->second);
		}
	}
	break;
	case EVENTID::BuyPotion:
	{
		if (m_iCoinAmount >= 5 && m_sType == "Player")
		{
			UpdateCoins(-5);
			AudioEngine::GetInstance()->PlayAudio(SHOPSOUNDS, "ShopPurchase", SFX);
			EventSystem::Instance()->AddEvent(EVENTID::PlayerHeal);
		}
	}
	break;
	case EVENTID::GainCoins:
	{
		UpdateCoins(*static_cast<int*>(event->GetData()) );
	}
	break;
	case EVENTID::SavePlayerInventory:
	{
		std::map<std::string, int> tempMap = *static_cast<std::map<std::string, int>*>(event->GetData());

		if (tempMap.size() == m_vSeedOptions.size())
		{
			m_vSeedOptions = tempMap;
		}
	}
	break;
	case EVENTID::SavePlayerMoney:
	{
		EventSystem::Instance()->AddEvent(EVENTID::SetPlayerMoney, &m_iCoinAmount);
	}
	break;
	case EVENTID::GetPlayerMoney:
		if (m_sType == "Player")
		{
			m_iCoinAmount = *static_cast<int*>(event->GetData());
		}
		break;
	default: break;
	}
}