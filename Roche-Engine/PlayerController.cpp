#include "stdafx.h"
#include "PlayerController.h"

void PlayerController::Initilise(EntityController& entityController, int num, std::shared_ptr<Physics> &physics, std::shared_ptr<Sprite> &sprite)
{
	m_iEntityNum = num;
	m_EntityController = &entityController;
	m_playerMovement = std::make_shared<PlayerMovement>();
	m_playerMovement->Initilise(*m_EntityController, 10, m_iEntityNum, physics, sprite);
}

void PlayerController::Update(const float dt)
{
	m_playerMovement->Update(dt);
}
 