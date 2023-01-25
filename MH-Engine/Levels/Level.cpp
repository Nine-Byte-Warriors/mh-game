#include "stdafx.h"
#include "Level.h"
#include "ProjectileEditor.h"

#if _DEBUG
#include <imgui/imgui.h>
#endif

void Level::OnCreate()
{
	try
	{
		// Initialize constant buffers
		HRESULT hr = m_cbMatrices.Initialize( m_gfx->GetDevice(), m_gfx->GetContext() );
		COM_ERROR_IF_FAILED( hr, "Failed to create 'Matrices' constant buffer!" );

        // Initialize 2d camera
        XMFLOAT2 aspectRatio = { static_cast<float>( m_gfx->GetWidth() ), static_cast<float>( m_gfx->GetHeight() ) };
        m_camera.SetProjectionValues( aspectRatio.x, aspectRatio.y, 0.0f, 1.0f );

        // Initialize systems
        m_textRenderer.Initialize( "beth_ellen_ms_16_bold.spritefont", m_gfx->GetDevice(), m_gfx->GetContext() );

		// Initialise Projectile Editor
        m_projectileEditor = ProjectileEditor::CreateEditor();
		m_projectileEditor->Initialise(*m_gfx, m_cbMatrices);
	}
	catch ( COMException& exception )
	{
		ErrorLogger::Log( exception );
        return;
	}
}

void Level::CreateEntity()
{
    m_entity.clear();
    m_iEntityAmount = m_entityController.GetSize();
    for (int i = 0; i < m_iEntityAmount; i++)
    {
        Entity *entityPop = new Entity(m_entityController, i);
        m_entity.push_back(*entityPop);
        m_entity[i].Initialize(*m_gfx, m_cbMatrices);
        delete entityPop;
    }

    m_collisionHandler.RemoveAllColliders();
    for (int i = 0; i < m_iEntityAmount; i++)
        m_collisionHandler.AddCollider(m_entity[i].GetCollider());
}

void Level::CreateUI()
{
    // Update user interface
    EventSystem::Instance()->AddEvent( EVENTID::ShowCursorEvent );
    m_ui->RemoveAllUI();
    for ( unsigned int i = 0; i < m_uiEditor.GetScreens().size(); i++ )
	    m_ui->AddUI( m_uiEditor.GetScreens()[i], m_uiEditor.GetScreenData()[i].name );
	m_ui->Initialize( *m_gfx, &m_cbMatrices, m_uiEditor.GetWidgets() );
    m_ui->HideAllUI();

#if !_DEBUG
    m_ui->ShowUI( "Credits" );
#endif
}

void Level::CreateTileMap()
{
    if ( m_bFirstLoad )
    {
        m_iTileMapRows = (m_gfx->GetHeight() / m_iTileSize) + 1;
        m_iTileMapColumns = m_gfx->GetWidth() / m_iTileSize;
        m_tileMapLoader.Initialize(m_iTileMapRows, m_iTileMapColumns);
    }

#ifdef _DEBUG
    if ( m_bFirstLoad )
        m_tileMapEditor.Initialize(m_iTileMapRows, m_iTileMapColumns);
    m_tileMapLoader.SetLevel(m_tileMapEditor.GetLevel(TileMapLayer::Background), m_tileMapEditor.GetLevel(TileMapLayer::Foreground));
#else
    m_tileMapLoader->LoadLevel("Resources\\TileMaps\\blue.json", "Resources\\TileMaps\\blue.json");
#endif

    CreateTileMapDraw(m_tileMapDrawBackground);
    CreateTileMapDraw(m_tileMapDrawForeground);
}

void Level::CreateTileMapDraw(std::vector<TileMapDraw>& tileMapDraw)
{
    int colPositionTotalTileLength = 0;
    int rowPositionTotalTileLength = 0;
    const int gapBetweenTiles = 0;

    for (int i = 0; i < m_iTileMapRows * m_iTileMapColumns; i++)
    {
        TileMapDraw tileMapDrawPop;
        tileMapDraw.push_back(tileMapDrawPop);
        tileMapDraw[i].Initialize(*m_gfx, m_cbMatrices, "NONE");

        if (i != 0)
        {
            colPositionTotalTileLength += m_iTileSize + gapBetweenTiles;
        }
        bool endOfRow = i % m_iTileMapColumns == 0 && i != 0;
        if (endOfRow)
        {
            rowPositionTotalTileLength += m_iTileSize + gapBetweenTiles;
            colPositionTotalTileLength = 0;
        }

        float positionWidth = colPositionTotalTileLength;
        float positionHeight = rowPositionTotalTileLength;

        tileMapDraw[i].GetTransform()->SetPositionInit(positionWidth, positionHeight);
        tileMapDraw[i].GetTransform()->SetScaleInit(m_iTileSize, m_iTileSize);
    }
}

