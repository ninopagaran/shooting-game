#define GLM_ENABLE_EXPERIMENTAL
#define STBI_MALLOC(sz) malloc(sz)
#define STBI_REALLOC(p, newsz) realloc(p, newsz)
#define STBI_FREE(p) free(p)

#include <glad/glad.h>
// clang-format off
#include "gameLayer.h"
#include "imgui.h"
#include "imfilebrowser.h"
#include "platformInput.h"
#include <cmath>
#include <cstdio>
#include <iostream>

#include <gl2d/gl2d.h>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glui/glui.h>

#include <platformTools.h>
#include <raudio.h>
#include <sstream>
#include <string>
// clang-format on
#include <bullets.h>
#include <enemy.h>
#include <load.h>
#include <tiledRenderer.h>

#include <queue>
#include <vector>

#pragma region states

enum Gamestate {
  MAIN_MENU,
  IN_GAME,
  HOW_TO_PLAY,
  CREDITS,
  GAMEOVER
};

enum Level {
  EASY,
  MEDIUM,
  HARD
};

#pragma endregion

#pragma region initial states

Gamestate currentGameState = MAIN_MENU;
Level currentLevel = EASY;

#pragma endregion

class GameData {
public:
  glm::vec2 playerPos = {100, 100};
  std::vector<Bullets> bullets;
  std::vector<Enemy> enemies;
  std::vector<LoadBullet> loads;
  std::queue<LoadBullet> jetLoad;

  float health = 1.0f;
  float spawnTimeEnemy = 3;
  float shootCooldown = 0.0f;

  int currentScore = 0;
  int highScore = 0;
  int lives = 3;
  int counter = 0;
};

GameData data;

#pragma region object resources

gl2d::Renderer2D renderer;

gl2d::Texture jetBodyTexture;
gl2d::TextureAtlasPadding jetAtlas;

gl2d::Texture jetPlayerTexture;
gl2d::Texture botTexture[4];

TiledRenderer tiledRenderer[2];
gl2d::Texture backgroundTexture[2];

gl2d::Texture bulletsTexture;
gl2d::TextureAtlasPadding bulletsAtlas;

gl2d::Texture reloadBullet[3];

gl2d::Texture healthBar;
gl2d::Texture heartTex;
gl2d::Texture health;
gl2d::Texture textBar;
gl2d::Texture damageIcon[3];
gl2d::Texture loadIcon;

gl2d::Font font;

Sound shootSound;
Sound gameOverSound;

gl2d::Texture playButton;
gl2d::TextureAtlasPadding playButtonAtlas;

gl2d::Texture howButton;
gl2d::TextureAtlasPadding howButtonAtlas;

gl2d::Texture creditsButton;
gl2d::TextureAtlasPadding creditsButtonAtlas;

gl2d::Texture howToPlayTex;
gl2d::Texture creditsTex;

gl2d::Texture menuBackground;
gl2d::Texture gameoverTex;

#pragma endregion

#pragma region intersect restart

bool intersectBullet(glm::vec2 bulletPos, glm::vec2 shipPos, float shipSize) {
  return glm::distance(bulletPos, shipPos) <= shipSize;
}

void restartGame() {
  data = {};
  renderer.currentCamera.follow(data.playerPos, 550, 0, 0, renderer.windowW,
                                renderer.windowH);
}

#pragma endregion

#pragma region load resources init

