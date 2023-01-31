#pragma once
#ifndef PLAYERMOVEMENT_H
#define PLAYERMOVEMENT_H

class Entity;
#include "Physics.h"
#include "Events/EventSystem.h"
#include "Entity.h"

class PlayerMovement : public Listener
{
public:
	void Initilise(EntityController& entityController, float speed, int entityNum, std::shared_ptr<Physics> &physics, std::shared_ptr<Sprite> &sprite);
	~PlayerMovement();

	void Update(const float dt);
	void SetSpeed(float speed);
	void Dash();

	// Inherited via Listener
	virtual void HandleEvent(Event* event) override;
	void AddToEvent() noexcept;

private:
	std::shared_ptr<Physics> m_physics;
	std::shared_ptr<Sprite> m_sprite;
	EntityController* m_entityController;
	float m_speed;
	float m_currentSpeed;
	float m_dashSpeed;
	bool m_bDash;
	bool m_bshouldMove;
	int m_iEntityNum;

	enum PlayerDirection
	{
		Down = 0,
		Left = 1,
		Right = 2,
		Up = 3
	};

};

#endif