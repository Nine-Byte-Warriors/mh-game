#include "stdafx.h"
#include "Image_Widget.h"

Image_Widget::Image_Widget()
{
	m_pSprite = std::make_shared<Sprite>();
	m_pTransform = std::make_shared<Transform>( m_pSprite );
}

Image_Widget::~Image_Widget() {}

void Image_Widget::Initialize( ID3D11Device* device, ID3D11DeviceContext* context, ConstantBuffer<Matrices>& mat )
{
	m_pSprite->Initialize( device, context, m_sTextFile, mat );
}

void Image_Widget::Update( const float dt )
{
	m_pTransform->Update();
}

void Image_Widget::Draw( ID3D11Device* device, ID3D11DeviceContext* context, XMMATRIX worldOrtho, TextRenderer* textRenderer )
{
	m_pSprite->UpdateTex( device, m_sTextFile );
	m_pSprite->UpdateBuffers( context );
	m_pSprite->Draw( m_pTransform->GetWorldMatrix(), worldOrtho );

	// image text
	XMVECTOR textsize = textRenderer->GetSpriteFont( m_eFontSize )->MeasureString( m_sText.c_str() );
	XMFLOAT2 textpos =
	{
		m_pTransform->GetPosition().x + ( m_pSprite->GetWidth() / 2.0f ) - ( XMVectorGetX( textsize ) * textRenderer->GetScale().x ) / 2.0f,
		m_pTransform->GetPosition().y + ( m_pSprite->GetHeight() / 2.0f ) - ( XMVectorGetY( textsize ) * textRenderer->GetScale().y ) / 2.0f
	};
	textRenderer->RenderString( m_sText, textpos, m_vTextColor, m_eFontSize, true );
}

void Image_Widget::Resolve( const std::string& text, XMVECTORF32 textColour, const std::string& texture, FontSize size )
{
	m_sText = text;
	m_eFontSize = size;
	m_vTextColor = textColour;

	if ( m_sTextFile != texture )
		m_sTextFile = texture;
}