bool initGame() {
  // game
  std::srand(std::time(0));
  // initializing stuff for the renderer
  gl2d::init();
  renderer.create();

  // game player sprite
  jetPlayerTexture.loadFromFile(RESOURCES_PATH "jets/Jet.png", true);
  botTexture[0].loadFromFile(RESOURCES_PATH "jets/1.png", true);
  botTexture[1].loadFromFile(RESOURCES_PATH "jets/2.png", true);
  botTexture[2].loadFromFile(RESOURCES_PATH "jets/3.png", true);
  botTexture[3].loadFromFile(RESOURCES_PATH "jets/4.png", true);

  // background
  backgroundTexture[0].loadFromFile(RESOURCES_PATH "background/sky_bg2.jpg",
                                    true);
  backgroundTexture[1].loadFromFile(RESOURCES_PATH "background/clouds_bg2.png",
                                    true);

  bulletsTexture.loadFromFileWithPixelPadding(
      RESOURCES_PATH "spaceShip/stitchedFiles/projectiles.png", 500, true);
  bulletsAtlas = gl2d::TextureAtlasPadding(3, 2, bulletsTexture.GetSize().x,
                                           bulletsTexture.GetSize().y);

  reloadBullet[0].loadFromFile(RESOURCES_PATH "bullets/reload/1.png", true);
  reloadBullet[1].loadFromFile(RESOURCES_PATH "bullets/reload/2.png", true);
  reloadBullet[2].loadFromFile(RESOURCES_PATH "bullets/reload/3.png", true);

  tiledRenderer[0] = TiledRenderer(5000, backgroundTexture[0]);
  tiledRenderer[1] = TiledRenderer(5000, backgroundTexture[1]);

  healthBar.loadFromFile(RESOURCES_PATH "healthBar.png", true);
  heartTex.loadFromFile(RESOURCES_PATH "heart1.png", true);
  health.loadFromFile(RESOURCES_PATH "health.png", true);

  textBar.loadFromFile(RESOURCES_PATH "textbar.png", true);
  damageIcon[0].loadFromFile(RESOURCES_PATH "bullets/red.png", true);
  damageIcon[1].loadFromFile(RESOURCES_PATH "bullets/blue.png", true);
  damageIcon[2].loadFromFile(RESOURCES_PATH "bullets/green.png", true);
  loadIcon.loadFromFile(RESOURCES_PATH "loadIcon.png", true);

  shootSound = LoadSound(RESOURCES_PATH "shootSfx.flac");
  SetSoundVolume(shootSound, 0.1);
  gameOverSound = LoadSound(RESOURCES_PATH "target.ogg");
  SetSoundVolume(gameOverSound, 0.3);

  font.createFromFile(RESOURCES_PATH "Minecraft.ttf");

  // menu
  playButton.loadFromFileWithPixelPadding(RESOURCES_PATH "buttons/1.png", 500,
                                          true);
  playButtonAtlas = gl2d::TextureAtlasPadding(2, 1, bulletsTexture.GetSize().x,
                                              bulletsTexture.GetSize().y);
  howButton.loadFromFileWithPixelPadding(RESOURCES_PATH "buttons/2.png", 500,
                                         true);
  howButtonAtlas = gl2d::TextureAtlasPadding(2, 1, bulletsTexture.GetSize().x,
                                             bulletsTexture.GetSize().y);
  creditsButton.loadFromFileWithPixelPadding(RESOURCES_PATH "buttons/3.png",
                                             500, true);
  creditsButtonAtlas = gl2d::TextureAtlasPadding(
      2, 1, bulletsTexture.GetSize().x, bulletsTexture.GetSize().y);

  howToPlayTex.loadFromFile(RESOURCES_PATH "howToPlay.png", true);
  creditsTex.loadFromFile(RESOURCES_PATH "credits_fn.png", true);

  menuBackground.loadFromFile(RESOURCES_PATH "ciriablast2.png", true);
  gameoverTex.loadFromFile(RESOURCES_PATH "gameoverr.png", true);
  restartGame();

  return true;
}

#pragma endregion


#pragma region spawn functions

void spawnEnemy() {

  int type;
  switch(currentLevel) {
    case EASY:
      type = 0;
      break;
    case MEDIUM:
      type = rand() % 3;
      break;
    case HARD:
      type = rand() % 4;
      break;
    default:
      type = rand() % 2;
      break;
  }

  glm::vec2 offset(2000, 0);
  offset =
      glm::vec2(glm::vec4(offset, 0, 1) *
                glm::rotate(glm::mat4(1.f), glm::radians((float)(rand() % 360)),
                            glm::vec3(0, 0, 1)));

  float speed = 800 + rand() % 1000;
  float turnSpeed = 2.2f + (rand() & 1000) / 500.f;
  float fireRange = 1.5 + (rand() % 1000) / 2000.f;
  float fireTimeReset = 0.1 + (rand() % 1000) / 500;
  glm::vec2 position = data.playerPos + offset;

  Enemy e(type, position, speed, turnSpeed, fireRange, fireTimeReset);
  data.enemies.push_back(e);
}

