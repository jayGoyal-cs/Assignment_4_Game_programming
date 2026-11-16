/**
* Author: Jay Goyal
* Assignment: Rise of the AI
* Date due: 11/16/2026
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#include "CS3113/LevelA.h"
#include "CS3113/LevelB.h"
#include "CS3113/LevelC.h"
#include "CS3113/LevelD.h"
#include "CS3113/LevelE.h"
#include "CS3113/Menu.h"
#include "CS3113/WinScene.h"
#include "CS3113/LoseScene.h"

// Global Constants
constexpr int SCREEN_WIDTH  = 1000,
              SCREEN_HEIGHT = 600,
              FPS           = 120,
              MAX_LIVES     = 3;

constexpr Vector2 ORIGIN = { SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

constexpr float FIXED_TIMESTEP = 1.0f / 60.0f;

// Global Variables
AppStatus gAppStatus       = RUNNING;
float     gPreviousTicks   = 0.0f,
          gTimeAccumulator = 0.0f;

Camera2D gCamera = {};

Scene                     *gCurrentScene   = nullptr;
SceneID                    gCurrentSceneID = MENU;
std::map<SceneID, Scene *> gLevels         = {};

int gLives = MAX_LIVES;

Music gBgm;
Sound gJumpSound,
      gLandingSound,
      gSnapSound;

bool gWasOnGround = true;

// Function Declarations
void switchToScene(SceneID id);
void shutdownLevels();
void startLevels();
void startBoss();
bool isBossStage(SceneID id);
void snapToLevel(SceneID id);
bool isLevel(SceneID id);
Vector2 getCameraTarget();
void initialise();
void processInput();
void update();
void render();
void shutdown();

bool isLevel(SceneID id)
{
    return id == LEVEL_A || id == LEVEL_B || id == LEVEL_C ||
           id == LEVEL_D || id == LEVEL_E;
}

bool isBossStage(SceneID id)
{
    return id == LEVEL_D || id == LEVEL_E;
}

void shutdownLevels()
{
    gLevels[LEVEL_A]->shutdown();
    gLevels[LEVEL_B]->shutdown();
    gLevels[LEVEL_C]->shutdown();
    gLevels[LEVEL_D]->shutdown();
    gLevels[LEVEL_E]->shutdown();
}

// the camera follows the player horizontally but stays inside the level
Vector2 getCameraTarget()
{
    if (!isLevel(gCurrentSceneID)) return ORIGIN;

    float x          = gCurrentScene->getState().player->getPosition().x;
    float levelWidth = LEVEL_WIDTH * LevelA::TILE_DIMENSION;

    if (x < ORIGIN.x)              x = ORIGIN.x;
    if (x > levelWidth - ORIGIN.x) x = levelWidth - ORIGIN.x;

    return { x, ORIGIN.y };
}

// switch to the menu / win / lose screen (the three worlds are unloaded)
void switchToScene(SceneID id)
{
    shutdownLevels();

    gCurrentSceneID = id;
    gCurrentScene   = gLevels[id];
    gCurrentScene->initialise();
    gCamera.target = getCameraTarget();
}

// (re)load all three worlds and start in world 1
void startLevels()
{
    shutdownLevels();

    gLevels[LEVEL_A]->initialise();
    gLevels[LEVEL_B]->initialise();
    gLevels[LEVEL_C]->initialise();

    gCurrentSceneID = LEVEL_A;
    gCurrentScene   = gLevels[LEVEL_A];
    gCamera.target  = getCameraTarget();
    gWasOnGround    = true;
}

// (re)load the boss arena and the button room and start in the arena
void startBoss()
{
    shutdownLevels();

    gLevels[LEVEL_D]->initialise();
    gLevels[LEVEL_E]->initialise();

    gCurrentSceneID = LEVEL_D;
    gCurrentScene   = gLevels[LEVEL_D];
    gCamera.target  = getCameraTarget();
    gWasOnGround    = true;
}

// snap to another world, keeping the player's position and velocity
void snapToLevel(SceneID id)
{
    if (id == gCurrentSceneID) return;

    Entity *oldPlayer = gCurrentScene->getState().player;
    Entity *newPlayer = gLevels[id]->getState().player;
    Map    *newMap    = gLevels[id]->getState().map;

    // don't allow snapping into a wall or the ground in the other world
    Vector2 position = oldPlayer->getPosition();
    Vector2 halfSize = Vector2Scale(oldPlayer->getColliderDimensions(), 0.5f);
    float   xOverlap, yOverlap;

    // corners of the hitbox, moved 2 pixels inward so that standing on the
    // floor doesn't count as being inside the floor
    float left   = position.x - halfSize.x + 2.0f;
    float right  = position.x + halfSize.x - 2.0f;
    float top    = position.y - halfSize.y + 2.0f;
    float bottom = position.y + halfSize.y - 2.0f;

    // don't snap if any corner would be inside a solid tile in the other world
    if (newMap->isSolidTileAt({ left,  top    }, &xOverlap, &yOverlap) ||
        newMap->isSolidTileAt({ right, top    }, &xOverlap, &yOverlap) ||
        newMap->isSolidTileAt({ left,  bottom }, &xOverlap, &yOverlap) ||
        newMap->isSolidTileAt({ right, bottom }, &xOverlap, &yOverlap))
        return;

    newPlayer->setPosition(position);
    newPlayer->setVelocity(oldPlayer->getVelocity());
    newPlayer->setDirection(oldPlayer->getDirection());

    gCurrentSceneID = id;
    gCurrentScene   = gLevels[id];

    PlaySound(gSnapSound);
}

void initialise()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Rise of the Frog");
    InitAudioDevice();

    gBgm = LoadMusicStream("assets/game/Music.mp3");
    SetMusicVolume(gBgm, 0.5f);
    PlayMusicStream(gBgm);

    gJumpSound    = LoadSound("assets/game/Jump_effect.mp3");
    gLandingSound = LoadSound("assets/game/Landing_effect.mp3");
    gSnapSound    = LoadSound("assets/game/snap.mp3");

    gLevels[MENU]    = new Menu(ORIGIN, "#011627");
    gLevels[LEVEL_A] = new LevelA(ORIGIN, "#C3F58F");
    gLevels[LEVEL_B] = new LevelB(ORIGIN, "#AEF6F5");
    gLevels[LEVEL_C] = new LevelC(ORIGIN, "#F9BDF9");
    gLevels[LEVEL_D] = new LevelD(ORIGIN, "#2B2D42");
    gLevels[LEVEL_E] = new LevelE(ORIGIN, "#FFE5B4");
    gLevels[WIN]     = new WinScene(ORIGIN, "#1B5E20");
    gLevels[LOSE]    = new LoseScene(ORIGIN, "#7F0000");

    gCamera.offset   = ORIGIN;
    gCamera.rotation = 0.0f;
    gCamera.zoom     = 1.0f;

    switchToScene(MENU);

    SetTargetFPS(FPS);
}

void processInput()
{
    if (IsKeyPressed(KEY_Q) || WindowShouldClose()) gAppStatus = TERMINATED;

    // menu, win and lose screens
    if (!isLevel(gCurrentSceneID))
    {
        if (IsKeyPressed(KEY_ENTER))
        {
            gLives = MAX_LIVES;
            if (gCurrentSceneID == MENU) startLevels();
            else                         switchToScene(MENU);
        }
        return;
    }

    if (isBossStage(gCurrentSceneID))
    {
        // boss fight: snap between the arena and the button room
        if (IsKeyPressed(KEY_ONE)) snapToLevel(LEVEL_D);
        if (IsKeyPressed(KEY_TWO)) snapToLevel(LEVEL_E);
    }
    else
    {
        // snap between the three worlds with the number keys
        if (IsKeyPressed(KEY_ONE))   snapToLevel(LEVEL_A);
        if (IsKeyPressed(KEY_TWO))   snapToLevel(LEVEL_B);
        if (IsKeyPressed(KEY_THREE)) snapToLevel(LEVEL_C);
    }

    // debug: jump straight to the boss fight
    if (IsKeyPressed(KEY_FOUR))
    {
        startBoss();
        return;
    }

    Entity *player = gCurrentScene->getState().player;

    player->resetMovement();

    if      (IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT))  player->moveLeft();
    else if (IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) player->moveRight();

    if ((IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_SPACE))
        && player->isCollidingBottom())
    {
        player->jump();
        PlaySound(gJumpSound);
    }

    // to avoid faster diagonal speed
    if (GetLength(player->getMovement()) > 1.0f) player->normaliseMovement();
}

void update()
{
    UpdateMusicStream(gBgm);

    // Delta time
    float ticks     = static_cast<float>(GetTime());
    float deltaTime = ticks - gPreviousTicks;
    gPreviousTicks  = ticks;

    // Fixed timestep
    deltaTime += gTimeAccumulator;

    if (deltaTime < FIXED_TIMESTEP)
    {
        gTimeAccumulator = deltaTime;
        return;
    }

    while (deltaTime >= FIXED_TIMESTEP)
    {
        gCurrentScene->update(FIXED_TIMESTEP);
        deltaTime -= FIXED_TIMESTEP;

        Vector2 cameraTarget = getCameraTarget();
        panCamera(&gCamera, &cameraTarget);

        // landing sound when the player touches the ground after being airborne
        if (isLevel(gCurrentSceneID))
        {
            bool onGround = gCurrentScene->getState().player->isCollidingBottom();
            if (onGround && !gWasOnGround) PlaySound(gLandingSound);
            gWasOnGround = onGround;
        }

        GameState state = gCurrentScene->getState();
        if (state.playerDied || state.nextSceneID != NO_SCENE) break;
    }

    gTimeAccumulator = deltaTime;
}

void render()
{
    BeginDrawing();
    BeginMode2D(gCamera);

    gCurrentScene->render();

    EndMode2D();

    if (isLevel(gCurrentSceneID))
    {
        DrawText(TextFormat("Lives: %i", gLives), 20, 20, 30, BLACK);
        if (isBossStage(gCurrentSceneID))
        {
            const char *room = gCurrentSceneID == LEVEL_D ? "1: Boss Arena"
                                                          : "2: Button Room";
            const char *hint = "Get past the giant frog, snap to the Button Room "
                               "and press the button!";

            DrawText(TextFormat("Lives: %i", gLives), 20, 20, 30, WHITE);
            DrawText(room, SCREEN_WIDTH - MeasureText(room, 30) - 20, 20, 30,
                     gCurrentSceneID == LEVEL_D ? WHITE : BLACK);
            DrawText(hint, SCREEN_WIDTH / 2 - MeasureText(hint, 20) / 2, 60, 20,
                     gCurrentSceneID == LEVEL_D ? WHITE : BLACK);
        }
        else
        {
            DrawText(TextFormat("World %i", gCurrentSceneID), SCREEN_WIDTH - 140,
                     20, 30, BLACK);
        }
    }

    EndDrawing();
}

void shutdown()
{
    for (std::pair<const SceneID, Scene *> &entry : gLevels)
        delete entry.second;

    gLevels.clear();

    UnloadMusicStream(gBgm);
    UnloadSound(gJumpSound);
    UnloadSound(gLandingSound);
    UnloadSound(gSnapSound);

    CloseAudioDevice();
    CloseWindow();
}

int main(void)
{
    initialise();

    while (gAppStatus == RUNNING)
    {
        processInput();
        update();

        GameState state = gCurrentScene->getState();

        if (state.playerDied)
        {
            gLives--;

            if (gLives <= 0)                       switchToScene(LOSE);
            else if (isBossStage(gCurrentSceneID)) startBoss();
            else                                   startLevels();
        }
        else if (state.nextSceneID == LEVEL_D)
        {
            startBoss();
        }
        else if (state.nextSceneID != NO_SCENE)
        {
            switchToScene(state.nextSceneID);
        }

        render();
    }

    shutdown();

    return 0;
}
