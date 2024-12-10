#include <bullets.h>

Bullets::Bullets() {
	position = { 0, 0 };
	fireDirection = { 0, 0 };
}

Bullets::Bullets(glm::vec2 pos, glm::vec2 dir, bool enemy, float d) {
	position = pos;
	fireDirection = dir;
	isEnemy = enemy;
	speed = 2500;	
	damage = d;
}

glm::vec2 Bullets::getPos() {
	return position;
}

void Bullets::render(gl2d::Renderer2D& renderer, gl2d::Texture bulletsTexture, gl2d::TextureAtlasPadding bulletsAtlas) {
	
	float jetAngle = atan2(fireDirection.y, -fireDirection.x);
	jetAngle = glm::degrees(jetAngle) + 90.f;

	glm::vec4 textureCoords = bulletsAtlas.get(1, 1);

	if (isEnemy)
	{
		textureCoords = bulletsAtlas.get(0, 0);
	}
	
	for (int i = 0; i < 5; i++)
	{
		glm::vec4 color(1 * (i + 4) / 5.f, 1 * (i + 4) / 5.f, 1 * (i + 4) / 5.f, (i + 1) / 5.f);

		renderer.renderRectangle({ position - glm::vec2(25,25) + (float)i * 25.f * fireDirection, 50,50 },
			bulletsTexture, color, {}, jetAngle, textureCoords);
	}

}

void Bullets::update(float deltaTime) {
	position += fireDirection * deltaTime * speed;
}

float Bullets::getDamage() {
	return damage;
}
