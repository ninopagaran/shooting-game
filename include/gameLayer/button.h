#pragma once
#include <gl2d/gl2d.h>

class Button{
public:
    float buttonSize = 250.f;
    bool selected = false;

    gl2d::Texture texture;
    gl2d::TextureAtlasPadding atlasPadding;
    Button();
    Button(float, gl2d::Texture, gl2d::TextureAtlasPadding);
    void render(gl2d::Renderer2D&, glm::vec2, bool);

};