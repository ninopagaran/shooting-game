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
#include <tiledRenderer.h>

class GameData 
{
public:
	glm::vec2 player1Pos = { 400,450 };
	glm::vec2 player1Angle = { 1, 0 };
	glm::vec2 player2Pos = { 1350,450 };
	glm::vec2 player2Angle = { -1, 0 };
};

GameData data;
TiledRenderer tiledRenderer;

gl2d::Renderer2D renderer;

gl2d::Texture human1BodyTexture;
gl2d::Texture human2BodyTexture;
gl2d::Texture backgroundTexture;

bool initGame()
{
	//initializing stuff for the renderer
	gl2d::init();
	renderer.create();

	human1BodyTexture.loadFromFile(RESOURCES_PATH "spaceShip/ships/green.png", true); //replace this sprite if naa na;
	human2BodyTexture.loadFromFile(RESOURCES_PATH "spaceShip/ships/green.png", true); 
	backgroundTexture.loadFromFile(RESOURCES_PATH "tempBackground.png", true);
	
	tiledRenderer = TiledRenderer(5000, backgroundTexture);

	
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
		move1 *= deltaTime * 2000;
		data.player1Angle += move1;
	}

	float jet1Angle = atan2(-data.player1Angle.x, -data.player1Angle.y);


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
		move2 *= deltaTime * 2000;
		data.player2Angle += move2;
	}
	
	float jet2Angle = atan2(-data.player2Angle.x, -data.player2Angle.y);

#pragma endregion

#pragma region camera follow

	glm::vec2 midpoint = {
		(data.player1Pos.x + data.player2Pos.x) / 2,
		(data.player1Pos.y + data.player2Pos.y) / 2
	};

	renderer.currentCamera.follow(midpoint, deltaTime * 450, 10, 50, w, h);

#pragma endregion 

#pragma region render background

	tiledRenderer.render(renderer);

#pragma endregion


	renderer.renderRectangle({ data.player1Pos, 100, 100 }, human1BodyTexture, Colors_White, {}, glm::degrees(jet1Angle));
	renderer.renderRectangle({ data.player2Pos, 100, 100 }, human2BodyTexture, Colors_White, {}, glm::degrees(jet2Angle));

	renderer.flush();


	//ImGui::ShowDemoWindow();


	return true;
#pragma endregion

}

//This function might not be be called if the program is forced closed
void closeGame()
{



}
