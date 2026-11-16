#include "LoseScene.h"

LoseScene::LoseScene()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

LoseScene::LoseScene(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}

LoseScene::~LoseScene() { shutdown(); }

void LoseScene::initialise()
{
    mGameState.nextSceneID = NO_SCENE;
    mGameState.playerDied  = false;
    mGameState.player      = nullptr;
    mGameState.map         = nullptr;
    mGameState.goal        = nullptr;
}

void LoseScene::update(float deltaTime) {}  // enter key is handled in main.cpp

void LoseScene::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    const char *title    = "You Lose";
    const char *subtitle = "Press enter to go back to the menu";

    DrawText(title, mOrigin.x - MeasureText(title, 60) / 2, mOrigin.y - 80,
             60, WHITE);
    DrawText(subtitle, mOrigin.x - MeasureText(subtitle, 30) / 2,
             mOrigin.y + 20, 30, WHITE);
}

void LoseScene::shutdown() {}