void spawnLoads() {

  if (data.loads.size() < 15 && data.jetLoad.size() < 20) {
    int typeBullet = rand() % 3;
    glm::vec2 offset(1500, 0);
    glm::vec2 offsetBullet = glm::vec2(
        glm::vec4(offset, 0, 1) *
        glm::rotate(glm::mat4(1.f), glm::radians((float)(rand() % 360)),
                    glm::vec3(0, 0, 1)));
    glm::vec2 posBullet = data.playerPos + offsetBullet;
    int load = 10;

    LoadBullet l(posBullet, typeBullet, load);

    data.loads.push_back(l);
  }
}

#pragma endregion

std::string level() {
  switch (currentLevel) {
    case EASY:
      return "EASY";
    case MEDIUM:
      return "MEDIUM";
    case HARD:
      return "HARD";
    default:
      return "HACKER";
  }

}

std::string strDamage(int type) {
  if (type == 0)
    return "50";
  else if (type == 1)
    return "30";
  else
    return "20";
}

const float jetSize = 180.f;

const float buttonSize = 250.f;
int presentButton = 0;

#pragma region menu functions

void menu(int w, int h) {

  for (int i = 0; i < 2; i++)
    tiledRenderer[i].render(renderer);

  renderer.renderRectangle(
      {
          glm::vec2{0, 0},
          w,
          h,
      },
      menuBackground);

  glm::vec2 playButtonPos = {985, 500};

  int maxButtons = 3;

  if (platform::isButtonReleased(platform::Button::Up)) {
    if (presentButton == 0)
      presentButton = maxButtons - 1;
    else
      presentButton -= 1;
  } else if (platform::isButtonReleased(platform::Button::Down)) {
    if (presentButton == maxButtons - 1)
      presentButton = 0;
    else
      presentButton += 1;
  }

  if (presentButton == 0) {
    renderer.renderRectangle(
        {playButtonPos - glm::vec2(buttonSize / 2, buttonSize / 2), buttonSize,
         buttonSize},
        playButton, Colors_White, {}, 0, playButtonAtlas.get(0, 0));
    if (platform::isButtonReleased(platform::Button::Enter))
      currentGameState = IN_GAME;
  } else
    renderer.renderRectangle(
        {playButtonPos - glm::vec2(buttonSize / 2, buttonSize / 2), buttonSize,
         buttonSize},
        playButton, Colors_White, {}, 0, playButtonAtlas.get(1, 0));

  if (presentButton == 1) {
    renderer.renderRectangle({playButtonPos + glm::vec2{0, 200} -
                                  glm::vec2(buttonSize / 2, buttonSize / 2),
                              buttonSize, buttonSize},
                             howButton, Colors_White, {}, 0,
                             playButtonAtlas.get(0, 0));
    if (platform::isButtonReleased(platform::Button::Enter))
      currentGameState = HOW_TO_PLAY;
  } else
    renderer.renderRectangle({playButtonPos + glm::vec2{0, 200} -
                                  glm::vec2(buttonSize / 2, buttonSize / 2),
                              buttonSize, buttonSize},
                             howButton, Colors_White, {}, 0,
                             playButtonAtlas.get(1, 0));

  if (presentButton == 2) {
    renderer.renderRectangle({playButtonPos + glm::vec2{0, 400} -
                                  glm::vec2(buttonSize / 2, buttonSize / 2),
                              buttonSize, buttonSize},
                             creditsButton, Colors_White, {}, 0,
                             playButtonAtlas.get(0, 0));
    if (platform::isButtonReleased(platform::Button::Enter))
      currentGameState = CREDITS;
  } else
    renderer.renderRectangle({playButtonPos + glm::vec2{0, 400} -
                                  glm::vec2(buttonSize / 2, buttonSize / 2),
                              buttonSize, buttonSize},
                             creditsButton, Colors_White, {}, 0,
                             playButtonAtlas.get(1, 0));
}

void howToplay(int w, int h) {
  renderer.renderRectangle(
      {
          glm::vec2{0, 0},
          w,
          h,
      },
      howToPlayTex);
  if (platform::isButtonReleased(platform::Button::Escape))
    currentGameState = MAIN_MENU;
}

