#include "stdafx.h"
#include "Entity.h"
#include "Graphics.h"
#include "EnemyController.h"

#define PI 3.1415

Entity::Entity(EntityController& entityController, int EntityNum)
{
	m_agent = nullptr;

	m_emitter = nullptr;

	m_colliderCircle = nullptr;
	m_colliderBox = nullptr;

	m_pController = nullptr;
	m_inventory = nullptr;

	m_vPosition = new Vector2f();

	m_entityController = &entityController;
	m_iEntityNum = EntityNum;


}

Entity::~Entity()
{
}

void Entity::SetComponents()
{
	m_pSprite = std::make_shared<Sprite>();
	m_pTransform = std::make_shared<Transform>( m_pSprite );
	m_pPhysics = std::make_shared<Physics>(m_pTransform);
	m_health = std::make_shared<Health>( GetType(), m_iEntityNum );
	m_health->SetHealth( m_entityController->GetHealth( m_iEntityNum ) );

	if (m_entityController->HasAI(m_iEntityNum))
	{
		m_agent = std::make_shared<Agent>(m_pPhysics);
	}

	if (m_entityController->HasProjectileSystem(m_iEntityNum))
	{
		std::string projectiletype = "";
		if (GetType() == "Player")
			projectiletype = "PlayerProjectile";
		else if (GetType() == "Enemy")
			projectiletype == "EnemyProjectile";

		m_vecProjectileManagers = ProjectileManager::GenerateManagers(m_entityController->GetProjectilePattern(m_iEntityNum), m_collisionHandler, projectiletype);
		m_emitter = std::make_shared<Emitter>(m_vecProjectileManagers, 0.01f);
	}

	if (m_entityController->HasCollider(m_iEntityNum))
	{
		bool isTrigger = m_entityController->GetColliderTrigger(m_iEntityNum);

		m_colliderCircle = std::make_shared<CircleCollider> (m_pTransform, m_pSprite, isTrigger, m_iEntityNum, m_sEntityType, 32);
		m_colliderBox = std::make_shared<BoxCollider>(m_pTransform, m_pSprite, isTrigger, m_iEntityNum, m_sEntityType, 32, 32);
	}
	else
	{
		m_colliderCircle = nullptr;
		m_colliderBox = nullptr;
	}

	if (m_colliderBox)
		m_health = std::make_shared<Health>(GetType(), m_iEntityNum, m_colliderBox);
	else if (m_colliderCircle)
		m_health = std::make_shared<Health>(GetType(), m_iEntityNum, m_colliderCircle);

	if(m_health)
		m_health->SetHealth( m_entityController->GetHealth(m_iEntityNum) );

	if (GetType() == "Player")
	{
		for (std::shared_ptr<ProjectileManager>& pManager : m_vecProjectileManagers)
			pManager->SetOwner(Projectile::ProjectileOwner::Player);
		
		m_pController = std::make_shared<PlayerController>(m_pPhysics, m_pSprite, m_emitter);
		m_inventory = std::make_shared<Inventory>();
	}

	if (GetType() == "Enemy")
	{
		for (std::shared_ptr<ProjectileManager>& pManager : m_vecProjectileManagers)
			pManager->SetOwner(Projectile::ProjectileOwner::Enemy);

		m_pController = std::make_shared<EnemyController>(m_pPhysics, m_pSprite, m_emitter);
		m_carrotEnemy = std::make_shared<CarrotEnemy>(this);

		m_agent->SetEmitter(m_emitter);

		if (m_entityController->GetName(m_iEntityNum) == "Onion")
		{
			//m_agent->LoadBehaviourFile("Resources\\Behaviours\\DefaultBehaviour.json");
			//m_agent->LoadBehaviourFile("Resources\\Behaviours\\FleeBehaviour.json"); 
			//m_agent->LoadBehaviourFile("Resources\\Behaviours\\SeekBehaviour.json");
			m_agent->LoadBehaviourFile("Resources\\Behaviours\\FireBehaviour.json");
			m_agent->FillStateMachine();
		}
	}

	if (GetType() == "Item")
	{
		m_shopItem = std::make_shared<ShopItem>(GetCollider(), m_entityController->GetName(m_iEntityNum));
	}

	if (GetType() == "LevelTrigger")
	{
		for (std::shared_ptr<ProjectileManager>& pManager : m_vecProjectileManagers)
			pManager->SetOwner(Projectile::ProjectileOwner::LevelTrigger);
		m_levelTrigger = std::make_shared<LevelTrigger>(GetCollider());
	}
}

