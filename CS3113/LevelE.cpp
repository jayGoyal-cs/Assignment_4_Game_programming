

#include "LevelE.h"

LevelE::LevelE()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

LevelE::LevelE(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}

LevelE::~LevelE() { shutdown(); }

void LevelE::initialise()
{
    mGameState.nextSceneID = NO_SCENE;
    mGameState.playerDied  = false;
    mGameState.goal        = nullptr;

    /*
        ----------- MAP -----------
    */
    mGameState.map = new Map(
        LEVEL_WIDTH, LEVEL_HEIGHT,                          // map grid cols & rows
        mLevelData,                                         // grid data
        "assets/game/Pixel Adventure 1/Free/Terrain/Terrain (16x16).png",
        TILE_DIMENSION,                                     // tile size
        22, 11,                                             // texture cols & rows
        { LEVEL_WIDTH * TILE_DIMENSION / 2.0f,               // map starts at (0, 0)
          LEVEL_HEIGHT * TILE_DIMENSION / 2.0f }
    );

    /*
        ----------- PROTAGONIST -----------
    */
    std::map<Direction, std::vector<int>> frogAnimationAtlas = {
        { LEFT,  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 } },
        { RIGHT, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 } },
    };

    mGameState.player = new Entity(
        { 125.0f, 425.0f },                                    // position
        { 64.0f, 64.0f },                                      // scale
        "assets/game/Pixel Adventure 1/Free/Main Characters/Ninja Frog/Run (32x32).png",
        ATLAS,                                                 // single image or atlas?
        { 1, 12 },                                             // atlas dimensions
        frogAnimationAtlas,                                    // actual atlas
        PLAYER                                                 // entity type
    );

    mGameState.player->setJumpingPower(550.0f);
    mGameState.player->setColliderDimensions({ 40.0f, 48.0f });
    mGameState.player->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });

    /*
        ----------- BUTTON (press it to beat the boss) -----------
    */
    mGameState.goal = new Entity(
        { 33 * TILE_DIMENSION + TILE_DIMENSION / 2.0f, FLOOR_Y - 24.0f },
        { 48.0f, 48.0f },
        "assets/game/Pixel Adventure 1/Free/Menu/Buttons/Play.png",
        BLOCK
    );
    mGameState.goal->setColliderDimensions({ 40.0f, 48.0f });
}

void LevelE::update(float deltaTime)
{
    mGameState.player->update(deltaTime, nullptr, mGameState.map, nullptr, 0);

    if (mGameState.player->getPosition().y > END_GAME_THRESHOLD)
        mGameState.playerDied = true;

    // pressing the button defeats the giant frog
    if (mGameState.player->collidesWith(mGameState.goal))
        mGameState.nextSceneID = WIN;
}

void LevelE::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.map->render();
    mGameState.goal->render();
    mGameState.player->render();
}

void LevelE::shutdown()
{
    delete mGameState.player;
    mGameState.player = nullptr;
    delete mGameState.map;
    mGameState.map = nullptr;
    delete mGameState.goal;
    mGameState.goal = nullptr;

    for (Entity *enemy : mGameState.enemies) delete enemy;
    mGameState.enemies.clear();
}
