#pragma once
#ifndef ENTITY_H
#define ENTITY_H

class Graphics;
#include "EntityController.h"
#include "Physics.h"
#include "EventSystem.h"
#include "ProjectileManager.h"
#include "Collider.h"
#include "Agent.h"
#include "Player.h"

enum class EntityType
{
	Player,
	Enemy,
	Projectile
};

class Entity : public Listener
{
public:
	Entity(EntityController& entityController, int EntityNum);
	~Entity();

	void Initialize(const Graphics& gfx, ConstantBuffer<Matrices>& mat);
	void Update(const float dt);
	void UpdateFromEntityData(const float dt, bool positionLocked);

	EntityType GetEntityType();

	inline std::shared_ptr<Agent> GetAI() const noexcept { return m_agent; }
	inline std::shared_ptr<Sprite> GetSprite() const noexcept { return m_sprite; }
	inline std::shared_ptr<Physics> GetPhysics() const noexcept { return m_physics; }
	inline std::shared_ptr<Transform> GetTransform() const noexcept { return m_transform; }
	inline std::shared_ptr<ProjectileManager> GetProjectileManager() const noexcept { return m_projectileManager;	}
	inline std::shared_ptr<Collider> GetCollider() const noexcept { return m_collider; };
	//inline std::shared_ptr<BoxCollider> GetCollider() const noexcept { return m_boxCollider; };

	Vector2f GetPos() { return *m_vPosition; }

	void AddToEvent() noexcept;
	void HandleEvent(Event* event) override;

private:
	void SetPositionInit();
	void SetScaleInit();

	void UpdatePosition();
	void UpdateScale();
	void UpdateFrame();
	void UpdateTexture();

	void UpdateMass();
	void UpdateSpeed();

	void UpdateType();
	void UpdateBehaviour();

	void UpdateProjectilePattern();

	void UpdateColliderShape();
	void UpdateColliderRadius();

	void UpdatePlayer(const float dt);
	void UpdateEnemy(const float dt);
	void UpdateProjectile(const float dt);

	int m_iEntityNum;
	EntityType m_entityType;

	ID3D11Device* m_device;

	Vector2f* m_vPosition;
	float m_fScaleX;
	float m_fScaleY;
	int m_iMaxFrameX;
	int m_iMaxFrameY;
	std::string m_sTex;

	std::string m_sBulletTex;
	float m_fBulletScaleX;
	float m_fBulletScaleY;
	int m_iBulletMaxFrameX;
	int m_iBulletMaxFrameY;
	float m_fBulletMass;
	float m_fBulletSpeed;
	std::string m_sBulletPattern;

	float m_fMass;
	float m_fSpeed;

	std::string m_sBehaviour;
	std::string m_sEntityType;

	std::string m_sColliderShape;
	float m_fColliderRadiusX;
	float m_fColliderRadiusY;

	std::shared_ptr<Agent> m_agent;
	std::shared_ptr<Sprite> m_sprite;
	std::shared_ptr<Physics> m_physics;
	std::shared_ptr<Transform> m_transform;
	std::shared_ptr<ProjectileManager> m_projectileManager;
	std::shared_ptr<CircleCollider> m_collider;

	EntityController* m_entityController;
};

#endif