void Entity::Initialize(const Graphics& gfx, ConstantBuffer<Matrices>& mat)
{
	SetComponents();


	m_device = gfx.GetDevice();
	m_context = gfx.GetContext();
	m_mat = &mat;

	SetProjectileManagerInit(gfx, mat);

	m_pSprite->Initialize(gfx.GetDevice(), gfx.GetContext(), m_entityController->GetTexture(m_iEntityNum), mat);

	SetPositionInit();
	SetScaleInit();
	UpdateRotation();
	UpdateBehaviour();
	UpdateCollider();
	SetAnimation();
	UpdateRowsColumns();
}

void Entity::SetProjectileManagerInit(const Graphics& gfx, ConstantBuffer<Matrices>& mat)
{
	if (m_entityController->HasProjectileBullet(m_iEntityNum) && m_entityController->HasProjectileSystem(m_iEntityNum))
	{
		std::string texture = m_entityController->GetProjectileBullet(m_iEntityNum)->texture;

		for (std::shared_ptr<ProjectileManager>& pManager : m_vecProjectileManagers)
			pManager->InitialiseFromFile(gfx, mat, texture);
	}
}

void Entity::Update(const float dt)
{
	m_pSprite->Update(dt);
	m_pTransform->Update();
	m_pPhysics->Update(dt);

	if (m_entityController->HasAI(m_iEntityNum))
		m_agent->Update(dt);

	if (m_entityController->HasProjectileSystem(m_iEntityNum))
	{
		for (std::shared_ptr<ProjectileManager>& pManager : m_vecProjectileManagers)
			pManager->Update(dt);
	}

	if (m_pController)
		m_pController->Update(dt);
}

std::string Entity::GetType()
{
	return m_entityController->GetType(m_iEntityNum);
}

void Entity::UpdateFromEntityData(const float dt, bool positionLocked)
{
	if (positionLocked)
		UpdatePosition();

	UpdateScale();
	UpdateRotation();
	UpdateMass();
	UpdateBehaviour();
	UpdateSpeed();
	UpdateProjectilePattern();
	UpdateTexture();
	UpdateCollider();
	UpdateAnimation();
	UpdateRowsColumns();
	UpdateAudio();
}

void Entity::SetPositionInit()
{
	m_vPosition->x = m_entityController->GetPosition(m_iEntityNum)[0];
	m_vPosition->y = m_entityController->GetPosition(m_iEntityNum)[1];
	m_pTransform->SetPositionInit(m_vPosition->x, m_vPosition->y);
	m_pTransform->SetPosition(m_vPosition->x, m_vPosition->y);
}

void Entity::SetScaleInit()
{
	m_fScaleX = m_entityController->GetScale(m_iEntityNum)[0];
	m_fScaleY = m_entityController->GetScale(m_iEntityNum)[1];
	m_pSprite->SetWidthHeight( m_fScaleX, m_fScaleY );

	if (m_entityController->HasProjectileBullet(m_iEntityNum))
	{
		m_fBulletScaleX = m_entityController->GetProjectileBullet(m_iEntityNum)->scale[0];
		m_fBulletScaleY = m_entityController->GetProjectileBullet(m_iEntityNum)->scale[1];

		for ( std::shared_ptr<ProjectileManager>& pManager : m_vecProjectileManagers )
			for ( std::shared_ptr<Projectile> pProjectile : pManager->GetProjector() )
				pProjectile->GetSprite()->SetWidthHeight( m_fBulletScaleX, m_fBulletScaleY );
	}
}

