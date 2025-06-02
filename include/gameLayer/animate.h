#pragma once
#include <gl2d/gl2d.h>

class myAnimate {
public:
    int framex = 0;
    int framey = 0;
    int size = 200.f;

    int numberOfFramesx = 0;
    int numberOfFramesy = 0;

    gl2d::Texture texture;
    gl2d::TextureAtlasPadding atlasPadding;
    myAnimate();
    myAnimate(float, gl2d::Texture, gl2d::TextureAtlasPadding);
    void render(gl2d::Renderer2D&, glm::vec2,int, int);

};