void credits(int w, int h) {
  renderer.renderRectangle(
      {
          glm::vec2{0, 0},
          w,
          h,
      },
      creditsTex);
  if (platform::isButtonReleased(platform::Button::Escape))
    currentGameState = MAIN_MENU;
}

#pragma endregion

void gameover(int w, int h, int points) {

  renderer.renderRectangle(
      {
          glm::vec2{0, 0},
          w,
          h,
      },
      gameoverTex);

  glm::vec2 screenCenter(w / 2.0F, h / 2.0F);

  std::string finalScore = "FINAL SCORE: " + std::to_string(points);
  renderer.renderText(screenCenter - glm::vec2(50, -50) + glm::vec2(2.0f, 2.0f),
                      finalScore.c_str(), font, Colors_Black, 1.0F, 4.0F, 3.0F,
                      true, {});
  renderer.renderText(screenCenter - glm::vec2(50, -50), finalScore.c_str(),
                      font, glm::vec4(0.780f, 0.151f, 0.0f, 1.0f), 1.0F, 4.0F,
                      3.0F, true, {});

  std::string restartText = "Press Enter to Restart";
  renderer.renderText(
      screenCenter - glm::vec2(50, -150) + glm::vec2(2.0f, 2.0f),
      restartText.c_str(), font, Colors_Black, 1.0F, 3.0F, 2.0F, true, {});
  renderer.renderText(screenCenter - glm::vec2(50, -150), restartText.c_str(),
                      font, Colors_White, 1.0F, 3.0F, 2.0F, true, {});

  std::string menuText = "Press ESC to return to Main Menu";
  renderer.renderText(
      screenCenter - glm::vec2(50, -220) + glm::vec2(2.0f, 2.0f),
      menuText.c_str(), font, Colors_Black, 1.0F, 3.0F, 2.0F, true, {});
  renderer.renderText(screenCenter - glm::vec2(50, -220), menuText.c_str(),
                      font, Colors_White, 1.0F, 3.0F, 2.0F, true, {});

  if (platform::isButtonReleased(platform::Button::Escape))
    currentGameState = MAIN_MENU;
  else if (platform::isButtonReleased(platform::Button::Enter)) {
    restartGame();
    currentGameState = IN_GAME;
  }
}

void recoverHealth() {
    data.health += 0.5f;

    if (data.health > 1.0f && data.lives < 3) {
        data.lives++;
        data.health -= 1.0f;
    }

	if (data.lives >= 3) {
		data.lives = 3;
		data.health = std::min(data.health, 1.0f);
	}
}