void Entity::UpdateRowsColumns()
{
	m_iRows = m_entityController->GetRows(m_iEntityNum);
	m_iColumns = m_entityController->GetColumns(m_iEntityNum);
	m_pSprite->SetRows(m_iRows);
	m_pSprite->SetColumns(m_iColumns);
}

void Entity::UpdatePosition()
{
	m_vPosition->x = m_entityController->GetPosition(m_iEntityNum)[0];
	m_vPosition->y = m_entityController->GetPosition(m_iEntityNum)[1];
	m_pTransform->SetPosition(m_vPosition->x, m_vPosition->y);
}

void Entity::UpdateScale()
{
	m_fScaleX = m_entityController->GetScale(m_iEntityNum)[0];
	m_fScaleY = m_entityController->GetScale(m_iEntityNum)[1];
	m_pSprite->SetWidthHeight( m_fScaleX, m_fScaleY );

	if (m_entityController->HasProjectileBullet(m_iEntityNum))
	{
		m_fBulletScaleX = m_entityController->GetProjectileBullet(m_iEntityNum)->scale[0];
		m_fBulletScaleY = m_entityController->GetProjectileBullet(m_iEntityNum)->scale[1];
		for ( std::shared_ptr<ProjectileManager> pManager : m_vecProjectileManagers )
			for ( std::shared_ptr<Projectile> pProjectile : pManager->GetProjector() )
				pProjectile->GetSprite()->SetWidthHeight( m_fBulletScaleX, m_fBulletScaleY );
	}
}

void Entity::UpdateAnimation()
{
	if (m_entityController->HasAnimation(m_iEntityNum))
	{
		for (int i = 0; i < m_animation.GetAnimationTypeSize(); i++)
		{
			if (m_animation.GetAnimationTypeName(i) == m_entityController->GetAnimationType(m_iEntityNum))
			{
				m_iMaxFrameX = m_animation.GetFrameCount()[i];
				m_iCurFrameY = i;
				m_pSprite->UpdateFrameTime(m_animation.GetFrameTiming(i));
			}
		}

		m_pSprite->SetMaxFrame(m_iMaxFrameX, m_iMaxFrameY);
		if (GetType() != "Player")
		{
			m_pSprite->SetCurFrameY(m_iCurFrameY);
		}

		if (m_entityController->HasProjectileBullet(m_iEntityNum))
		{
			m_iBulletMaxFrameX = m_entityController->GetProjectileBullet(m_iEntityNum)->maxFrame[0];
			m_iBulletMaxFrameY = m_entityController->GetProjectileBullet(m_iEntityNum)->maxFrame[1];
			for (std::shared_ptr<ProjectileManager> pManager : m_vecProjectileManagers)
			{
				for (std::shared_ptr<Projectile> pProjectile : pManager->GetProjector())
					pProjectile->GetSprite()->SetMaxFrame(m_iBulletMaxFrameX, m_iBulletMaxFrameY);
			}
		}
	}
}

void Entity::UpdateRotation()
{
	m_fRotation = m_entityController->GetRotation(m_iEntityNum) * PI / 4;
	m_pTransform->SetRotation(m_fRotation);
}

void Entity::UpdateTexture()
{
	m_sTex = m_entityController->GetTexture(m_iEntityNum);
	m_pSprite->UpdateTex(m_device, m_sTex);

	if (m_entityController->HasProjectileBullet(m_iEntityNum))
	{
		m_sBulletTex = m_entityController->GetProjectileBullet(m_iEntityNum)->texture;
		for (std::shared_ptr<ProjectileManager> pManager : m_vecProjectileManagers)
		{
			for (std::shared_ptr<Projectile> pProjectile : pManager->GetProjector())
			{
				if (pProjectile->GetSprite()->HasTexture())
				{
					pProjectile->GetSprite()->UpdateTex(m_device, m_sBulletTex);
				}
				else
				{
					pProjectile->GetSprite()->Initialize(m_device, m_context, m_entityController->GetTexture(m_iEntityNum), *m_mat);
				}
			}
		}
	}
}

