#include "stdafx.h"
#include "ProjectileManager.h"

ProjectileManager::ProjectileManager(Projectile::ProjectileOwner owner)
{
	// TODO: should be passed in from Projectile JSON
	m_fLifeTime = 1.0f;
	float fSpeed = 50.0f;
	m_fDelay = 0.0f;
	m_fCounter = 0.0f;
	m_fTotalDuration = 0.0f;

	m_bWillRepeat = false;

	m_vecProjectilePool = std::vector<std::shared_ptr<Projectile>>();
	for (int i = 0; i < INITIAL_POOL_COUNT; i++)
		m_vecProjectilePool.push_back(std::make_shared<Projectile>(fSpeed));
	
	m_owner = owner;
	m_bUseTarget = false;
}

ProjectileManager::~ProjectileManager()
{
	m_vecProjectilePool.clear();
}

std::vector<std::shared_ptr<Projectile>> ProjectileManager::CreateProjectilePool(std::vector<ProjectileData::ProjectileJSON> vecProjectileJsons, float fGlobalSpeed, bool bUseGlobalSpeed)
{
	std::vector<std::shared_ptr<Projectile>> vecProjectilePool;

	for (ProjectileData::ProjectileJSON pJson : vecProjectileJsons)
	{
		std::shared_ptr<Projectile> pProjectile = std::make_shared<Projectile>(
			bUseGlobalSpeed == true
			? fGlobalSpeed
			: pJson.m_fSpeed,
			pJson.m_fLifeTime
			);
		pProjectile->SetDirection(Vector2f(pJson.m_fAngle));
		pProjectile->SetOffSet(Vector2f(pJson.m_fX, pJson.m_fY));
		pProjectile->SetWave(pJson.m_fAngle, pJson.m_fAmplitude, pJson.m_fFrequency);
		pProjectile->SetDelay(pJson.m_fDelay);
		pProjectile->SetOwner(Projectile::ProjectileOwner::None);

		vecProjectilePool.push_back(std::move(pProjectile));
	}

	return vecProjectilePool;

}

std::vector<std::shared_ptr<ProjectileManager>> ProjectileManager::GenerateManagers(std::string filepath)
{
	std::vector<std::shared_ptr<ProjectileManager>> vecManagers;
	std::vector<ProjectileData::ManagerJSON> vecManagersJson;
	JsonLoading::LoadJson(vecManagersJson, /*PATTERN_FOLDER_PATH + */ filepath);

	for (ProjectileData::ManagerJSON jMan : vecManagersJson)
	{
		std::shared_ptr <ProjectileManager> pManager = std::make_shared<ProjectileManager>();
		pManager->SetDelay(jMan.m_fDelay);
		pManager->SetProjectilePool(
			CreateProjectilePool(
				jMan.m_vecProjectiles,
				jMan.m_fGlobalSpeed,
				jMan.m_bUseGlobalSpeed
			)
		);

		if (jMan.m_bLoop)
			pManager->EnableRepeat();
		
		if (jMan.m_bUseTarget)
			pManager->EnableTargeting();

		vecManagers.push_back(std::move(pManager));
	}

	return vecManagers;
}

void ProjectileManager::Initialize(const Graphics& gfx, ConstantBuffer<Matrices>& mat)
{
	for(std::shared_ptr<Projectile>& pProjectile : m_vecProjectilePool)
		pProjectile->Initialize(gfx, mat, Sprite::Type::Projectile);
}

void ProjectileManager::InitialiseFromFile(const Graphics& gfx, ConstantBuffer<Matrices>& mat, const std::string& filename)
{
	for (std::shared_ptr<Projectile>& pProjectile : m_vecProjectilePool)
		pProjectile->Initialize(gfx, mat, filename);
}

void ProjectileManager::InitialiseFromFile(const Graphics& gfx, ConstantBuffer<Matrices>& mat, const std::string& filename, Vector2f size)
{
	for (std::shared_ptr<Projectile>& pProjectile : m_vecProjectilePool)
		pProjectile->Initialize(gfx, mat, filename, size);
}

