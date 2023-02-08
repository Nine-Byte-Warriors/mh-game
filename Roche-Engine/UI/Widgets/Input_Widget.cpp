#include "stdafx.h"
#include "Input_Widget.h"

Input_Widget::Input_Widget()
{
	m_pSprite = std::make_shared<Sprite>();
	m_pTransform = std::make_shared<Transform>( m_pSprite );
}

Input_Widget::~Input_Widget() {}

void Input_Widget::Initialize( ID3D11Device* device, ID3D11DeviceContext* context, ConstantBuffer<Matrices>& mat, int index )
{
	m_iIdentifier = index;
	m_vTextOffset = { 0.0f, 0.0f };
	m_pSprite->Initialize( device, context, "", mat );
}

void Input_Widget::Update( const float dt )
{
	m_pTransform->Update();
}

void Input_Widget::Draw( ID3D11Device* device, ID3D11DeviceContext* context, XMMATRIX worldOrtho, TextRenderer* textRenderer )
{
	// Button sprite
	m_pSprite->UpdateTex( device, m_buttonTexture );
	m_pSprite->UpdateBuffers( context );
	m_pSprite->Draw( m_pTransform->GetWorldMatrix(), worldOrtho );

	XMVECTOR textsize = textRenderer->GetSpriteFont( m_eFontSize )->MeasureString( m_sCurrText.c_str() );
	XMFLOAT2 textPos =
	{
		m_pTransform->GetPosition().x + ( m_pSprite->GetWidth() / 2.0f ) - ( XMVectorGetX( textsize ) * textRenderer->GetScale().x ) / 2.0f,
		m_pTransform->GetPosition().y + ( m_pSprite->GetHeight() / 2.0f ) - ( XMVectorGetY( textsize ) * textRenderer->GetScale().y ) / 2.0f
	};
	textPos.x += m_vTextOffset.x;
	textPos.y += m_vTextOffset.y;
	textRenderer->RenderString( m_bUsePlaceholder ? m_sPlaceholder : m_sCurrText, textPos, m_textColour, m_eFontSize, true );
}

void Input_Widget::Resolve( std::string placeholder, std::string& keys, XMVECTORF32 textColour, const std::vector<std::string>& textures, MouseData& mData, int index, FontSize size )
{
	m_eFontSize = size;
	m_textColour = textColour;
	m_sPlaceholder = placeholder;
	m_buttonTexture = textures[0];

	// Button collison
	if (
		mData.Pos.x >= m_pTransform->GetPosition().x &&
		mData.Pos.x <= ( m_pTransform->GetPosition().x + m_pSprite->GetWidth() ) &&
		mData.Pos.y >= m_pTransform->GetPosition().y &&
		mData.Pos.y <= ( m_pTransform->GetPosition().y + m_pSprite->GetHeight() )
		)
	{
		m_buttonTexture = textures[1];
		if ( mData.LPress && !mData.Locked )
		{
			EventSystem::Instance()->AddEvent( EVENTID::ClearCharBuffer, this );
			mData.Locked = true;
			m_bSelected = true;
			m_sCurrText = "";
			keys.clear();
		}
	}
	else
	{
		if ( mData.LPress )
		{
			EventSystem::Instance()->AddEvent( EVENTID::ReadCharInput, (bool*)false );
			m_bSelected = false;
		}
	}

	if ( m_bSelected && m_iIdentifier == index )
	{
		m_bUsePlaceholder = false;
		EventSystem::Instance()->AddEvent( EVENTID::ReadCharInput, (bool*)true );
		m_sPlaceholder = keys;
		m_sCurrText = keys;
	}
}