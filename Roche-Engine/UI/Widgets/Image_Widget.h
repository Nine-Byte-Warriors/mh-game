#pragma once
#ifndef IMAGE_WIDGET_H
#define IMAGE_WIDGET_H

#include "Widget.h"
#include "Transform.h"

class Graphics;

class Image_Widget : public Widget
{
public:
    Image_Widget();
    Image_Widget( const std::string& texture );
    ~Image_Widget();

    void Initialize( ID3D11Device* device, ID3D11DeviceContext* context, ConstantBuffer<Matrices>& mat );
    void Update( const float dt );
    void Draw( ID3D11Device* device, ID3D11DeviceContext* context, XMMATRIX worldOrtho, TextRenderer* textRenderer);
    void Resolve(const std::string& text, XMVECTORF32 textColour,const std::string& texture, FontSize size = FontSize::LARGE );

    inline std::shared_ptr<Sprite> GetSprite() const noexcept { return m_sprite; }
    inline std::shared_ptr<Transform> GetTransform() const noexcept { return m_transform; }

private:
	std::string m_sText;
    FontSize m_eFontSize;
	XMVECTORF32 m_vTextColor;

    std::string m_sTextFile;
    std::shared_ptr<Sprite> m_sprite;
    std::shared_ptr<Transform> m_transform;
};

#endif