void ProjectileManager::Update( const float dt )
{
	if (!IsFinished())
		m_fDuration -= dt;
	else
	{
		// TODO : add call to spawn projectiles when duration counter is reset.
		if (m_bWillRepeat)
		{
			m_fDuration = m_fTotalDuration;
			SpawnProjectiles(m_vSpawnPosition);
		}
		else
			m_fDuration = 0.0f;
	}

	if (m_fCounter >= 0.0f)
	{
		m_fCounter -= dt;
		return;
	}

	for (std::shared_ptr<Projectile> pProjectile : m_vecProjectilePool)
		pProjectile->Update(dt);
}

void ProjectileManager::Draw( ID3D11DeviceContext* context, XMMATRIX orthoMatrix )
{
	for (std::shared_ptr<Projectile> pProjectile : m_vecProjectilePool)
		pProjectile->Draw(context, orthoMatrix);
}

void ProjectileManager::SetProjectilePool(std::vector<std::shared_ptr<Projectile>> vecProjectilePool)
{
	m_fTotalDuration = m_fDelay;
	m_vecProjectilePool = vecProjectilePool;

	for (std::shared_ptr<Projectile> pProjectile : vecProjectilePool)
		m_fTotalDuration += pProjectile->GetDelay() + pProjectile->GetMaxLifeTime();
}

void ProjectileManager::UpdatePattern(std::string filepath)
{
	JsonLoading::LoadJson(m_vecManagers, filepath);

	for (ProjectileData::ManagerJSON pManager : m_vecManagers)
	{
		m_fDelay = pManager.m_fDelay;
		m_bUseTarget = pManager.m_bUseTarget;
		UpdateProjectilePool(pManager.m_vecProjectiles);
	}
}

void ProjectileManager::UpdateProjectilePool(std::vector<ProjectileData::ProjectileJSON> vecProjectileJsons)
{
	std::vector<std::shared_ptr<Projectile>> vecProjectilePool;

	for (ProjectileData::ProjectileJSON pJson : vecProjectileJsons)
	{
		std::shared_ptr<Projectile> pProjectile = std::make_shared<Projectile>(pJson.m_fSpeed, pJson.m_fLifeTime);
		pProjectile->SetDirection(Vector2f(pJson.m_fAngle));
		pProjectile->SetOffSet(Vector2f(pJson.m_fX, pJson.m_fY));
		pProjectile->SetWave(pJson.m_fAngle, pJson.m_fAmplitude, pJson.m_fFrequency);

		vecProjectilePool.push_back(std::move(pProjectile));
	}

	m_vecProjectilePool = vecProjectilePool;
}

void ProjectileManager::SpawnProjectile()
{
	m_fCounter = m_fDelay;
	m_fDuration = m_fTotalDuration;

	std::shared_ptr<Projectile> pProjectile = GetFreeProjectile();

	if (pProjectile != nullptr)
		pProjectile->SpawnProjectile(m_vSpawnPosition, m_vTargetPosition, m_fLifeTime);
}

void ProjectileManager::SpawnProjectile(Vector2f vSpawnPosition, float fLifeTime)
{
	m_fCounter = m_fDelay;
	m_fDuration = m_fTotalDuration;

	std::shared_ptr<Projectile> pProjectile = GetFreeProjectile();

	if(pProjectile != nullptr)
		pProjectile->SpawnProjectile(vSpawnPosition, m_vTargetPosition, m_fLifeTime);
}

void ProjectileManager::SpawnProjectiles(Vector2f vSpawnPosition)
{
	m_fCounter = m_fDelay;
	m_fDuration = m_fTotalDuration;
	m_vSpawnPosition = vSpawnPosition;

	for (std::shared_ptr<Projectile> pProjectile : m_vecProjectilePool)
	{
		pProjectile->SetOwner(m_owner);
		if(m_bUseTarget)
			pProjectile->SpawnProjectile(vSpawnPosition, m_vTargetPosition, -1.0f);
		else
			pProjectile->SpawnProjectile(vSpawnPosition, -1.0f);
	}
}

std::shared_ptr<Projectile> ProjectileManager::GetFreeProjectile()
{
	for (std::shared_ptr<Projectile> pProjectile : m_vecProjectilePool)
		if (!pProjectile->IsAlive())
			return pProjectile;

	return nullptr;
}