void Level::OnSwitch()
{
	// Update level system
	EventSystem::Instance()->AddEvent( EVENTID::SetCurrentLevelEvent, &m_iCurrentLevel );
	EventSystem::Instance()->AddEvent( EVENTID::SetNextLevelEvent, &m_iNextLevel );

    CreateEntity();
    CreateUI();
}

void Level::BeginFrame()
{
	// Setup pipeline state
	m_gfx->BeginFrame();
	m_gfx->UpdateRenderState();
}

void Level::RenderFrame()
{
	auto gfxContext = m_gfx->GetContext();
	auto camMatrix = m_camera.GetWorldOrthoMatrix();

    // Sprites
    RenderFrameTileMap(m_tileMapDrawBackground);
    RenderFrameTileMap(m_tileMapDrawForeground);

    m_projectileEditor->Draw(gfxContext, camMatrix);

    RenderFrameEntity();
}

void Level::RenderFrameEntity()
{
    for (int i = 0; i < m_iEntityAmount; i++)
    {
        m_entity[i].GetSprite()->UpdateBuffers(m_gfx->GetContext());
        m_entity[i].GetSprite()->Draw(m_entity[i].GetTransform()->GetWorldMatrix(), m_camera.GetWorldOrthoMatrix());

        if (m_entity[i].GetEntityType() == EntityType::Player)
        {
            m_entity[i].GetProjectileManager()->Draw(m_gfx->GetContext(), m_camera.GetWorldOrthoMatrix());
        }
    }
}

void Level::RenderFrameTileMap(std::vector<TileMapDraw>& tileMapDraw)
{
    for ( unsigned i = 0; i < m_iTileMapRows * m_iTileMapColumns; i++ )
    {
        tileMapDraw[i].GetSprite()->UpdateBuffers(m_gfx->GetContext());
        tileMapDraw[i].GetSprite()->Draw(tileMapDraw[i].GetTransform()->GetWorldMatrix(), m_camera.GetWorldOrthoMatrix());
    }
}

void Level::EndFrame_Start()
{
    // Render ui
    m_ui->Draw(
        m_gfx->GetShaderVtx(), m_gfx->GetShaderPix(),
        m_camera.GetWorldOrthoMatrix(), &m_textRenderer
    );

    // Render scene to texture
    m_gfx->RenderSceneToTexture();

#if _DEBUG
    // Render imgui windows
    m_imgui->BeginRender();
    ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0.0f, 0.0f ) );
    if ( ImGui::Begin( "Scene Window", FALSE ) )
    {
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 vMax = ImGui::GetWindowContentRegionMax();

        // Update imgui mouse position for scene render window
        Vector2f* mousePos = new Vector2f( ImGui::GetCursorScreenPos().x, ImGui::GetCursorScreenPos().y );
        EventSystem::Instance()->AddEvent( EVENTID::ImGuiMousePosition, mousePos );

        vMax.x += ImGui::GetWindowPos().x;
        vMax.y += ImGui::GetWindowPos().y;

        ImGui::GetWindowDrawList()->AddImage(
            (void*)m_gfx->GetRenderTargetPP()->GetShaderResourceView(),
            pos, ImVec2( vMax.x, vMax.y )
        );
    }
    ImGui::End();
    ImGui::PopStyleVar();

    m_imgui->SpawnInstructionWindow();
    m_gfx->SpawnControlWindow();

    Vector2f GOpos = m_enemy.GetTransform()->GetPosition();
    Vector2f Tpos = m_enemy.GetAI()->GetTargetPosition();
    m_enemy.GetAI()->SpawnControlWindow(GOpos, Tpos);

    m_uiEditor.SpawnControlWindow( *m_gfx );
    m_projectileEditor->SpawnEditorWindow(*m_gfx, m_cbMatrices);
    m_entityEditor.SpawnControlWindow(m_gfx->GetWidth(), m_gfx->GetHeight());
    m_tileMapEditor.SpawnControlWindow();
    m_audioEditor.SpawnControlWindow();
    m_player.SpawnControlWindow();
#endif
}

void Level::EndFrame_End()
{
#if _DEBUG
    m_imgui->EndRender();
#endif

    // Present Frame
	m_gfx->EndFrame();
}

