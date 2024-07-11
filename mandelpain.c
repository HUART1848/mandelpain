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

const char *shaderSource =
    "#version 330\n"
    "in vec2 fragTexCoord;\n"
    "uniform int maxiter;\n"
    "uniform vec2 x;\n"
    "uniform vec2 y;\n"
    "out vec4 finalColor;\n"
    "void main() {\n"
    "    float cr = x.x + fragTexCoord.x * (x.y - x.x);\n"
    "    float ci = y.x + fragTexCoord.y * (y.y - y.x);\n"
    "    float zr = 0.0;\n"
    "    float zi = 0.0;\n"
    "    for (float i = 0; i < maxiter; ++i) {\n"
    "        float zr2 = zr * zr - zi * zi + cr;\n"
    "        float zi2 = 2.0 * zr * zi + ci;\n"
    "        zr = zr2;\n"
    "        zi = zi2;\n"
    "        if (zr * zr + zi * zi > 4.0) {\n"
    "            float c = i / maxiter;\n"
    "            finalColor = vec4(c, c, c, 1.0);\n"
    "            return;\n"
    "        }\n"
    "    }\n"
    "    finalColor = vec4(0.0, 0.0, 0.0, 1.0);\n"
    "}";

float lerp(float a, float b, float t) { return (1.0f - t) * a + t * b; }

Vector2 lerpVector2(Vector2 a, Vector2 b, float t) {
    Vector2 ret = {lerp(a.x, b.x, t), lerp(a.y, b.y, t)};
    return ret;
}

int main(void) {
    InitWindow(W, H, "mandelbrot");
    printf("%s", shaderSource);
    RenderTexture target = LoadRenderTexture(W, H);
    Shader shader = LoadShaderFromMemory(0, shaderSource);

    int maxiterLoc = GetShaderLocation(shader, "maxiter");
    int xLoc = GetShaderLocation(shader, "x");
    int yLoc = GetShaderLocation(shader, "y");

    SetTargetFPS(60);

    int maxiter = 256;
    Vector2 x = {X1, X2};
    Vector2 y = {Y1, Y2};

    SetShaderValue(shader, maxiterLoc, &maxiter, SHADER_UNIFORM_INT);
    SetShaderValue(shader, xLoc, &x, SHADER_UNIFORM_VEC2);
    SetShaderValue(shader, yLoc, &y, SHADER_UNIFORM_VEC2);

    float re = 0.0f;
    float im = 0.0f;

    float zoom = 1.5f;
    float moveFactor = 1 / 15.0f;
    float step = 12.0f;
    int showInfo = 1;
    while (!WindowShouldClose()) {
        float dt = GetFrameTime();

        if (IsKeyDown(KEY_A)) re -= zoom * moveFactor;
        if (IsKeyDown(KEY_D)) re += zoom * moveFactor;
        if (IsKeyDown(KEY_S)) im += zoom * moveFactor;
        if (IsKeyDown(KEY_W)) im -= zoom * moveFactor;

        if (IsKeyDown(KEY_E)) zoom /= 1.1f;
        if (IsKeyDown(KEY_Q)) zoom *= 1.1f;

        if (IsKeyDown(KEY_M)) maxiter += 1;
        if (IsKeyDown(KEY_N)) maxiter = maxiter - 2 < 2 ? 2 : maxiter - 2;

        if (IsKeyPressed(KEY_H)) showInfo = !showInfo;

        Vector2 xGoal = {re - zoom, re + zoom};
        Vector2 yGoal = {im - zoom, im + zoom};

        x = lerpVector2(x, xGoal, step * dt);
        y = lerpVector2(y, yGoal, step * dt);

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
