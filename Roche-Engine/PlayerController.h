#pragma once
#ifndef PLAYERCONTROLLER_H
#define PLAYERCONTROLLER_H

class Entity;
class PlayerMovement;
#include "PlayerMovement.h"
#include "Entity.h"

class PlayerController
{
public:
	void Initilise(EntityController& entityController, int num, std::shared_ptr<Physics> &physics, std::shared_ptr<Sprite> &sprite);
	
	void Update(const float dt);

private:
	EntityController* m_EntityController;
	std::shared_ptr<PlayerMovement> m_playerMovement;

	int m_iEntityNum;
};

#endif 