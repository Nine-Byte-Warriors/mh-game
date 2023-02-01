#pragma once
#ifndef SHOOTING_H
#define SHOOTING_H

#include "Physics.h"
#include "Events/EventSystem.h"
#include "ProjectileManager.h"
#include "Projectile.h"

class Shooting : Listener
{
public:
	Shooting();
	~Shooting();

	void Shoot(Vector2f Targetloc);

	void AddToEvent() noexcept;
	virtual void HandleEvent(Event* event) override;

	std::shared_ptr<ProjectileManager> pProjectileManager;
	
};
#endif 
