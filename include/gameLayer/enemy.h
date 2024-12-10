#pragma once 
#include <gl2d/gl2d.h>

const float botSize = 150.f;

class Enemy
{
private:
	glm::vec2 position = {};
	glm::vec2 viewDirection = { 1, 0 };
	float speed = 1500.f;
	float turnSpeed = 3.f;

	float life = 1.f;

	float firedTime = 1.f;
	float fireTimeReset = 0.2;
	float fireRange = 1.5;
	float bulletSpeed = 2000;
	int type;
	float damage;
public:
	Enemy();
	Enemy(int, glm::vec2, float, float, float, float);
	glm::vec2 getPos(); 
	glm::vec2 getView();
	void render(gl2d::Renderer2D& renderer, gl2d::Texture& sprites);
	bool update(float deltaTime, glm::vec2 playerPos);
	void damageLife(float);
	float getLife();
	int getType();
	float getDamage();
};
