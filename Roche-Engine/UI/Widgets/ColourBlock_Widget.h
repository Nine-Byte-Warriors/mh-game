#pragma once
#ifndef COLOURBLOCK_WIDGET_H
#define COLOURBLOCK_WIDGET_H

#include "Transform.h"

class ColourBlock_Widget
{
public:
    ColourBlock_Widget();
    ~ColourBlock_Widget();

    void Initialize( ID3D11Device* device, ID3D11DeviceContext* context, ConstantBuffer<Matrices>& mat );
    void Update( const float dt );
    void Draw( ID3D11Device* device, ID3D11DeviceContext* context, XMMATRIX worldOrtho );
    void Resolve( Colour colour );

    inline std::shared_ptr<Sprite> GetSprite() const noexcept { return m_pSprite; }
    inline std::shared_ptr<Transform> GetTransform() const noexcept { return m_pTransform; }

private:
    Colour m_colour;
    std::shared_ptr<Sprite> m_pSprite;
    std::shared_ptr<Transform> m_pTransform;
};

#endif