void gameplay(float deltaTime, int w, int h) {

#pragma region level states

    static bool scoreReset = false;

  if (data.currentScore < 10 || data.lives == 1)
  {
    currentLevel = EASY;
	if (data.lives == 1 && !scoreReset) {
		if (data.currentScore > data.highScore) {
			data.highScore = data.currentScore;
		}
		data.currentScore = 0;
        scoreReset = true;
	}
  }
  else if (data.currentScore >= 10 && data.currentScore < 20)
  {
    currentLevel = MEDIUM;
  }
  else
  {
    currentLevel = HARD;
  }

  if (data.lives > 1) {
      scoreReset = false;
  }

#pragma endregion


#pragma region movement on player

  glm::vec2 move = {};

  if (platform::isButtonHeld(platform::Button::W) ||
      platform::isButtonHeld(platform::Button::Up)) {
    move.y = -1;
  }
  if (platform::isButtonHeld(platform::Button::S) ||
      platform::isButtonHeld(platform::Button::Down)) {
    move.y = 1;
  }
  if (platform::isButtonHeld(platform::Button::A) ||
      platform::isButtonHeld(platform::Button::Left)) {
    move.x = -1;
  }
  if (platform::isButtonHeld(platform::Button::D) ||
      platform::isButtonHeld(platform::Button::Right)) {
    move.x = 1;
  }

  if (move.x != 0 || move.y != 0) {
    move = glm::normalize(move);
    move *= deltaTime * 1500; // 200 pixels per seccond
    data.playerPos += move;
  }

#pragma endregion

#pragma region camera follow

  renderer.currentCamera.follow(data.playerPos, deltaTime * 550, 1, 150, w, h);

#pragma endregion

#pragma region render background

  for (int i = 0; i < 2; i++)
    tiledRenderer[i].render(renderer);

#pragma endregion

#pragma region mouse pos

  glm::vec2 mousePos = platform::getRelMousePosition();
  glm::vec2 screenCenter(w / 2.f, h / 2.f);

  glm::vec2 mouseDirection = mousePos - screenCenter;

  if (glm::length(mouseDirection) == 0.f) {
    mouseDirection = {1, 0};
  } else {
    mouseDirection = normalize(mouseDirection);
  }

  float jetAngle = atan2(mouseDirection.y, -mouseDirection.x);

#pragma endregion

#pragma region render loads bullets

  spawnLoads();

  for (int i = 0; i < data.loads.size(); i++) {

    if (glm::distance(data.playerPos, data.loads[i].getPos()) > 4000.f) {
      data.loads.erase(data.loads.begin() + i);
      i--;
      continue;
    }
    int type = data.loads[i].getType();

    renderer.renderRectangle({data.loads[i].getPos(), 100.f, 100.f},
                             reloadBullet[type], Colors_White, {}, {});
  }

  for (int i = 0; i < data.loads.size(); i++) {

    if (intersectBullet(data.loads[i].getPos(), data.playerPos, jetSize)) {
      LoadBullet l(data.loads[i].getPos(), data.loads[i].getType(),
                   data.loads[i].getLoad());

      data.jetLoad.push(l);
      data.loads.erase(data.loads.begin() + i);
      i--;
      continue;
    }
  }

#pragma endregion

#pragma region handle bullets

  data.shootCooldown -= deltaTime;

  if (platform::isLMousePressed() &&
      !(data.jetLoad.empty())) {
    if (data.shootCooldown <= 0.0f) {
      if (data.jetLoad.front().canLoadBullet()) {
        Bullets b(data.playerPos, mouseDirection, false,
                  data.jetLoad.front().getDamage(),
                  data.jetLoad.front().getType());
        data.bullets.push_back(b);
        PlaySound(shootSound);

        data.shootCooldown = 0.3f;
      } else
        data.jetLoad.pop();
    }
  }

  for (int i = 0; i < data.bullets.size(); i++) {

    if (glm::distance(data.bullets[i].getPos(), data.playerPos) > 5'000) {
      data.bullets.erase(data.bullets.begin() + i);
      i--;
      continue;
    }

    if (!data.bullets[i].isEnemy) {
      bool breakBothLoops = false;
      for (int e = 0; e < data.enemies.size(); e++) {

        if (intersectBullet(data.bullets[i].getPos(), data.enemies[e].getPos(),
                            jetSize)) {
          data.enemies[e].damageLife(data.bullets[i].getDamage());

          if (data.enemies[e].getLife() <= 0) {
            // kill enemy
            data.currentScore += 1;
			data.counter++;
			if (data.counter >= 10) {
				data.counter = 0;
				recoverHealth(); 
			}
            std::cout << "Current Score: " << data.currentScore << std::endl;
            data.enemies.erase(data.enemies.begin() + e);
          }
          // std::cout << data.bullets[i].getDamage() << std::endl;
          data.bullets.erase(data.bullets.begin() + i);
          i--;
          breakBothLoops = true;
          continue;
        }
      }

      if (breakBothLoops) {
        continue;
      }
    } else {
      if (intersectBullet(data.bullets[i].getPos(), data.playerPos, jetSize)) {
        data.health -= data.bullets[i].getDamage();

        data.bullets.erase(data.bullets.begin() + i);
        i--;
        continue;
      }
    }

    data.bullets[i].update(deltaTime);
  }

  if (data.health <= 0) {
    data.lives--;
    if (data.lives == 0) {
        // kill player
        PlaySound(gameOverSound);
        currentGameState = GAMEOVER;
    }
    else {
		data.health = 1.0f;
    }

  }

#pragma endregion

#pragma region render bullets
  for (auto &b : data.bullets) {
    b.render(renderer, bulletsTexture, bulletsAtlas);
  }

#pragma endregion

#pragma region handle bullets enemies

  if (data.enemies.size() < 10) {
    data.spawnTimeEnemy -= deltaTime;

    if (data.spawnTimeEnemy < 0) {
      data.spawnTimeEnemy = rand() % 6 + 1;

      spawnEnemy();
      if (rand() % 3 == 0) {
        spawnEnemy();
        spawnEnemy();
      }
    }
  }

  for (int i = 0; i < data.enemies.size(); i++) {
    if (glm::distance(data.playerPos, data.enemies[i].getPos()) > 4000.f) {
      // dispawn enemy
      data.enemies.erase(data.enemies.begin() + i);
      i--;
      continue;
    }

    if (data.enemies[i].update(deltaTime, data.playerPos)) {
      Bullets b(data.enemies[i].getPos(), data.enemies[i].getView(), true,
                data.enemies[i].getDamage(), 1);
      // todo speed
      data.bullets.push_back(b);
      if (!IsSoundPlaying(shootSound))
        PlaySound(shootSound);
    }
  }

#pragma endregion

#pragma region render enemies

  for (auto &e : data.enemies) {
    int type = e.getType();
    if (!(type >= 0 && type <= 3))
      type = 0;
    e.render(renderer, botTexture[type]);
  }

#pragma endregion

#pragma region render jet

  renderer.renderRectangle(
      {data.playerPos - glm::vec2(jetSize / 2, jetSize / 2), jetSize, jetSize},
      jetPlayerTexture, Colors_White, {}, glm::degrees(jetAngle) + 90.f);

#pragma endregion

  std::string d;
  if (!(data.jetLoad.empty()))
    d = strDamage(data.jetLoad.front().getType());
  else
    d = "0";

  std::string remLoad = std::to_string(data.jetLoad.size());
  std::string currentLevel = level();
  std::string currentPoints = std::to_string(data.currentScore);
  std::string highScoreStr = std::to_string(data.highScore);

  renderer.pushCamera();
  {

    glui::Frame f({0, 0, w, h});

    glui::Box healthBox = glui::Box()
                              .xLeftPerc(0.65)
                              .yTopPerc(0.04)
                              .xDimensionPercentage(0.3)
                              .yAspectRatio(1.f / 8.f);

    renderer.renderRectangle(healthBox, healthBar);
    glm::vec4 newRect = healthBox();
    newRect.z *= data.health;


    const float heartSize = 60.0f;
    const float heartSpacing = 70.0f;
    const float yOffset = 10.0f; 


    glm::vec2 heartBasePos = {newRect.x, newRect.y + newRect.w + yOffset};

    for (int i = 0; i < data.lives; i++) {
        renderer.renderRectangle(
            {heartBasePos + glm::vec2(i * heartSpacing, 0), heartSize, heartSize},
            heartTex, Colors_White
        );
    }

	const float belowHeartsY = heartBasePos.y + heartSize + 15.0f;

    glm::vec4 textCoords = {0, 1, 1, 0};
    textCoords.z *= data.health;

    renderer.renderRectangle(newRect, health, Colors_White, {}, {}, textCoords);

    const char *points = currentPoints.c_str();
    const char *hScore = highScoreStr.c_str();
    const char *myLevel = currentLevel.c_str();
    const char *load = remLoad.c_str();
    const char *damage = d.c_str();

    glui::Box levelBox = glui::Box()
                             .xLeftPerc(0.03)
                             .yTopPerc(0.01)
                             .xDimensionPercentage(0.15)
                             .yAspectRatio(1.f / 1.8f);

    renderer.renderRectangle(levelBox, textBar);

    renderer.renderText(glm::vec2{200, 79}, "Level", font, Colors_White, (0.5F),
                        (4.0F), (3.0F), true, {(0, 0), (0, 0), (0, 0), 0});
    renderer.renderText(glm::vec2{460, 77}, myLevel, font, Colors_White, (0.6F),
                        (4.0F), (3.0F), true);

    glui::Box highScoreBox = glui::Box()
                            .xLeftPerc(0.35)
                            .yTopPerc(0.01)
                            .xDimensionPercentage(0.15)
                            .yAspectRatio(1.f / 1.8f);

    renderer.renderRectangle(highScoreBox, textBar);
    renderer.renderText(glm::vec2{ 820, 79 }, "High Score", font, Colors_White, (0.5F),
        (4.0F), (3.0F), true, { (0, 1), (0, 1), (0, 1), 0 });
    renderer.renderText(glm::vec2{ 1045, 74 }, hScore, font, Colors_White, (0.6F),
        (4.0F), (3.0F), true);

    glui::Box scoreBox = glui::Box()
                             .xLeftPerc(0.35)
                             .yTopPerc(0.08)
                             .xDimensionPercentage(0.15)
                             .yAspectRatio(1.f / 1.8f);

    renderer.renderRectangle(scoreBox, textBar);
    renderer.renderText(glm::vec2{820, 149}, "Score", font, Colors_White, (0.5F),
                        (4.0F), (3.0F), true, {(0, 1), (0, 1), (0, 1), 0});
    renderer.renderText(glm::vec2{1045, 144}, points, font, Colors_White, (0.6F),
                        (4.0F), (3.0F), true);

    glui::Box damageBox = glui::Box()
                              .xLeftPerc(0.75)
                              .yTopPerc(0.17)
                              .xDimensionPercentage(0.03)
                              .yAspectRatio(0.7);

    if (d == "20")
      renderer.renderRectangle(damageBox, damageIcon[2]);
    else if (d == "30")
      renderer.renderRectangle(damageBox, damageIcon[1]);
    else
      renderer.renderRectangle(damageBox, damageIcon[0]);

    renderer.renderText(glm::vec2{1700, belowHeartsY + 10}, "Damage: ", font, Colors_White,
                        (0.5F), (4.0F), (3.0F), true);
    renderer.renderText(glm::vec2{1790, belowHeartsY + 4}, damage, font, Colors_White,
                        (0.5F), (4.0F), (3.0F), true);

    glui::Box loadBox = glui::Box()
                            .xLeftPerc(0.78)
                            .yTopPerc(0.12)
                            .xDimensionPercentage(0.023)
                            .yAspectRatio(1);
    renderer.renderRectangle(loadBox, loadIcon);
    renderer.renderText(glm::vec2{1725, heartBasePos.y + 5}, "Load: ", font, Colors_White,
                        (0.5F), (4.0F), (3.0F), true);
    renderer.renderText(glm::vec2{1790, heartBasePos.y + 5}, load, font, Colors_White, (0.5F),
                        (4.0F), (3.0F), true);
  }
  renderer.popCamera();

  if (platform::isButtonReleased(platform::Button::Escape))
    currentGameState = MAIN_MENU;
}

