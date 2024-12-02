#pragma once 
#include <gl2d/gl2d.h>

const float botSize = 150.f;

class Enemy
{
private:
	glm::uvec2 type = {};
	glm::vec2 position = {};
	glm::vec2 viewDirection = { 1, 0 };
	float speed = 1500.f;
	float turnSpeed = 3.f;


public:
	Enemy();
	Enemy(glm::uvec2, glm::vec2, float, float);
	void render(gl2d::Renderer2D& renderer, gl2d::Texture& sprites, gl2d::TextureAtlasPadding& atlas);
	void update(float deltaTime, glm::vec2 playerPos);
};
