#include "stdafx.h"
#include "TileMapDraw.h"
#include "Graphics.h"

TileMapDraw::TileMapDraw()
{
	m_pSprite = std::make_shared<Sprite>();
	m_pTransform = std::make_shared<Transform>( m_pSprite );
}

void TileMapDraw::Initialize( const Graphics& gfx, ConstantBuffer<Matrices>& mat, const std::string& path )
{
	m_pSprite->Initialize( gfx.GetDevice(), gfx.GetContext(), path, mat );
}

void TileMapDraw::Update(const float dt)
{
	m_pTransform->Update();
}