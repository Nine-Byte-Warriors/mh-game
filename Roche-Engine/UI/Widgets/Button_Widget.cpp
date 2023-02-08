#include "stdafx.h"
#include "Button_Widget.h"
#include "AudioEngine.h"

#define SOUND_BANK_NAME "UI"

Button_Widget::Button_Widget()
{
    m_pSprite = std::make_shared<Sprite>();
    m_pTransform = std::make_shared<Transform>( m_pSprite );
}

Button_Widget::~Button_Widget() {}

void Button_Widget::Initialize( ID3D11Device* device, ID3D11DeviceContext* context, ConstantBuffer<Matrices>& mat )
{
    m_vTextOffset = { 0.0f, 0.0f };
    m_pSprite->Initialize( device, context, "", mat );
}

void Button_Widget::Update( const float dt )
{
    m_pTransform->Update();
}

void Button_Widget::Draw( ID3D11Device* device, ID3D11DeviceContext* context, XMMATRIX worldOrtho, TextRenderer* textRenderer )
{
    // Button sprite
    m_pSprite->UpdateTex( device, m_buttonTexture );
    m_pSprite->UpdateBuffers( context );
    m_pSprite->Draw( m_pTransform->GetWorldMatrix(), worldOrtho );

    // Button text
    XMVECTOR textsize = textRenderer->GetSpriteFont( m_eFontSize )->MeasureString( m_sText.c_str() );
    XMFLOAT2 textPos =
    {
        m_pTransform->GetPosition().x + ( m_pSprite->GetWidth() / 2.0f ) - ( XMVectorGetX( textsize ) * textRenderer->GetScale().x ) / 2.0f,
        m_pTransform->GetPosition().y + ( m_pSprite->GetHeight() / 2.0f ) - ( XMVectorGetY( textsize ) * textRenderer->GetScale().y ) / 2.0f
    };
    textPos.x += m_vTextOffset.x;
    textPos.y += m_vTextOffset.y;
    textRenderer->RenderString( m_sText, textPos, m_vTextColor, m_eFontSize, true );
}

bool Button_Widget::Resolve( const std::string& text, XMVECTORF32 textColour, const std::vector<std::string>& textures, MouseData& mData, bool keepSelected, FontSize size, bool shouldPlayAudio )
{
    m_sText = text;
    m_eFontSize = size;
    m_vTextColor = textColour;
    m_buttonState = ButtonState::Default;

    // Button collision
    if (
        mData.Pos.x >= m_pTransform->GetPosition().x &&
        mData.Pos.x <= ( m_pTransform->GetPosition().x + m_pSprite->GetWidth() ) &&
        mData.Pos.y >= m_pTransform->GetPosition().y &&
        mData.Pos.y <= ( m_pTransform->GetPosition().y + m_pSprite->GetHeight() )
        )
    {
        if ( mData.LPress && !mData.Locked )
            m_buttonState = ButtonState::Pressed;
        else
            m_buttonState = ButtonState::Hover;
    }

    // Button state
    if ( keepSelected )
    {
        m_buttonTexture = textures[2];
        return false;
    }

    switch ( m_buttonState )
    {
    case ButtonState::Default:
        m_buttonTexture = textures[0];
        break;
    case ButtonState::Hover:
        m_buttonTexture = textures[1];
        break;
    case ButtonState::Pressed:
        m_buttonTexture = textures[2];
        //if(shouldPlayAudio)
            AudioEngine::GetInstance()->PlayAudio( SOUND_BANK_NAME, "UIButtonClick", SFX );
        m_bIsPressed = true;
        mData.Locked = true;
        return true;
    default:
        break;
    }
    m_bIsPressed = false;
    return false;
}