void Entity::UpdateMass()
{
	if (m_agent != nullptr)
	{
		m_fMass = m_entityController->GetMass(m_iEntityNum);
		m_pPhysics->SetMass(m_fMass);

		if (m_entityController->HasProjectileBullet(m_iEntityNum))
		{
			m_fBulletMass = m_entityController->GetProjectileBullet(m_iEntityNum)->mass;
			for (std::shared_ptr<ProjectileManager> pManager : m_vecProjectileManagers)
			{
				for (std::shared_ptr<Projectile> pProjectile : pManager->GetProjector())
					pProjectile->GetPhysics()->SetMass(m_fBulletMass);
			}
		}
	}
}

void Entity::UpdateSpeed()
{
	m_fSpeed = m_entityController->GetSpeed(m_iEntityNum);

	if (m_entityController->HasAI(m_iEntityNum) && m_agent != nullptr)
	{
		m_agent->SetSpeed(m_fSpeed);
	}
}

void Entity::UpdateBehaviour()
{
	if (m_agent == nullptr)
		return;

	if (!m_entityController->HasAI(m_iEntityNum))
		return;

	if (m_agent->HasBehaviourFile())
		return;

	m_sBehaviour = m_entityController->GetBehaviour(m_iEntityNum);

	m_agent->SetBehaviour(AILogic::StateType::GetType(m_sBehaviour));
}

void Entity::UpdateProjectilePattern()
{
	//TODO
	//if (m_entityType != EntityType::Projectile)
	//{
	//	m_sBulletPattern = m_entityController->GetProjectileBullet(m_iEntityNum)->projectilePattern;
	//	if (m_sBulletPattern != "None")
	//	{
	//		static bool temp = true;
	//		if (true)
	//		{
	//			temp = !temp;

	//			auto temp = m_projectileManager->GetProjector().size();
	//			for (int i = 0; i < m_projectileManager->GetProjector().size(); i++)
	//			{

	//				m_projectileManager->UpdatePattern(m_sBulletPattern);
	//				/*m_projectileManager->GetProjector()[i]->*/
	//			}
	//		}
	//	}
	//}
}

void Entity::UpdateCollider()
{
	if (m_entityController->HasCollider(m_iEntityNum))
	{
		UpdateColliderRadius();
		UpdateColliderLayer();
		UpdateColliderEnabled();
		UpdateColliderStatic();
		UpdateColliderTrigger();
		UpdateColliderMask();
		UpdateColliderShape();
	}
	else if (m_colliderCircle == nullptr || m_colliderBox == nullptr)
	{
		m_colliderCircle = std::make_shared<CircleCollider>(m_pTransform, m_pSprite, true, m_iEntityNum, GetType(), 32);
		m_colliderBox = std::make_shared<BoxCollider>(m_pTransform, m_pSprite, true, m_iEntityNum, GetType(), 32, 32);
	}
}

void Entity::UpdateColliderRadius()
{
	if (m_entityController->HasCollider(m_iEntityNum) && m_colliderCircle != nullptr)
	{
		m_fColliderRadiusX = m_entityController->GetColliderRadius(m_iEntityNum)[0];
		m_fColliderRadiusY = m_entityController->GetColliderRadius(m_iEntityNum)[1];

		if (m_entityController->GetColliderShape(m_iEntityNum) == "Circle")
		{
			m_colliderCircle->SetRadius(m_fColliderRadiusX);
			m_colliderBox->SetWidth(0);
			m_colliderBox->SetHeight(0);
		}
		else if (m_entityController->GetColliderShape(m_iEntityNum) == "Box")
		{
			m_colliderBox->SetWidth(m_fColliderRadiusX);
			m_colliderBox->SetHeight(m_fColliderRadiusY);
			m_colliderCircle->SetRadius(0);
		}
	}
	else if (m_colliderCircle == nullptr)
	{
		m_colliderCircle = std::make_shared<CircleCollider>(m_transform, m_sprite, true, m_iEntityNum, GetType(), 32);
		m_colliderBox = std::make_shared<BoxCollider>(m_transform, m_sprite, true, m_iEntityNum, GetType(), 32, 32);
	}
}

