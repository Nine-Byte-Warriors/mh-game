#include "stdafx.h"
#include "Level1.h"
#include <imgui/imgui.h>

void Level1::OnCreate()
{
	try
	{
		// Initialize constant buffers
		HRESULT hr = m_cbMatrices.Initialize( graphics->GetDevice(), graphics->GetContext() );
		COM_ERROR_IF_FAILED( hr, "Failed to create 'Matrices' constant buffer!" );

        hr = m_cbMatrices2D.Initialize( graphics->GetDevice(), graphics->GetContext() );
		COM_ERROR_IF_FAILED( hr, "Failed to create 'Matrices2D' constant buffer!" );

        // Initialize game objects
	    hr = m_cube.InitializeMesh( graphics->GetDevice(), graphics->GetContext() );
        COM_ERROR_IF_FAILED(hr, "Failed to create 'cube' object!");

        m_player.Initialize( graphics->GetDevice(), graphics->GetContext(), 64.0f, 64.0f, "Resources\\Textures\\Carrot 64 normal SS.png", m_cbMatrices2D );
        m_player.SetInitialPosition( graphics->GetWidth() / 2 - m_player.GetWidth() / 2, graphics->GetHeight() / 2 - m_player.GetHeight() / 2, 0 );

        XMFLOAT2 aspectRatio = { static_cast<float>( graphics->GetWidth() ), static_cast<float>( graphics->GetHeight() ) };
        m_camera2D.SetProjectionValues( aspectRatio.x, aspectRatio.y, 0.0f, 1.0f );

        // Initialize systems
        m_spriteFont = std::make_unique<SpriteFont>( graphics->GetDevice(), L"Resources\\Fonts\\open_sans_ms_16_bold.spritefont" );
        m_spriteBatch = std::make_unique<SpriteBatch>( graphics->GetContext() );
        m_postProcessing.Initialize( graphics->GetDevice() );
        m_bUseCustomPP = true;

	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
        return;
	}
}

void Level1::OnSwitch()
{
	// Update Level System
	CurrentLevel = 1;
	levelName = "Level1";
	EventSystem::Instance()->AddEvent( EVENTID::SetCurrentLevelEvent, &CurrentLevel );
	NextLevel = 2;
	EventSystem::Instance()->AddEvent( EVENTID::SetNextLevelEvent, &NextLevel );
}

void Level1::BeginFrame()
{
	// Setup pipeline state
	graphics->BeginFrame();
}

void Level1::RenderFrame()
{
    // Objects
	graphics->UpdateRenderState3D();
	m_cube.UpdateBuffers( m_cbMatrices, *m_camera );
    graphics->GetContext()->VSSetConstantBuffers( 0u, 1u, m_cbMatrices.GetAddressOf() );
    m_cube.Draw( graphics->GetContext() );

    // Sprites
	graphics->UpdateRenderState2D();
    m_player.UpdateBuffers( graphics->GetContext() );
    m_player.Draw( m_camera2D.GetWorldOrthoMatrix() );
}

void Level1::EndFrame()
{
	// Render text
    m_spriteBatch->Begin();
    static XMFLOAT2 textPosition = { graphics->GetWidth() * 0.5f, graphics->GetHeight() * 0.96f };
    std::function<XMFLOAT2( const wchar_t* )> DrawOutline = [&]( const wchar_t* text ) mutable -> XMFLOAT2
    {
        XMFLOAT2 originF = XMFLOAT2( 1.0f, 1.0f );
        XMVECTOR origin = m_spriteFont->MeasureString( text ) / 2.0f;
        XMStoreFloat2( &originF, origin );

        // Draw outline
        m_spriteFont->DrawString( m_spriteBatch.get(), text,
            XMFLOAT2( textPosition.x + 1.0f, textPosition.y + 1.0f ), Colors::Black, 0.0f, originF );
        m_spriteFont->DrawString( m_spriteBatch.get(), text,
            XMFLOAT2( textPosition.x - 1.0f, textPosition.y + 1.0f ), Colors::Black, 0.0f, originF );
        m_spriteFont->DrawString( m_spriteBatch.get(), text,
            XMFLOAT2( textPosition.x - 1.0f, textPosition.y - 1.0f ), Colors::Black, 0.0f, originF );
        m_spriteFont->DrawString( m_spriteBatch.get(), text,
            XMFLOAT2( textPosition.x + 1.0f, textPosition.y - 1.0f ), Colors::Black, 0.0f, originF );

        return originF;
    };
    const wchar_t* text = L"This is example text.";
    XMFLOAT2 originF = DrawOutline( text );
    m_spriteFont->DrawString( m_spriteBatch.get(), text, textPosition,
        Colors::Green, 0.0f, originF, XMFLOAT2( 1.0f, 1.0f ) );
    m_spriteBatch->End();

    // Render scene to texture
    graphics->BeginRTT();
    m_bUseCustomPP ?
        graphics->EndRTT() :
        m_postProcessing.Bind( graphics->GetContext(), graphics->GetRenderTarget() );

    // Render imgui windows
    m_imgui->BeginRender();
    m_imgui->SpawnInstructionWindow();
    if ( ImGui::Begin( "Post-Processing", FALSE, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove ) )
    {
        ImGui::Checkbox( "Use Custom Post-Processing?", &m_bUseCustomPP );
        m_bUseCustomPP ?
            graphics->SpawnControlWindowRTT() :
            m_postProcessing.SpawnControlWindow();
    }
    ImGui::End();
    m_cube.SpawnControlWindow();
    m_imgui->EndRender();

    // Present Frame
	graphics->EndFrame();
}

void Level1::Update( const float dt )
{
	// Update the cube transform, material etc. 
    m_cube.Update( dt );
}