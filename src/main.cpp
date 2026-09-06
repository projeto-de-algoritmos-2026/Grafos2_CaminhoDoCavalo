#include "raylib.h"
#include <vector>
#include "grafo.h"

const int BOARD_SIZE = 8;
const int TILE_SIZE = 80;
const int SCREEN_SIZE = BOARD_SIZE * TILE_SIZE;

bool emAnimacao = false;
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

    while (!WindowShouldClose()) {
        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            int gridX = GetMouseX() / TILE_SIZE;
            int gridY = GetMouseY() / TILE_SIZE;

            if (startX == -1) {
                startX = gridX;
                startY = gridY;
            } else if (targetX == -1) {
                targetX = gridX;
                targetY = gridY;

                rotaFinal = encontrarCaminhoCavalo({startX, startY}, {targetX, targetY}, obstaculos);
                
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

        if (IsMouseButtonPressed(MOUSE_BUTTON_MIDDLE)) {
            int gridX = GetMouseX() / TILE_SIZE;
            int gridY = GetMouseY() / TILE_SIZE;
            Posicao obs = {gridX, gridY};

            auto it = std::find(obstaculos.begin(), obstaculos.end(), obs);
            if (it != obstaculos.end()) {
                obstaculos.erase(it);
                numeroObstaculos--;
            } else if (numeroObstaculos < 10) {
                obstaculos.push_back(obs);
                numeroObstaculos++;
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

        if (targetX != -1) {
            DrawRectangle(targetX * TILE_SIZE, targetY * TILE_SIZE, TILE_SIZE, TILE_SIZE, RED);
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

                Rectangle sourceRec = { spriteWidth * 2.0f, 0.0f, spriteWidth, spriteHeight };
                Rectangle destRec = { (float)(obs.x * TILE_SIZE), (float)(obs.y * TILE_SIZE), (float)TILE_SIZE, (float)TILE_SIZE };
            
                DrawTexturePro(blackPiecesSheet, sourceRec, destRec, { 0.0f, 0.0f }, 0.0f, WHITE);
            }
        }

        Posicao posCavalo = {startX, startY};
        if (!rotaFinal.empty() && startX != -1) {
            posCavalo = rotaFinal[passoAtual];
        }

        if (startX != -1) {
            DrawRectangle(posCavalo.x * TILE_SIZE, posCavalo.y * TILE_SIZE, TILE_SIZE, TILE_SIZE, GREEN);
            
            if (whitePiecesSheet.id > 0) {
                float spriteWidth = (float)whitePiecesSheet.width / 6.0f;
                float spriteHeight = (float)whitePiecesSheet.height;
                Rectangle sourceRec = { spriteWidth * 1.0f, 0.0f, spriteWidth, spriteHeight };

                Rectangle destRec = { (float)(posCavalo.x * TILE_SIZE), (float)(posCavalo.y * TILE_SIZE), (float)TILE_SIZE, (float)TILE_SIZE };

                DrawTexturePro(whitePiecesSheet, sourceRec, destRec, { 0.0f, 0.0f }, 0.0f, WHITE);
        
            } else {
                DrawText("C", posCavalo.x * TILE_SIZE + 30, posCavalo.y * TILE_SIZE + 20, 32, DARKGREEN);
            }
        }

        DrawRectangle(0, SCREEN_SIZE - 40, SCREEN_SIZE, 40, Fade(BLACK, 0.7f));
        DrawText("Esq: Inicio/Destino | Meio: Obstaculo | Dir: Limpar", 15, SCREEN_SIZE - 30, 16, RAYWHITE);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}