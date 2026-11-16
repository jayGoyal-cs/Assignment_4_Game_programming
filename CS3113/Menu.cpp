/**
* Author: Jay Goyal
* Assignment: Rise of the AI
* Date due: 11/16/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "Menu.h"

Menu::Menu()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

Menu::Menu(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}

Menu::~Menu() { shutdown(); }

void Menu::initialise()
{
    mGameState.nextSceneID = NO_SCENE;
    mGameState.playerDied  = false;
    mGameState.player      = nullptr;
    mGameState.map         = nullptr;
    mGameState.goal        = nullptr;
}

void Menu::update(float deltaTime) {}  // enter key is handled in main.cpp

void Menu::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    const char *lines[] = {
        "Press 1, 2 or 3 to snap between the three worlds.",
        "You keep your position and velocity when you teleport",
        "swap between the 3 levels to beat the game.",
        "so you have to snap between all 3 worlds to reach the end",
        "and at the end there is a boss that will hunt you."
    };

    const char *title  = "Swap Frog";
    const char *prompt = "Press enter to start";

    DrawText(title, mOrigin.x - MeasureText(title, 60) / 2, 80, 60, WHITE);

    for (int i = 0; i < 5; i++)
        DrawText(lines[i], mOrigin.x - MeasureText(lines[i], 24) / 2,
                 180 + i * 36, 24, LIGHTGRAY);

    DrawText(prompt, mOrigin.x - MeasureText(prompt, 36) / 2, 470, 36, YELLOW);
}

void Menu::shutdown() {}
