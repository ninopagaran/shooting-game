#include <bullets.h>

Bullets::Bullets() {
	position = { 0, 0 };
	fireDirection = { 0, 0 };
}

Bullets::Bullets(glm::vec2 pos, glm::vec2 dir) {
	position = pos;
	fireDirection = dir;
}

glm::vec2 Bullets::getPos() {
	return position;
}

void Bullets::render(gl2d::Renderer2D& renderer, gl2d::Texture bulletsTexture, gl2d::TextureAtlasPadding bulletsAtlas) {
	
	float spaceShipAngle = atan2(-fireDirection.x, -fireDirection.y);
	spaceShipAngle = glm::degrees(spaceShipAngle);

	renderer.renderRectangle({ position - glm::vec2(25,25), 50,50 },
		bulletsTexture, Colors_White, {}, spaceShipAngle, bulletsAtlas.get(1, 1));
}

void Bullets::update(float deltaTime) {
	position += fireDirection * deltaTime * 1500.f;
}
