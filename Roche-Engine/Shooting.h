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

private:
	std::shared_ptr<Projectile> GetFreeProjectile();
	std::shared_ptr<ProjectileManager> pProjectileManager;

	//test
	Vector2f pos;
	Vector2f mousePos;
	Vector2f targetpos;
	std::pair<Sprite*, Vector2f*>* charSpriteandPos;
	Sprite* charSprite;
	XMFLOAT2 m_vSizeOfScreen, m_vPosition, m_vInitPosition;
	XMMATRIX m_mOrthoMatrix, m_mWorldMatrix;
	
};
#endif 
