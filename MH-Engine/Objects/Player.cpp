#include "stdafx.h"
#include "Player.h"
#include "Graphics.h"

#if _DEBUG
#include <imgui/imgui.h>
#endif

Player::Player()
{
	m_fXDirection = 0.0f;
	m_fYDirection = 0.0f;

	m_vPlayerPos = std::make_shared<Vector2f>();
	m_sprite = std::make_shared<Sprite>();
	m_transform = std::make_shared<Transform>( m_sprite );
	m_physics = std::make_shared<Physics>( m_transform );
	m_collider = std::make_shared<BoxCollider>( m_transform, 32, 32 );
	m_pProjectileManager = std::make_shared<ProjectileManager>();
	AddToEvent();
}

Player::~Player()
{
	RemoveFromEvent();
}

void Player::Initialize( const Graphics& gfx, ConstantBuffer<Matrices>& mat )
{
	m_sprite->Initialize( gfx.GetDevice(), gfx.GetContext(), Sprite::Type::Carrot, mat );
	m_pProjectileManager->Initialize( gfx, mat );
}

void Player::Update( const float dt )
{
	m_sprite->Update( dt );
	m_physics->Update( dt );
	m_transform->Update();
	m_pProjectileManager->Update( dt );

	m_vPlayerPos = std::make_shared<Vector2f>(m_transform->GetPosition());
	EventSystem::Instance()->AddEvent( EVENTID::PlayerPosition, m_vPlayerPos.get());
}

#if _DEBUG
void Player::SpawnControlWindow()
{
	if ( ImGui::Begin( "Player##Window", FALSE, ImGuiWindowFlags_AlwaysAutoResize ) )
	{
		ImGui::Text( std::string( "X: " ).append( std::to_string( m_transform->GetPosition().x ) ).c_str() );
		ImGui::SameLine();
		ImGui::Text( std::string( "Y: " ).append( std::to_string( m_transform->GetPosition().y ) ).c_str() );

		if ( ImGui::Button("FIRE!") )
			EventSystem::Instance()->AddEvent(EVENTID::PlayerFire, nullptr);
	}
	ImGui::End();
}
#endif

void Player::AddToEvent() noexcept
{
	EventSystem::Instance()->AddClient( EVENTID::PlayerUp, this );
	EventSystem::Instance()->AddClient( EVENTID::PlayerLeft, this );
	EventSystem::Instance()->AddClient( EVENTID::PlayerDown, this );
	EventSystem::Instance()->AddClient( EVENTID::PlayerRight, this );
	EventSystem::Instance()->AddClient(EVENTID::PlayerDash, this);

	EventSystem::Instance()->AddClient(EVENTID::PlayerFire, this);
}

void Player::RemoveFromEvent() noexcept
{
	EventSystem::Instance()->RemoveClient( EVENTID::PlayerUp, this );
	EventSystem::Instance()->RemoveClient( EVENTID::PlayerLeft, this );
	EventSystem::Instance()->RemoveClient( EVENTID::PlayerDown, this );
	EventSystem::Instance()->RemoveClient( EVENTID::PlayerRight, this );
}

void Player::HandleEvent( Event* event )
{
	float movementFactor = 10.0f;
	float movementDashFactor = 0.0f;
	float DashValue = 100.0f;
	switch ( event->GetEventID() )
	{
	case EVENTID::PlayerUp: 
		m_physics->AddForce( { 0.0f, -movementFactor } ); 
		m_fYDirection = -DashValue;
		break;
	case EVENTID::PlayerLeft: 
		m_physics->AddForce( { -movementFactor, 0.0f } ); 
		m_fXDirection = -DashValue;
		break;
	case EVENTID::PlayerDown: 
		m_physics->AddForce( { 0.0f, movementFactor } ); 
		m_fYDirection = DashValue;
		break;
	case EVENTID::PlayerRight: 
		m_physics->AddForce( { movementFactor, 0.0f } ); 
		m_fXDirection = DashValue;
		break;

	case EVENTID::PlayerDash: m_physics->AddForce({ movementDashFactor + m_fXDirection,movementDashFactor + m_fYDirection }); 
		m_fXDirection = 0.0f;
		m_fYDirection = 0.0f;
		break;
	default: break;
	}
}