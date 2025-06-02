#pragma once
#include <gl2d/gl2d.h>

class LoadBullet
{
private:
	glm::vec2 position = {};
	int type = 0;
	float damage = 0;
	int load = 0;
public:
	LoadBullet();
	LoadBullet(glm::vec2, int, int);
	int getType();
	int getLoad();
	float getDamage();
	glm::vec2 getPos();
	bool canLoadBullet();
	void render(gl2d::Renderer2D& renderer, gl2d::Texture texture, gl2d::TextureAtlasPadding atlaspadding, int framex);

};