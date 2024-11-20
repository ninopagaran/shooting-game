#define GLM_ENABLE_EXPERIMENTAL
#include "gameLayer.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include "platformInput.h"
#include "imgui.h"
#include <iostream>
#include <sstream>
#include "imfilebrowser.h"
#include <gl2d/gl2d.h>
#include <platformTools.h>

class GameData 
{
public:
	glm::vec2 player1Pos = { 400,450 };
	glm::vec2 player2Pos = { 1350,450 };
};

GameData data;

gl2d::Renderer2D renderer;

gl2d::Texture human1BodyTexture;
gl2d::Texture human2BodyTexture;

bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create();

	human1BodyTexture.loadFromFile(RESOURCES_PATH "spaceShip/ships/green.png", true); //replace this sprite if naa na;
	human2BodyTexture.loadFromFile(RESOURCES_PATH "spaceShip/ships/green.png", true); 



	
	
	return true;
}



bool gameLogic(float deltaTime)
{
#pragma region init stuff
	int w = 0; int h = 0;
	w = platform::getFrameBufferSizeX(); //window w
	h = platform::getFrameBufferSizeY(); //window h
	
	glViewport(0, 0, w, h);
	glClear(GL_COLOR_BUFFER_BIT); //clear screen

	renderer.updateWindowMetrics(w, h);
#pragma endregion

#pragma region movement on player 1

	glm::vec2 move1 = {};

	if (platform::isButtonHeld(platform::Button::W))
		move1.y = -1;
	if (platform::isButtonHeld(platform::Button::S))
		move1.y = 1;
	if (platform::isButtonHeld(platform::Button::A))
		move1.x = -1;
	if (platform::isButtonHeld(platform::Button::D))
		move1.x = 1;

	if (move1.x != 0 || move1.y != 0)
	{
		move1 = glm::normalize(move1);
		move1 *= deltaTime * 500;
		data.player1Pos += move1;
	}
#pragma endregion

#pragma region movement on player 2

	glm::vec2 move2 = {};

	if (platform::isButtonHeld(platform::Button::Up))
		move2.y = -1;
	if (platform::isButtonHeld(platform::Button::Down))
		move2.y = 1;
	if (platform::isButtonHeld(platform::Button::Left))
		move2.x = -1;
	if (platform::isButtonHeld(platform::Button::Right))
		move2.x = 1;

	if (move2.x != 0 || move2.y != 0)
	{
		move2 = glm::normalize(move2);
		move2 *= deltaTime * 500;
		data.player2Pos += move2;
	}
#pragma endregion



	renderer.renderRectangle({ data.player1Pos, 200, 200}, human1BodyTexture);
	renderer.renderRectangle({ data.player2Pos, 200, 200 }, human2BodyTexture);

	renderer.flush();


	//ImGui::ShowDemoWindow();


	return true;
#pragma endregion

}

//This function might not be be called if the program is forced closed
void closeGame()
{



}
