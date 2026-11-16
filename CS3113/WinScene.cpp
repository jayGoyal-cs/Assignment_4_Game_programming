#include "WinScene.h"

WinScene::WinScene()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

WinScene::WinScene(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}

WinScene::~WinScene() { shutdown(); }

void WinScene::initialise()
{
    mGameState.nextSceneID = NO_SCENE;
    mGameState.playerDied  = false;
    mGameState.player      = nullptr;
    mGameState.map         = nullptr;
    mGameState.goal        = nullptr;
}

void WinScene::update(float deltaTime) {}  // enter key is handled in main.cpp

void WinScene::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    const char *title    = "You Win!";
    const char *subtitle = "Press enter to go back to the menu";

    DrawText(title, mOrigin.x - MeasureText(title, 60) / 2, mOrigin.y - 80,
             60, WHITE);
    DrawText(subtitle, mOrigin.x - MeasureText(subtitle, 30) / 2,
             mOrigin.y + 20, 30, WHITE);
}

void WinScene::shutdown() {}
