#include <button.h>

Button::Button() {
    buttonSize = 250.f;
    selected = false;
}

Button::Button(float size, gl2d::Texture texture, gl2d::TextureAtlasPadding atlas) {
    buttonSize = size;
    texture = texture;
    atlasPadding = atlas;
}

void Button::render(gl2d::Renderer2D& renderer ,glm::vec2 position, bool select) {

    selected = select;
    renderer.renderRectangle(
        {position - glm::vec2(buttonSize / 2, buttonSize / 2), buttonSize, buttonSize},
        texture, Colors_White, {}, 0, atlasPadding.get(selected ? 0 : 1, 0));

}