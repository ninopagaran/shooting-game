#pragma once 
#include <gl2d/gl2d.h>

class TiledRenderer
{
private:
	float backgroundSize;
	gl2d::Texture texture;
public:
	TiledRenderer();
	TiledRenderer(float,gl2d::Texture);
	void render(gl2d::Renderer2D& renderer);
};