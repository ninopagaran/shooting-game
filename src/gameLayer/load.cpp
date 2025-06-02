#include <load.h>
#include <iostream>

LoadBullet::LoadBullet() {
	type = 0;
	damage = 0;
	load = 0;
}

LoadBullet::LoadBullet(glm::vec2 pos, int t, int l) {
	position = pos;
	type = t;
	load = l;

	//std::cout << t << std::endl;
	if (t == 0)
		damage = 0.5;
	else if (t == 1)
		damage = 0.3;
	else
		damage = 0.2;
}

glm::vec2 LoadBullet::getPos() {
	return position;
}

int LoadBullet::getType() {
	return type;
}

float LoadBullet::getDamage() {
	return damage;
}

int LoadBullet::getLoad() {
	return load;
}
bool LoadBullet::canLoadBullet() {
	if (load > 0)
	{
		load--;
		return true;
	}
	else
		return false;
}

void LoadBullet::render(gl2d::Renderer2D& renderer, gl2d::Texture texture, gl2d::TextureAtlasPadding atlasPadding, int framex) {

	float size = 150.f;
	renderer.renderRectangle({position, size, size}, texture, Colors_White, glm::vec2(0.5f, 0.5f), 0, atlasPadding.get(framex, 0));

}