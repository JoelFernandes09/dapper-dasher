#include "raylib.h"
#include "cstdio"
#include <string>
#include <stdlib.h>
#include <ctime>

struct AnimData
{
    Rectangle rect;
    Vector2 pos;
    int frame;
    float updateTime;
    float runningTime;
};

bool isPlayerOnGround(AnimData player, int windowHeight)
{
    return player.pos.y >= (windowHeight - player.rect.height);
}

AnimData updatePlayerAnim(AnimData playerAnim, bool isInAir)
{
    if (playerAnim.runningTime >= playerAnim.updateTime && !isInAir)
    {
        playerAnim.rect.x = playerAnim.frame * playerAnim.rect.width;
        playerAnim.frame++;
        if (playerAnim.frame > 5)
        {
            playerAnim.frame = 0;
        }
        playerAnim.runningTime = 0;
    }
    return playerAnim;
}

AnimData updateObstacleAnim(AnimData obstacleAnim, int obstacleFrameHeight, int windowWidth, float obstacleVelocity, float deltaTime)
{
    if (obstacleAnim.runningTime >= obstacleAnim.updateTime)
    {
        obstacleAnim.rect.x = obstacleAnim.frame * obstacleAnim.rect.width;
        obstacleAnim.rect.y = obstacleFrameHeight * obstacleAnim.rect.height;
        obstacleAnim.frame++;
        if (obstacleFrameHeight > 6 && obstacleAnim.frame > 4)
        {
            obstacleAnim.frame = 0;
            obstacleFrameHeight = 0;
            obstacleAnim.runningTime = 0;
        }
        if (obstacleAnim.frame > 7)
        {
            obstacleFrameHeight++;
            obstacleAnim.frame = 0;
            obstacleAnim.runningTime = 0;
        }
    }

    return obstacleAnim;
}

int random(int min, int max)
{
    static bool first = true;
    if (first)
    {
        srand(time(NULL));
        first = false;
    }
    return min + rand() % ((max + 1) - min);
}

