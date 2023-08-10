#include "raylib.h"

#define LARGURA 800
#define ALTURA 800
#define PX 0
#define PY 1

#define LADO_ISAAC 20
#define PROJETIL_ISAAC 5

int disparo(int direcao[], int projetil[])
{
    if (direcao[PX] == 1)
    {
        do
        {
            projetil[PX] = projetil[PX] + PROJETIL_ISAAC;
        }
        while(projetil[PX] < LARGURA);
    }
    if (direcao[PX] == -1)
    {
        do
        {
            projetil[PX] = projetil[PX] - PROJETIL_ISAAC;
        }
        while(projetil[PX] > 0);
    }
    if (direcao[PY] == 1)
    {
        do
        {
            projetil[PY] = projetil[PY] + PROJETIL_ISAAC;
        }
        while(projetil[PY] < ALTURA);
    }
    if (direcao[PY] == -1)
    {
        do
        {
            projetil[PY] = projetil[PY] - PROJETIL_ISAAC;
        }
        while(projetil[PY] > 0);
    }
    return projetil;
}

int projeteis(int direcao[], int posicao[], int projetil[])
{
    projetil[PX], projetil[PY]; // Declaração correta do array de projetil

    projetil[PX] = posicao[PX];
    projetil[PY] = posicao[PY];

    if (IsKeyPressed(KEY_UP))
    {
        direcao[PY] = -1;
    }
    if (IsKeyPressed(KEY_DOWN))
    {
        direcao[PY] = 1;
    }
    if (IsKeyPressed(KEY_RIGHT))
    {
        direcao[PX] = 1;
    }
    if (IsKeyPressed(KEY_LEFT))
    {
        direcao[PX] = -1;
    }
    return direcao;
}

int move(int intent[], int posicao[])
{
    if (intent[PX] == 1 && posicao[PX] < (LARGURA - LADO_ISAAC))
    {
        posicao[PX] = posicao[PX] + LADO_ISAAC;
    }
    if (intent[PX] == -1 && posicao[PX] > 0)
    {
        posicao[PX] = posicao[PX] - LADO_ISAAC;
    }
    if (intent[PY] == 1 && posicao[PY] < (ALTURA - LADO_ISAAC))
    {
        posicao[PY] = posicao[PY] + LADO_ISAAC;
    }
    if (intent[PY] == -1 && posicao[PY] > 0)
    {
        posicao[PY] = posicao[PY] - LADO_ISAAC;
    }
    return posicao;
}

int DeveMover(int intent[])
{
    intent[PX] = 0;
    intent[PY] = 0;

    if (IsKeyDown(KEY_D))
    {
        intent[PX] = 1;
    }
    if (IsKeyDown(KEY_A))
    {
        intent[PX] = -1;
    }
    if (IsKeyDown(KEY_W))
    {
        intent[PY] = -1;
    }
    if (IsKeyDown(KEY_S))
    {
        intent[PY] = 1;
    }
    return intent;
}

int main()
{
    int posicao[2], intent[2], direcao[2], projetil[2];
    int jogoON = 1;

    posicao[PX] = LARGURA / 2; // Inicia no centro da tela em relação a X
    posicao[PY] = ALTURA / 2;  // Inicia no centro da tela em relação a Y

    InitWindow(LARGURA, ALTURA, "Quadrado");
    SetTargetFPS(60);

    while (!WindowShouldClose() && jogoON)
    {
        DeveMover(intent);

        move(intent, posicao);
        intent[PX] = 0;
        intent[PY] = 0;

        projeteis(direcao, posicao, projetil);
        disparo(direcao, projetil);

        //if (posicao[PX] < 0 || posicao[PX] >= LARGURA || posicao[PY] < 0 || posicao[PY] >= ALTURA)
        //jogoON = 0;

        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangle(posicao[PX], posicao[PY], LADO_ISAAC, LADO_ISAAC, GREEN);
        DrawRectangle(projetil[PX], projetil[PY], PROJETIL_ISAAC, PROJETIL_ISAAC, BLUE);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
