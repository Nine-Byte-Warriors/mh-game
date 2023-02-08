#include "stdafx.h"
#include "Projectile.h"
#include "Graphics.h"

Projectile::Projectile(float fSpeed, float fLifeTime)
{
	m_vSpawnPosition = Vector2f();
	m_vTargetPosition = Vector2f();
	m_vDirection = Vector2f();
	m_vOffSet = Vector2f();

	m_fSpeed = fSpeed;
	m_fLifeTime = 0.0f;
	m_fMaxLifeTime = fLifeTime;
	m_fDelay = 0.0f;

	m_bFixedDirection = true;
	m_fAmplitude = 0.0f;
	m_fFrequency = 0.0f;
	
	m_pSprite = std::make_shared<Sprite>();
	m_pTransform = std::make_shared<Transform>( m_pSprite );
	m_pPhysics = std::make_shared<Physics>(m_pTransform);
	m_pCollider = std::make_shared<CircleCollider>(m_pTransform, m_pSprite, true, 2, "Projectile", 32);

	m_owner = ProjectileOwner::None;
}

void Projectile::Initialize(const Graphics& gfx, ConstantBuffer<Matrices>& mat, Sprite::Type type)
{
	m_pSprite->Initialize(gfx.GetDevice(), gfx.GetContext(), type, mat);
	m_pTransform->SetPositionInit(0.0f, 0.0f);
}

void Projectile::Initialize(const Graphics& gfx, ConstantBuffer<Matrices>& mat, const std::string& sSpritePath)
{
	m_pSprite->Initialize(gfx.GetDevice(), gfx.GetContext(), sSpritePath, mat);
	m_pTransform->SetPositionInit(0.0f, 0.0f);
}

void Projectile::Initialize(const Graphics& gfx, ConstantBuffer<Matrices>& mat, const std::string& sSpritePath, Vector2f vSize)
{
	m_pSprite->Initialize(gfx.GetDevice(), gfx.GetContext(), sSpritePath, mat);
	m_pTransform->SetPositionInit(0.0f, 0.0f);

	float fWidth = vSize.x == 0.0f ? m_pSprite->GetWidth() : vSize.x;
	float fHeight = vSize.y == 0.0f ? m_pSprite->GetHeight() : vSize.y;
	m_pSprite->SetWidthHeight( fWidth, fHeight );
}

void Projectile::Update(const float dt)
{
	if (!IsAlive())
	{
		SetOwner(ProjectileOwner::None);
		return;
	}
	
	m_fDelay -= dt;
	if (m_fDelay > 0.0f)
		return;

	m_fLifeTime -= dt;

	if (m_fAmplitude == 0.0f || m_fFrequency == 0.0f)
		m_pPhysics->AddForce(m_vDirection.Multiply(m_fSpeed));
	else
		CalcDirection();

	m_pSprite->Update(dt);
	m_pPhysics->Update(dt);
	m_pTransform->Update();
}

void Projectile::Draw(ID3D11DeviceContext* context, XMMATRIX orthoMatrix)
{
	if (!IsAlive())
		return;

	if (m_fDelay > 0.0f)
		return;
	
	m_pSprite->UpdateBuffers(context);
	m_pSprite->Draw(m_pTransform->GetWorldMatrix(), orthoMatrix);
}

void Projectile::SpawnProjectile(Vector2f vSpawnPosition, Vector2f vTargetPosition, float fLifeTime)
{
	m_fLifeTime = fLifeTime <= 0.0f ? m_fMaxLifeTime : fLifeTime;
	
	m_vDirection = vSpawnPosition
		.DirectionTo(vTargetPosition)
		.Normalised();
	
	m_vAnchorPosition = vSpawnPosition;
	m_pTransform->SetPosition(m_vAnchorPosition);

	m_pPhysics->ResetForces();
}

void Projectile::SpawnProjectile(Vector2f vSpawnPosition, float fLifeTime)
{
	m_fLifeTime = fLifeTime <= 0.0f	? m_fMaxLifeTime : fLifeTime;

	m_vAnchorPosition = vSpawnPosition.Add(m_vOffSet);
	m_pTransform->SetPosition(m_vAnchorPosition);

	m_pPhysics->ResetForces();
}

void Projectile::CalcDirection()
{
	float fAngle = m_fAngle + AI_MATH_HALF_PI_F;
	float fCurrentDist = m_fAmplitude * sinf((m_fMaxLifeTime - m_fLifeTime) * m_fFrequency);
	Vector2f vWavePosition = Vector2f(
		cosf(fAngle) * fCurrentDist,
		sinf(fAngle) * fCurrentDist
	);
	m_vAnchorPosition = m_vAnchorPosition.Add(m_vDirection.Multiply(m_fSpeed));
	m_pTransform->SetPosition(vWavePosition.Add(m_vAnchorPosition));
}