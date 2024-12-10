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