int main()
{
    const int windowWidth = 1280, windowHeight = 720;
    int velocity{0};
    const int gravity{1000}, jumpVelocity{-750}; // Acceleration due to gravity (pixels/frame)/frame
    bool isInAir = false;

    InitWindow(windowWidth, windowHeight, "Dapper Dasher");

    // Player Initialization
    Texture2D player = LoadTexture("textures/scarfy.png");
    AnimData playerAnim = {
        {0.0, 0.0, static_cast<float>(player.width / 6), static_cast<float>(player.height)},
        {(windowWidth / 2 - playerAnim.rect.width / 2), windowHeight - playerAnim.rect.height},
        0,
        1.0 / 12.0,
        0,
    };

    // Obstacle Initializations
    float obstacleVelocity{-200};

    Texture2D obstacleText = LoadTexture("textures/12_nebula_spritesheet.png");

    AnimData obstacle1Anim = {
        {0.0, 0.0, static_cast<float>(obstacleText.width / 8), static_cast<float>(obstacleText.height / 8)},
        {windowWidth, (windowHeight - obstacle1Anim.rect.height)},
        0,
        1.0 / 12.0,
        0,
    };

    AnimData obstacle2Anim = {
        {0.0, 0.0, static_cast<float>(obstacleText.width / 8), static_cast<float>(obstacleText.height / 8)},
        {(windowWidth + 300), (windowHeight - obstacle1Anim.rect.height)},
        0,
        1.0 / 12.0,
        0,
    };

    AnimData obstacles[2]{obstacle1Anim, obstacle2Anim};
    int obstacleFrameHeight[2]{0, 0};

    Texture2D backgroundFar = LoadTexture("textures/far-buildings.png");
    Texture2D backgroundBack = LoadTexture("textures/back-buildings.png");
    Texture2D foreground = LoadTexture("textures/foreground.png");
    float backgroundX{0}, backgroundBackX{0}, foregroundX{0};
    float scrollMultiplier{1}, obstacleSpeedMultiplier{1};
    bool collision{false};
    bool gameOver{false};
    bool gameStarted{false};
    Vector2 backgroundPos{};
    Vector2 backgroundPos2{};
    Vector2 backgroundBackPos{};
    Vector2 backgroundBackPos2{};
    Vector2 foregroundPos{};
    Vector2 foregroundPos2{};

    float score{0};

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(WHITE);

        const float deltaTime{GetFrameTime()}; // Time between frames

        if (!gameOver && gameStarted)
        {
            if (isPlayerOnGround(playerAnim, windowHeight))
            {
                velocity = 0;
                isInAir = false;
            }
            else
            {
                velocity += gravity * deltaTime;
                isInAir = true;
            }

            if (IsKeyPressed(KEY_SPACE) && !isInAir)
            {
                velocity += jumpVelocity;
            }

            playerAnim.pos.y += velocity * deltaTime;

            // Updating Animation for the Sprite
            playerAnim.runningTime += deltaTime;
            playerAnim = updatePlayerAnim(playerAnim, isInAir);

            for (int i = 0; i < 2; i++)
            {
                int randomDist = random(150, 400);
                obstacles[i].runningTime += deltaTime;
                obstacles[i] = updateObstacleAnim(obstacles[i], obstacleFrameHeight[i], windowWidth, obstacleVelocity, deltaTime);

                obstacles[i].pos.x += obstacleSpeedMultiplier * obstacleVelocity * deltaTime;
                if (obstacles[i].pos.x < -obstacles[i].rect.width)
                {
                    obstacles[i].pos.x = windowWidth + randomDist;
                }
            }
            backgroundX -= scrollMultiplier * 20 * deltaTime;
            if (backgroundX <= -backgroundFar.width * 6)
                backgroundX = 0;
            backgroundBackX -= scrollMultiplier * 40 * deltaTime;
            if (backgroundBackX <= -backgroundBack.width * 6)
                backgroundBackX = 0;
            foregroundX -= scrollMultiplier * 80 * deltaTime;
            if (foregroundX <= -foreground.width * 6)
                foregroundX = 0;
            backgroundPos = {backgroundX, 0};
            backgroundPos2 = {backgroundX + backgroundFar.width * 6, 0};
            backgroundBackPos = {backgroundBackX, 0};
            backgroundBackPos2 = {backgroundBackX + backgroundBack.width * 6, 0};
            foregroundPos = {foregroundX, 0};
            foregroundPos2 = {foregroundX + foreground.width * 6, 0};
            DrawTextureEx(backgroundFar, backgroundPos, 0, 6, WHITE);
            DrawTextureEx(backgroundFar, backgroundPos2, 0, 6, WHITE);
            DrawTextureEx(backgroundBack, backgroundBackPos, 0, 6, WHITE);
            DrawTextureEx(backgroundBack, backgroundBackPos2, 0, 6, WHITE);
            DrawTextureEx(foreground, foregroundPos, 0, 6, WHITE);
            DrawTextureEx(foreground, foregroundPos2, 0, 6, WHITE);

            for (AnimData obstacle : obstacles)
            {
                float pad{40};
                Rectangle obsRec{obstacle.pos.x + pad, obstacle.pos.y + pad, obstacle.rect.width - 2 * pad, obstacle.rect.height - 2 * pad};
                Rectangle playerRec{playerAnim.pos.x, playerAnim.pos.y, playerAnim.rect.width, playerAnim.rect.height};
                if (CheckCollisionRecs(obsRec, playerRec))
                {
                    collision = true;
                }
            }

            if (collision == true)
            {
                DrawTextureRec(player, playerAnim.rect, playerAnim.pos, RED);
                DrawText("Game Over", windowWidth / 2 - 300, windowHeight / 2 - 250, 100, WHITE);
                gameOver = true;
            }
            else
            {
                DrawText(TextFormat("Score: %.1f m", score), windowWidth / 2 - 300, windowHeight / 2 - 250, 100, WHITE);
                DrawTextureRec(player, playerAnim.rect, playerAnim.pos, WHITE);
                DrawTextureRec(obstacleText, obstacles[0].rect, obstacles[0].pos, WHITE);
                DrawTextureRec(obstacleText, obstacles[1].rect, obstacles[1].pos, WHITE);
            }
            score += 0.01;
            scrollMultiplier += 0.001;
            obstacleSpeedMultiplier += 0.001;
        }
        else
        {
            DrawTextureEx(backgroundFar, backgroundPos, 0, 6, WHITE);
            DrawTextureEx(backgroundFar, backgroundPos2, 0, 6, WHITE);
            DrawTextureEx(backgroundBack, backgroundBackPos, 0, 6, WHITE);
            DrawTextureEx(backgroundBack, backgroundBackPos2, 0, 6, WHITE);
            DrawTextureEx(foreground, foregroundPos, 0, 6, WHITE);
            DrawTextureEx(foreground, foregroundPos2, 0, 6, WHITE);
            DrawTextureRec(player, playerAnim.rect, playerAnim.pos, WHITE);
            if (gameStarted)
            {
                DrawText("Game Over", windowWidth / 2 - 300, windowHeight / 2 - 250, 100, WHITE);
                DrawText(TextFormat("Score: %.1f m", score), windowWidth / 2 - 300, windowHeight / 2 - 100, 100, WHITE);
            }
        }

        if (IsKeyPressed(KEY_ENTER))
        {
            gameStarted = true;
        }

        if (!gameStarted)
        {
            DrawText("Press ENTER to start...", 200, windowHeight / 2 - 250, 80, WHITE);
        }

        EndDrawing();
    }

    UnloadTexture(player);
    UnloadTexture(obstacleText);
    UnloadTexture(backgroundFar);
    UnloadTexture(backgroundBack);
    UnloadTexture(foreground);
    CloseWindow();
}