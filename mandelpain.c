#include <math.h>
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

#define EPS (1e-6)

#define MIN(a, b) ((b) > (a) ? a : b)
#define MAX(a, b) ((a) > (b) ? a : b)

float lerp(float a, float b, float t) { return (fabs(a - b) < EPS) ? b : (1.0f - t) * a + t * b; }

Vector2 lerpVector2(Vector2 a, Vector2 b, float t) {
    Vector2 ret = {lerp(a.x, b.x, t), lerp(a.y, b.y, t)};
    return ret;
}

unsigned char mdlc(float cr, float ci, int maxiter) {
    float zr = 0.0f;
    float zi = 0.0f;

    for (int i = 0; i < maxiter; ++i) {
        float zr2 = zr * zr - zi * zi + cr;
        float zi2 = 2.0f * zr * zi + ci;

        zr = zr2;
        zi = zi2;
        if (zr * zr + zi * zi > 4.0) {
            return i % 255;
        }
    }

    return 0;
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

    float re = 0.0;
    float im = 0.0;
    
    float zoom = 1.5;
    float moveFactor = 4.0;
    float step = 12.0;
    int showInfo = 1;
    
    int cpuMode = 0;
    int lineChunks = 64;
    int lineChunkSize = H / lineChunks;
    int curLine = 0;
    int stopLine = curLine + lineChunkSize;

    int moved = 0;
    float elapsed = 0.0f;
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        moved = 0;
        if (IsKeyDown(KEY_A)) { re -= zoom * moveFactor * dt; moved = 1; }
        if (IsKeyDown(KEY_D)) { re += zoom * moveFactor * dt; moved = 1; }
        if (IsKeyDown(KEY_S)) { im -= zoom * moveFactor * dt; moved = 1; }
        if (IsKeyDown(KEY_W)) { im += zoom * moveFactor * dt; moved = 1; }

        if (IsKeyDown(KEY_E)) { zoom /= 1.0f + moveFactor * dt; moved = 1; }
        if (IsKeyDown(KEY_Q)) { zoom *= 1.0f + moveFactor * dt; moved = 1; }

        if (IsKeyDown(KEY_M)) { maxiter += IsKeyDown(KEY_LEFT_SHIFT) ? 100 : 1; moved = 1; }
        if (IsKeyDown(KEY_N)) { maxiter = MAX(2, maxiter - (IsKeyDown(KEY_LEFT_SHIFT) ? 100 : 1)); moved = 1; }
        
        if (IsKeyPressed(KEY_H)) showInfo = !showInfo;
        if (IsKeyPressed(KEY_P)) palette = !palette;
        if (IsKeyPressed(KEY_G)) { cpuMode = !cpuMode; moved = 1; }

        if (cpuMode && moved == 1) {
            curLine = 0;
            stopLine = curLine + lineChunkSize;
        }

        Vector2 xGoal = {re - zoom * ratio, re + zoom * ratio};
        Vector2 yGoal = {im - zoom, im + zoom};

        x = lerpVector2(x, xGoal, step * dt);
        y = lerpVector2(y, yGoal, step * dt);

        SetShaderValue(shader, paletteLoc, &palette, SHADER_UNIFORM_INT);
        SetShaderValue(shader, maxiterLoc, &maxiter, SHADER_UNIFORM_INT);
        SetShaderValue(shader, xLoc, &x, SHADER_UNIFORM_VEC2);
        SetShaderValue(shader, yLoc, &y, SHADER_UNIFORM_VEC2);

        BeginDrawing();

        if (cpuMode) {
            ClearBackground(BLACK);

            BeginTextureMode(target);

            if (curLine < H && !moved) {
                for (int i = curLine; i < stopLine; i ++) {
                    for (int j = 0; j < W; ++j) {
                        float cr = x.x + ((float) j / (float) W) * (x.y - x.x);
                        float ci = y.x + ((float) i / (float) H) * (y.y - y.x);

                        unsigned char c = mdlc(cr, ci, maxiter);
                        Color pixel = {c, c, c, 255};
                        DrawPixel(j, i, pixel);
                    }
                }
                curLine += lineChunkSize;
                stopLine = MIN(stopLine + lineChunkSize, H);
            }


            EndTextureMode();

            DrawTexture(target.texture, 0, 0, WHITE);
        } else {
            BeginShaderMode(shader);
            DrawTexture(target.texture, 0, 0, WHITE);
            EndShaderMode();
        }

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
