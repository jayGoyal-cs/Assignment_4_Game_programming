#include "LevelD.h"

LevelD::LevelD()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

LevelD::LevelD(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}

LevelD::~LevelD() { shutdown(); }

void LevelD::initialise()
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
        ----------- BOSS (giant frog) -----------
    */
    std::map<Direction, std::vector<int>> bossAnimationAtlas = {
        { LEFT,  { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 } },
        { RIGHT, { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 } },
    };

    Entity *boss = new Entity(
        { 28 * TILE_DIMENSION, FLOOR_Y - 65.0f },              // position
        { 160.0f, 160.0f },                                    // scale (giant!)
        "assets/game/Pixel Adventure 1/Free/Main Characters/Ninja Frog/Idle (32x32).png",
        ATLAS, { 1, 11 }, bossAnimationAtlas, NPC
    );
    boss->setColliderDimensions({ 100.0f, 130.0f });
    boss->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    boss->setJumpingPower(550.0f);
    boss->setSpeed(80);
    boss->setAIType(FOLLOWER);
    boss->setAIState(WALKING);   // always chasing the player

    mGameState.enemies.push_back(boss);
    mHopTimer = 0.0f;
}

void LevelD::update(float deltaTime)
{
    mGameState.player->update(deltaTime, nullptr, mGameState.map, nullptr, 0);

    Entity *boss = mGameState.enemies[0];
    boss->update(deltaTime, mGameState.player, mGameState.map, nullptr, 0);

    // the boss hops every few seconds
    mHopTimer += deltaTime;
    if (mHopTimer >= HOP_INTERVAL && boss->isCollidingBottom())
    {
        boss->jump();
        mHopTimer = 0.0f;
    }

    // touching the boss costs a life (checked from the player's perspective)
    if (mGameState.player->collidesWith(boss)) mGameState.playerDied = true;

    if (mGameState.player->getPosition().y > END_GAME_THRESHOLD)
        mGameState.playerDied = true;
}

void LevelD::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.map->render();
    for (Entity *enemy : mGameState.enemies) enemy->render();
    mGameState.player->render();
}

void LevelD::shutdown()
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
