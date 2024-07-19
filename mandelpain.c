#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RAYLIB_IMPLEMENTATION

#include "raylib.h"

#define W 1000
#define H 1000

#define X1 (-1.5)
#define X2 (1.5)
#define Y1 (-1.5)
#define Y2 (1.5)

#define MAX(a, b) ((a) > (b) ? a : b)

float lerp(float a, float b, float t) { return (1.0f - t) * a + t * b; }

Vector2 lerpVector2(Vector2 a, Vector2 b, float t) {
    Vector2 ret = {lerp(a.x, b.x, t), lerp(a.y, b.y, t)};
    return ret;
}

int main(void) {
    int w = W;
    int h = H;
    float ratio = (float) w / h;

    InitWindow(w, h, "mandelpain");

    RenderTexture target = LoadRenderTexture(w, h);
    Shader shader = LoadShader(0, "./shader.glsl");

    int paletteLoc = GetShaderLocation(shader, "palette");
    int maxiterLoc = GetShaderLocation(shader, "maxiter");
    int xLoc = GetShaderLocation(shader, "x");
    int yLoc = GetShaderLocation(shader, "y");

    int palette = 0;
    int maxiter = 512;
    Vector2 x = {X1, X2};
    Vector2 y = {Y1, Y2};

    SetShaderValue(shader, paletteLoc, &palette, SHADER_UNIFORM_INT);
    SetShaderValue(shader, maxiterLoc, &maxiter, SHADER_UNIFORM_INT);
    SetShaderValue(shader, xLoc, &x, SHADER_UNIFORM_VEC2);
    SetShaderValue(shader, yLoc, &y, SHADER_UNIFORM_VEC2);

    float re = 0.0f;
    float im = 0.0f;
    
    float zoom = 1.5f;
    float moveFactor = 4.0f;
    float step = 12.0f;
    int showInfo = 1;

    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (IsKeyDown(KEY_A)) re -= zoom * moveFactor * dt;
        if (IsKeyDown(KEY_D)) re += zoom * moveFactor * dt;
        if (IsKeyDown(KEY_S)) im += zoom * moveFactor * dt;
        if (IsKeyDown(KEY_W)) im -= zoom * moveFactor * dt;

        if (IsKeyDown(KEY_E)) zoom /= 1.0f + moveFactor * dt;
        if (IsKeyDown(KEY_Q)) zoom *= 1.0f + moveFactor * dt;

        if (IsKeyDown(KEY_M)) maxiter += 1;
        if (IsKeyDown(KEY_N)) maxiter = MAX(2, maxiter - 2);
        
        if (IsKeyPressed(KEY_H)) showInfo = !showInfo;
        if (IsKeyPressed(KEY_P)) palette = !palette;

        Vector2 xGoal = {re - zoom * ratio, re + zoom * ratio};
        Vector2 yGoal = {im - zoom, im + zoom};

        x = lerpVector2(x, xGoal, step * dt);
        y = lerpVector2(y, yGoal, step * dt);

        SetShaderValue(shader, paletteLoc, &palette, SHADER_UNIFORM_INT);
        SetShaderValue(shader, maxiterLoc, &maxiter, SHADER_UNIFORM_INT);
        SetShaderValue(shader, xLoc, &x, SHADER_UNIFORM_VEC2);
        SetShaderValue(shader, yLoc, &y, SHADER_UNIFORM_VEC2);

        BeginDrawing();

        BeginShaderMode(shader);
        DrawTexture(target.texture, 0, 0, WHITE);
        EndShaderMode();

        if (showInfo) {
            DrawFPS(10, 10);
            DrawText(TextFormat("re = %f", re), W - 400, H - 100, 50, GREEN);
            DrawText(TextFormat("im = %f", im), W - 400, H - 50, 50, GREEN);
            DrawText(TextFormat("maxiter = %d", maxiter), 10, H - 50, 50, GREEN);
        }

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