void Entity::UpdateColliderTrigger()
{
	if (m_entityController->HasCollider(m_iEntityNum) && m_colliderCircle != nullptr)
	{
		m_colliderBox->SetIsTrigger(m_entityController->GetColliderTrigger(m_iEntityNum));
		m_colliderCircle->SetIsTrigger(m_entityController->GetColliderTrigger(m_iEntityNum));
	}
}

void Entity::UpdateColliderLayer()
{
	std::string colliderLayer = m_entityController->GetColliderLayer(m_iEntityNum);

	if (!m_entityController->HasCollider(m_iEntityNum)) return;

	if (colliderLayer == "Decoration")
	{
		m_colliderCircle->SetLayer(LayerNo::Decoration);
		m_colliderBox->SetLayer(LayerNo::Decoration);
	}
	else if (colliderLayer == "Player")
	{
		m_colliderCircle->SetLayer(LayerNo::Player);
		m_colliderBox->SetLayer(LayerNo::Player);
	}
	else if (colliderLayer == "Enemy")
	{
		m_colliderCircle->SetLayer(LayerNo::Enemy);
		m_colliderBox->SetLayer(LayerNo::Enemy);
	}
	else if (colliderLayer == "PlayerProjectile")
	{
		m_colliderCircle->SetLayer(LayerNo::PlayerProjectile);
		m_colliderBox->SetLayer(LayerNo::PlayerProjectile);
	}
	else if (colliderLayer == "EnemyProjectile")
	{
		m_colliderCircle->SetLayer(LayerNo::EnemyProjectile);
		m_colliderBox->SetLayer(LayerNo::EnemyProjectile);
	}
}

void Entity::UpdateColliderMask()
{
	std::vector<bool> colliderMaskData = m_entityController->GetColliderMask(m_iEntityNum);
	LayerMask colliderMask = LayerMask(colliderMaskData[0], colliderMaskData[1], colliderMaskData[2], colliderMaskData[3], colliderMaskData[4]);
	m_colliderCircle->SetCollisionMask(colliderMask);
	m_colliderBox->SetCollisionMask(colliderMask);
}

void Entity::UpdateColliderStatic()
{
	bool isStatic = m_entityController->GetColliderStatic(m_iEntityNum);
	m_colliderCircle->SetIsStatic(isStatic);
	m_colliderBox->SetIsStatic(isStatic);
}

void Entity::UpdateColliderEnabled()
{
	bool isEnabled = m_entityController->GetColliderEnabled(m_iEntityNum);
	m_colliderCircle->SetIsEnabled(isEnabled);
	m_colliderBox->SetIsEnabled(isEnabled);
}

void Entity::UpdateColliderShape()
{
	m_sColliderShape = m_entityController->GetColliderShape(m_iEntityNum);
}

void Entity::UpdateAudio()
{
	if(m_entityController->HasAudio(m_iEntityNum))
		m_sSoundBankName = m_entityController->GetSoundBankName(m_iEntityNum);
}

void Entity::UpdateEntityNum(int num)
{
	m_iEntityNum = num;
	if (m_health)
	{
		m_health->SetEntityNum(num);
	}
}

void Entity::SetAnimation()
{
	if (m_entityController->GetAnimationFile(m_iEntityNum) != "None" && m_entityController->HasAnimation(m_iEntityNum))
	{
		m_animation.LoadEntityAnimation(m_entityController->GetAnimationFile(m_iEntityNum));
		m_iMaxFrameY = m_animation.GetFrameCount().size();
		UpdateAnimation();
	}
}
