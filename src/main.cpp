#include "raylib.h"
#include <vector>
#include "grafo.h"
#include <map>

const int BOARD_SIZE = 8;
const int TILE_SIZE = 80;
const int SCREEN_SIZE = BOARD_SIZE * TILE_SIZE;

bool emAnimacao = false;
bool tabuleiroBloqueado = false;
int numeroObstaculos = 0;
size_t passoAtual = 0;
float tempoAcumulado = 0.0f;
float velocidadeSalto = 0.25f;

int main() {
    InitWindow(SCREEN_SIZE, SCREEN_SIZE, "Caminho do Cavalo - Dijkstra/A*");
    SetTargetFPS(60);

    /* Texturas */
    Texture2D whitePiecesSheet = LoadTexture("../assets/WhitePieces.png");
    Texture2D blackPiecesSheet = LoadTexture("../assets/BlackPieces.png");

    int startX = -1, startY = -1;
    int targetX = -1, targetY = -1;
    std::vector<Posicao> rotaFinal;
    std::vector<Posicao> obstaculos;

    std::map<Posicao, int> spritesObstaculos;

    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && !tabuleiroBloqueado) {
            int gridX = GetMouseX() / TILE_SIZE;
            int gridY = GetMouseY() / TILE_SIZE;

            if (startX == -1) {
                startX = gridX;
                startY = gridY;
            } else if (targetX == -1) {
                targetX = gridX;
                targetY = gridY;

                rotaFinal = encontrarCaminhoCavalo({startX, startY}, {targetX, targetY}, obstaculos);
                tabuleiroBloqueado = true;
                
                if (!rotaFinal.empty()) {
                    emAnimacao = true;
                    passoAtual = 0;
                    tempoAcumulado = 0.0f;
                }
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            startX = -1; startY = -1;
            targetX = -1; targetY = -1;
            rotaFinal.clear();
            obstaculos.clear();
            emAnimacao = false;
            passoAtual = 0;
            numeroObstaculos = 0;
<<<<<<< HEAD
            tabuleiroBloqueado = false;
=======
            spritesObstaculos.clear();
>>>>>>> 1cc86ea (feat: add modification to select ramdom black piece as obstacle)
        }

        if (emAnimacao && !rotaFinal.empty()) {
            tempoAcumulado += GetFrameTime();
            if(tempoAcumulado >= velocidadeSalto) {
                tempoAcumulado = 0.0f;
                passoAtual++;
                if (passoAtual >= rotaFinal.size()) {
                    emAnimacao = false;
                    passoAtual = rotaFinal.size() - 1;
                }
            }
        }

        if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE) && !tabuleiroBloqueado) {
            int gridX = GetMouseX() / TILE_SIZE;
            int gridY = GetMouseY() / TILE_SIZE;
            Posicao obs = {gridX, gridY};

            auto it = std::find(obstaculos.begin(), obstaculos.end(), obs);
            if (it != obstaculos.end()) {
                obstaculos.erase(it);
                spritesObstaculos.erase(obs);
                numeroObstaculos--;
            } else if (numeroObstaculos < 10) {
                obstaculos.push_back(obs);
                spritesObstaculos[obs] = GetRandomValue(0,5);
                numeroObstaculos++;
            }
        }
        if (emAnimacao && !rotaFinal.empty()) {
            if (passoAtual < rotaFinal.size() - 1) {
                tempoAcumulado += GetFrameTime();

                if (tempoAcumulado >= velocidadeSalto) {
                    tempoAcumulado -= velocidadeSalto;
                    passoAtual++;

                    if (passoAtual >= rotaFinal.size() - 1) {
                        emAnimacao = false;
                        passoAtual = rotaFinal.size() - 1;
                        tempoAcumulado = 0.0f;
                    }
                }
            } else {
                emAnimacao = false;
            }
        }
        BeginDrawing();
        ClearBackground(RAYWHITE);


        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                Color tileColor = ((x + y) % 2 == 0) ? RAYWHITE : LIGHTGRAY;
                DrawRectangle(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, tileColor);
                DrawRectangleLines(x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE, TILE_SIZE, DARKGRAY);
            }
        }


        for (const auto& p: rotaFinal) {
            if ((p.x != startX || p.y != startY) && (p.x != targetX || p.y != targetY)) {
                DrawRectangle(p.x * TILE_SIZE, p.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, SKYBLUE);
            }
        }


        if (targetX != -1) {
            DrawRectangle(targetX * TILE_SIZE, targetY * TILE_SIZE, TILE_SIZE, TILE_SIZE, RED);
        }


        for (const auto& obs : obstaculos) {
            DrawRectangle(obs.x * TILE_SIZE, obs.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, DARKGRAY);

            if (blackPiecesSheet.id > 0) {
                float spriteWidth = (float)blackPiecesSheet.width / 6.0f;
                float spriteHeight = (float)blackPiecesSheet.height;
                int spriteIndex = spritesObstaculos[obs];
                Rectangle sourceRec = { spriteWidth * (float)spriteIndex, 0.0f, spriteWidth, spriteHeight };
                Rectangle destRec = { (float)(obs.x * TILE_SIZE), (float)(obs.y * TILE_SIZE), (float)TILE_SIZE, (float)TILE_SIZE };

                DrawTexturePro(blackPiecesSheet, sourceRec, destRec, { 0.0f, 0.0f }, 0.0f, WHITE);
            }
        }


        float drawX = startX * TILE_SIZE;
        float drawY = startY * TILE_SIZE;

        if (!rotaFinal.empty() && startX != -1) {
            if (emAnimacao && passoAtual < rotaFinal.size() - 1) {

                float t = tempoAcumulado / velocidadeSalto;
                if (t > 1.0f) t = 1.0f; // Clamp de segurança

                Posicao posAtual = rotaFinal[passoAtual];
                Posicao posProx  = rotaFinal[passoAtual + 1];


                drawX = (posAtual.x + (posProx.x - posAtual.x) * t) * TILE_SIZE;
                drawY = (posAtual.y + (posProx.y - posAtual.y) * t) * TILE_SIZE;
            } else {

                drawX = rotaFinal[passoAtual].x * TILE_SIZE;
                drawY = rotaFinal[passoAtual].y * TILE_SIZE;
            }
        }


        if (startX != -1) {
            DrawRectangle(drawX, drawY, TILE_SIZE, TILE_SIZE, GREEN);

            if (whitePiecesSheet.id > 0) {
                float spriteWidth = (float)whitePiecesSheet.width / 6.0f;
                float spriteHeight = (float)whitePiecesSheet.height;
                Rectangle sourceRec = { spriteWidth * 1.0f, 0.0f, spriteWidth, spriteHeight };

                Rectangle destRec = { drawX, drawY, (float)TILE_SIZE, (float)TILE_SIZE };

                DrawTexturePro(whitePiecesSheet, sourceRec, destRec, { 0.0f, 0.0f }, 0.0f, WHITE);

            } else {
                DrawText("C", drawX + 30, drawY + 20, 32, DARKGREEN);
            }
        }


        DrawRectangle(0, SCREEN_SIZE - 40, SCREEN_SIZE, 40, Fade(BLACK, 0.7f));
        DrawText("Esq: Inicio/Destino | Meio: Obstaculo | Dir: Limpar", 15, SCREEN_SIZE - 30, 16, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}