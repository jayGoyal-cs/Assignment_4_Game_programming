#include "LevelA.h"

LevelA::LevelA()
    : Scene{ { 0.0f, 0.0f }, "#000000" }
{}

LevelA::LevelA(Vector2 origin, const char *bgHexCode)
    : Scene{ origin, bgHexCode }
{}

LevelA::~LevelA() { shutdown(); }

Entity *LevelA::makeSpikedBall(int col)
{
    Entity *ball = new Entity(
        { col * TILE_DIMENSION + TILE_DIMENSION / 2.0f, 325.0f },
        { 40.0f, 40.0f },
        "assets/game/Pixel Adventure 1/Free/Traps/Spiked Ball/Spiked Ball.png",
        NPC
    );
    ball->setColliderDimensions({ 32.0f, 32.0f });
    ball->setSpeed(120);
    ball->setAIType(FLYER);
    ball->setAIState(IDLE);
    return ball;
}

Entity *LevelA::makeSaw(int col)
{
    std::map<Direction, std::vector<int>> sawAtlas = {
        { LEFT,  { 0, 1, 2, 3, 4, 5, 6, 7 } },
        { RIGHT, { 0, 1, 2, 3, 4, 5, 6, 7 } },
    };

    Entity *saw = new Entity(
        { col * TILE_DIMENSION + TILE_DIMENSION / 2.0f, FLOOR_Y - 20.0f },
        { 50.0f, 50.0f },
        "assets/game/Pixel Adventure 1/Free/Traps/Saw/On (38x38).png",
        ATLAS, { 1, 8 }, sawAtlas, NPC
    );
    saw->setColliderDimensions({ 40.0f, 40.0f });
    saw->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    saw->setSpeed(100);
    saw->setAIType(WANDERER);
    saw->setAIState(WALKING);
    return saw;
}

Entity *LevelA::makeSpikeHead(int col)
{
    Entity *spikeHead = new Entity(
        { col * TILE_DIMENSION + TILE_DIMENSION / 2.0f, FLOOR_Y - 20.0f },
        { 54.0f, 52.0f },
        "assets/game/Pixel Adventure 1/Free/Traps/Spike Head/Idle.png",
        NPC
    );
    spikeHead->setColliderDimensions({ 40.0f, 40.0f });
    spikeHead->setAcceleration({ 0.0f, ACCELERATION_OF_GRAVITY });
    spikeHead->setSpeed(110);
    spikeHead->setAIType(FOLLOWER);
    spikeHead->setAIState(IDLE);
    return spikeHead;
}

void LevelA::initialise()
{
    mGameState.nextSceneID = NO_SCENE;
    mGameState.playerDied  = false;

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
        ----------- GOAL (end of the level) -----------
    */
    mGameState.goal = new Entity(
        { 37 * TILE_DIMENSION + TILE_DIMENSION / 2.0f, FLOOR_Y - 32.0f },
        { 64.0f, 64.0f },
        "assets/game/Pixel Adventure 1/Free/Items/Checkpoints/End/End (Idle).png",
        BLOCK
    );
    mGameState.goal->setColliderDimensions({ 48.0f, 64.0f });

    /*
        ----------- ENEMIES -----------
    */
    // wanderer: a spinning saw that walks back and forth
    mGameState.enemies.push_back(makeSaw(28));
}

void LevelA::update(float deltaTime)
{
    mGameState.player->update(
        deltaTime,      // delta time / fixed timestep
        nullptr,        // player
        mGameState.map, // map
        nullptr,        // collidable entities
        0               // col. entity count
    );

    for (Entity *enemy : mGameState.enemies)
        enemy->update(deltaTime, mGameState.player, mGameState.map, nullptr, 0);

    // collisions are checked from the player's perspective
    for (Entity *enemy : mGameState.enemies)
        if (mGameState.player->collidesWith(enemy))
            mGameState.playerDied = true;

    // fell into a pit
    if (mGameState.player->getPosition().y > END_GAME_THRESHOLD)
        mGameState.playerDied = true;

    // reached the end flag
    if (mGameState.player->collidesWith(mGameState.goal))
        mGameState.nextSceneID = LEVEL_D; // on to the boss fight
}

void LevelA::render()
{
    ClearBackground(ColorFromHex(mBGColourHexCode));

    mGameState.map->render();
    mGameState.goal->render();
    for (Entity *enemy : mGameState.enemies) enemy->render();
    mGameState.player->render();
}

void LevelA::shutdown()
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