bool gameLogic(float deltaTime) {
#pragma region init stuff
  int w = 0;
  int h = 0;
  w = platform::getFrameBufferSizeX(); // window w
  h = platform::getFrameBufferSizeY(); // window h

  glViewport(0, 0, w, h);
  glClear(GL_COLOR_BUFFER_BIT); // clear screen

  renderer.updateWindowMetrics(w, h);
#pragma endregion

  switch(currentGameState) {
    case MAIN_MENU:
      renderer.pushCamera();
      menu(w, h);
      restartGame();
      renderer.popCamera();
      break;
    case IN_GAME:
      gameplay(deltaTime, w, h);
      break;
    case HOW_TO_PLAY:
      renderer.pushCamera();
      howToplay(w, h);
      renderer.popCamera();
      break;
    case CREDITS:
      renderer.pushCamera();
      credits(w, h);
      renderer.popCamera();
      break;
    case GAMEOVER:
      renderer.pushCamera();
      gameover(w, h, data.highScore);
      renderer.popCamera();
      break;
    default:
      std::cerr << "Unknown game state!" << std::endl;
      return false;
  }
  renderer.flush();
  // ImGui::ShowDemoWindow();

  /*ImGui::Begin("debug");
  ImGui::Text("Bullets 1 count: %d", (int)data.bullets.size());
  ImGui::Text("Enemies count: %d", (int)data.enemies.size());
  ImGui::Text("Points : %d", (int)data.points);

  if (ImGui::Button("Spawn enemy"))
  {
          spawnEnemy();
  }

  if (ImGui::Button("Reset game"))
  {
          restartGame();
  }

  ImGui::SliderFloat("Player Health", &data.health, 0, 1);
  ImGui::End();*/

  return true;
}

// This function might not be be called if the program is forced closed
void closeGame() {}
