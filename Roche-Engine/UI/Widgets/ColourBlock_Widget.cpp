#include "stdafx.h"
#include "ColourBlock_Widget.h"

ColourBlock_Widget::ColourBlock_Widget()
{
	m_pSprite = std::make_shared<Sprite>();
    m_pTransform = std::make_shared<Transform>( m_pSprite );
}

ColourBlock_Widget::~ColourBlock_Widget() { }

void ColourBlock_Widget::Initialize( ID3D11Device* device, ID3D11DeviceContext* context, ConstantBuffer<Matrices>& mat )
{
	m_pSprite->Initialize( device, context, "", mat );
}

void ColourBlock_Widget::Update( const float dt )
{
	m_pTransform->Update();
}

void ColourBlock_Widget::Draw( ID3D11Device* device, ID3D11DeviceContext* context, XMMATRIX worldOrtho )
{
	m_pSprite->UpdateTex( device, m_colour );
	m_pSprite->UpdateBuffers( context );
	m_pSprite->Draw( m_pTransform->GetWorldMatrix(), worldOrtho );
}

void ColourBlock_Widget::Resolve( Colour colour )
{
	m_colour = colour;
}