#pragma once
#ifndef IMAGE_WIDGET_H
#define IMAGE_WIDGET_H

#include "Transform.h"
#include "TextRenderer.h"

class Image_Widget
{
public:
    Image_Widget();
    ~Image_Widget();

    void Initialize( ID3D11Device* device, ID3D11DeviceContext* context, ConstantBuffer<Matrices>& mat );
    void Update( const float dt );
    void Draw( ID3D11Device* device, ID3D11DeviceContext* context, XMMATRIX worldOrtho, TextRenderer* textRenderer);
    void Resolve(const std::string& text, XMVECTORF32 textColour,const std::string& texture, FontSize size = FontSize::LARGE );

    inline std::shared_ptr<Sprite> GetSprite() const noexcept { return m_sprite; }
    inline std::shared_ptr<Transform> GetTransform() const noexcept { return m_transform; }

private:
	std::string m_sText;
	XMVECTORF32 m_vTextColor;
    FontSize m_eFontSize = FontSize::LARGE;

    std::string m_sTextFile;
    std::shared_ptr<Sprite> m_sprite;
    std::shared_ptr<Transform> m_transform;
};

#endif