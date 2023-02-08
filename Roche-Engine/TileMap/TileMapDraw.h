#pragma once
#ifndef TILEMAPDRAW_H
#define TILEMAPDRAW_H

class Graphics;
#include "Transform.h"

class TileMapDraw
{
public:
	TileMapDraw();
	void Initialize(const Graphics& gfx, ConstantBuffer<Matrices>& mat, const std::string& path);
	void Update(const float dt);

	inline std::shared_ptr<Sprite> GetSprite() const noexcept { return m_pSprite; }
	inline std::shared_ptr<Transform> GetTransform() const noexcept { return m_pTransform; }
private:
	std::shared_ptr<Sprite> m_pSprite;
	std::shared_ptr<Transform> m_pTransform;
};

#endif