void Level::Update( const float dt )
{
#if _DEBUG
    m_uiEditor.Update( dt );
    if ( m_uiEditor.ShouldShowAll() || m_bFirstLoad )
    {
        m_ui->ShowAllUI();
    }
    else if ( m_uiEditor.GetCurrentScreenIndex() > -1 )
    {
        m_ui->HideAllUI();
        std::string name = m_uiEditor.GetCurrentScreenName();
        m_ui->ShowUI( m_uiEditor.GetCurrentScreenName() );
    }
    else
    {
        m_ui->HideAllUI();
    }
#endif
    UpdateTileMap( dt, m_tileMapDrawBackground, TileMapLayer::Background);
    UpdateTileMap( dt, m_tileMapDrawForeground, TileMapLayer::Foreground);
    m_bFirstLoad = false;

    UpdateEntity( dt );

    m_ui->Update( dt, m_uiEditor.GetWidgets() );

	m_projectileEditor->Update( dt );
    m_collisionHandler.Update();
}

void Level::UpdateEntity(const float dt)
{
#if _DEBUG
    UpdateEntityFromEditor(dt);
#endif

    for (int i = 0; i < m_iEntityAmount; i++)
    {
        m_entity[i].Update(dt);
    }
}

void Level::UpdateEntityFromEditor(const float dt)
{
    m_entityController.SetEntityData(m_entityEditor.GetEntityData());

    if (m_iEntityAmount < m_entityController.GetSize())
    {
        for (int i = m_iEntityAmount; i < m_entityController.GetSize(); i++)
        {
            Entity* entityPop = new Entity(m_entityController, i);
            m_entity.push_back(*entityPop);
            m_entity[i].Initialize(*m_gfx, m_cbMatrices);
            delete entityPop;

            m_entity[i].GetSprite()->UpdateBuffers(m_gfx->GetContext());
            m_entity[i].GetSprite()->Draw(m_entity[i].GetTransform()->GetWorldMatrix(), m_camera.GetWorldOrthoMatrix());
            m_entity[i].GetProjectileManager()->Draw(m_gfx->GetContext(), m_camera.GetWorldOrthoMatrix());
        }
        m_iEntityAmount = m_entityEditor.GetEntityData().size();
    }

    for (int i = 0; i < m_iEntityAmount; i++)
    {
        m_entity[i].UpdateFromEntityData(dt, m_entityEditor.IsPositionLocked());
    }
}

void Level::UpdateTileMap(const float dt, std::vector<TileMapDraw>& tileMapDraw, TileMapLayer tileMapLayer)
{
    const int numberOfTileMapLayers = 2;
    if ( m_bFirstLoad )
        m_iFirstTimeTileMapDrawBothLayers = numberOfTileMapLayers;
#if _DEBUG
    if ( m_bFirstLoad )
        m_iUpdateBothTileMapLayers = numberOfTileMapLayers;

    if (m_tileMapEditor.UpdateDrawOnceAvalible() || m_iFirstTimeTileMapDrawBothLayers > 0 || m_tileMapEditor.UpdateDrawContinuousAvalible())
#else
    if (m_iFirstTimeTileMapDrawBothLayers > 0)
#endif
    {
#if _DEBUG
        if (tileMapLayer == m_tileMapEditor.GetTileMapLayer() || m_tileMapEditor.GetTileMapLayer() == TileMapLayer::Both)
#endif
        {
            for (int i = 0; i < m_iTileMapRows * m_iTileMapColumns; i++)
            {
                tileMapDraw[i].Update(dt);

                std::string texture = "Resources\\Textures\\Tiles\\";
                texture += m_tileMapLoader.GetTileTypeName(i, tileMapLayer);
                texture += ".png";

                tileMapDraw[i].GetSprite()->UpdateTex(m_gfx->GetDevice(), texture);
            }
        }
#if _DEBUG
        else
        {
            for (int i = 0; i < m_iTileMapRows * m_iTileMapColumns; i++)
            {
                tileMapDraw[i].Update(dt);

                std::string texture = "Resources\\Textures\\Tiles\\transparent.png";

                tileMapDraw[i].GetSprite()->UpdateTex(m_gfx->GetDevice(), texture);
            }
        }
        m_iUpdateBothTileMapLayers--;
        if (m_iUpdateBothTileMapLayers == 0)
        {
            m_tileMapEditor.UpdateDrawOnceDone();
            m_tileMapEditor.UpdateMapDrawn();
            m_iUpdateBothTileMapLayers = numberOfTileMapLayers;
        }
#endif
        m_iFirstTimeTileMapDrawBothLayers--;
    }
}