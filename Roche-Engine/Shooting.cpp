#include "stdafx.h"
#include "Shooting.h"

Shooting::Shooting()
{
	AddToEvent();
}

Shooting::~Shooting()
{
	EventSystem::Instance()->RemoveClient(EVENTID::PlayerPosition, this);
	EventSystem::Instance()->RemoveClient(EVENTID::MousePosition, this);
	EventSystem::Instance()->RemoveClient(EVENTID::PlayerFire, this);
}

void Shooting::Shoot(Vector2f Targetloc)
{
	std::shared_ptr<Projectile> pProjectile;

	if (pProjectile != nullptr)
		pProjectile->SpawnProjectile(1, 1);
}


void Shooting::AddToEvent() noexcept
{
	EventSystem::Instance()->AddClient(EVENTID::PlayerPosition, this);
	EventSystem::Instance()->AddClient(EVENTID::MousePosition, this);
	EventSystem::Instance()->AddClient(EVENTID::PlayerFire, this);
}

void Shooting::HandleEvent(Event* event)
{
	switch (event->GetEventID())
	{
	case EVENTID::PlayerPosition:

		break;
	case EVENTID::MousePosition:

		break;
	/*case EVENTID::PlayerFire:
		
		break;*/
	default:
		break;
	}
}


