#include <animate.h>

myAnimate::myAnimate() {
    framex = 0;
    framey = 0;
    size = 200.f;
}

myAnimate::myAnimate(float s, gl2d::Texture t, gl2d::TextureAtlasPadding atlas) {
    framex = 0;
    framey = 0;
    size = s;
    texture = t;
    atlasPadding = atlas;
}


void myAnimate::render(gl2d::Renderer2D& renderer, glm::vec2 position, int x, int y) {
    framex = x;
    framey = y;

    renderer.renderRectangle(
        {position - glm::vec2(size / 2, size / 2), size, size},
        texture, Colors_White, {}, 0, atlasPadding.get(framex, framey));
}