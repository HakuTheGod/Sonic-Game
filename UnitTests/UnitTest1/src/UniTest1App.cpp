#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

// Tile and Window Constants
const int WINDOW_WIDTH = 272;
const int WINDOW_HEIGHT = 780;
const int TILE_WIDTH = 16;
const int TILE_HEIGHT = 16;
const int SCALE = 4;

const int SCALED_TILE_WIDTH = TILE_WIDTH * SCALE;
const int SCALED_TILE_HEIGHT = TILE_HEIGHT * SCALE;

// Map data structure
std::vector<std::vector<int>> mapData;
int mapWidthTiles = 0;
int mapHeightTiles = 0;

// View Window representing the camera view
SDL_Rect viewWindow = { 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT };

// Scroll Multiplier state
float scrollMultipliers[] = { 0.5f, 1.0f, 1.5f, 2.0f };
int multiplierIndex = 1; // Default to 1.0f

// Mouse dragging state
bool isMouseDown = false;
int lastMouseX = 0;
int lastMouseY = 0;

// Tiled CSV Map Export
bool ReadTextMap(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open map file: " << filename << std::endl;
        return false;
    }

    std::string line;
    mapData.clear();
    mapWidthTiles = 0;
    mapHeightTiles = 0;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string value;
        std::vector<int> row;

        while (std::getline(ss, value, ',')) {
            if (!value.empty()) {
                row.push_back(std::stoi(value));
            }
        }

        if (!row.empty()) {
            if (mapWidthTiles == 0) {
                mapWidthTiles = static_cast<int>(row.size());
            }
            mapData.push_back(row);
            mapHeightTiles++;
        }
    }

    file.close();
    std::cout << "Loaded map successfully! Dimensions: " << mapWidthTiles << " x " << mapHeightTiles << " tiles." << std::endl;
    return true;
}

// Scrolling Constraints & Logic
int GetMapPixelWidth() {
    return mapWidthTiles * SCALED_TILE_WIDTH;
}

int GetMapPixelHeight() {
    return mapHeightTiles * SCALED_TILE_HEIGHT;
}

void FilterScroll(int* dx, int* dy) {

    int maxW = GetMapPixelWidth();
    int maxH = GetMapPixelHeight();

    // Horizontal scrolling
    if (maxW <= viewWindow.w) {
        *dx = 0;
        viewWindow.x = 0;
    }
    else {
        if (viewWindow.x + *dx < 0)
            *dx = -viewWindow.x;

        else if (viewWindow.x + viewWindow.w + *dx > maxW)
            *dx = maxW - (viewWindow.x + viewWindow.w);
    }

    // Vertical scrolling
    if (maxH <= viewWindow.h) {
        *dy = 0;
        viewWindow.y = 0;
    }
    else {
        if (viewWindow.y + *dy < 0)
            *dy = -viewWindow.y;

        else if (viewWindow.y + viewWindow.h + *dy > maxH)
            *dy = maxH - (viewWindow.y + viewWindow.h);
    }
}

void Scroll(int dx, int dy) {
    FilterScroll(&dx, &dy);
    viewWindow.x += dx;
    viewWindow.y += dy;
}

// Render Function for Tile Map
void RenderMap(SDL_Renderer* renderer, SDL_Texture* tilesetTexture) {
    if (!tilesetTexture || mapData.empty()) return;

    int startCol = viewWindow.x / TILE_WIDTH;
    int startRow = viewWindow.y / TILE_HEIGHT;
    int endCol = (viewWindow.x + viewWindow.w - 1) / TILE_WIDTH;
    int endRow = (viewWindow.y + viewWindow.h - 1) / TILE_HEIGHT;

    // Clamp end indices to map boundaries
    endCol = std::min(endCol, mapWidthTiles - 1);
    endRow = std::min(endRow, mapHeightTiles - 1);

    int offsetX = viewWindow.x % TILE_WIDTH;
    int offsetY = viewWindow.y % TILE_HEIGHT;

    // Get tileset dimensions to calculate columns
    int texW, texH;
    SDL_QueryTexture(tilesetTexture, NULL, NULL, &texW, &texH);
    int tilesetCols = texW / TILE_WIDTH;

    for (int row = startRow; row <= endRow; ++row) {
        for (int col = startCol; col <= endCol; ++col) {
            int tileIndex = mapData[row][col];
            if (tileIndex < 0) continue; // Skip empty/background tiles

            // Calculate source rect in tileset image
            int srcX = (tileIndex % tilesetCols) * TILE_WIDTH;
            int srcY = (tileIndex / tilesetCols) * TILE_HEIGHT;
            SDL_Rect srcRect = { srcX, srcY, TILE_WIDTH, TILE_HEIGHT };

            // Calculate destination rect on screen
            int destX = (col - startCol) * TILE_WIDTH - offsetX;
            int destY = (row - startRow) * TILE_HEIGHT - offsetY;
            SDL_Rect destRect = { destX * SCALE, destY * SCALE, TILE_WIDTH * SCALE, TILE_HEIGHT * SCALE };

            SDL_RenderCopy(renderer, tilesetTexture, &srcRect, &destRect);
        }
    }
}

int main(int argc, char* argv[]) {
    // Initialize Graphics in Window Mode (640x480)
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Initialization failed: " << SDL_GetError() << std::endl;
        return -1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image Initialization failed: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return -1;
    }

    SDL_Window* window = SDL_CreateWindow("Standalone Step 1 - Sonic Project",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        IMG_Quit();
        SDL_Quit();
        return -1;
    }

   if (!ReadTextMap("UnitTest1Media/ground.csv")) {
        std::cerr << "Failed to load text map!" << std::endl;
    }

    SDL_Surface* tempSurface = IMG_Load("UnitTest1Media/gentle_forest_v01.png");
    if (!tempSurface) {
        std::cerr << "Failed to load tileset image: " << IMG_GetError() << std::endl;
    }
    SDL_Texture* tilesetTexture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    bool isRunning = true;
    SDL_Event event;

    // Main Game Loop
    while (isRunning) {
        // Input Logic & Event Handling
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_QUIT:
                isRunning = false;
                break;

            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    isMouseDown = true;
                    lastMouseX = event.button.x;
                    lastMouseY = event.button.y;
                }
                break;

            case SDL_MOUSEBUTTONUP:
                if (event.button.button == SDL_BUTTON_LEFT) {
                    isMouseDown = false;
                }
                break;

            case SDL_MOUSEMOTION:
                if (isMouseDown) {
                    // Mouse scrolling offset calculation multiplied by factor
                    int dx = (lastMouseX - event.button.x) * scrollMultipliers[multiplierIndex];
                    int dy = (lastMouseY - event.button.y) * scrollMultipliers[multiplierIndex];
                    Scroll(dx, dy);
                    lastMouseX = event.button.x;
                    lastMouseY = event.button.y;
                }
                break;

            case SDL_KEYUP:
                // Multiplier adjustments on key release (+ / - / 0)
                if (event.key.keysym.sym == SDLK_PLUS || event.key.keysym.sym == SDLK_KP_PLUS || event.key.keysym.sym == SDLK_EQUALS) {
                    multiplierIndex = (multiplierIndex + 1) % 4; // Loops 0.5 -> 1.0 -> 1.5 -> 2.0 -> 0.5
                    std::cout << "Scroll Multiplier: " << scrollMultipliers[multiplierIndex] << std::endl;
                }
                else if (event.key.keysym.sym == SDLK_MINUS || event.key.keysym.sym == SDLK_KP_MINUS) {
                    multiplierIndex = (multiplierIndex - 1 + 4) % 4; // Loops backward
                    std::cout << "Scroll Multiplier: " << scrollMultipliers[multiplierIndex] << std::endl;
                }
                else if (event.key.keysym.sym == SDLK_0 || event.key.keysym.sym == SDLK_KP_0) {
                    multiplierIndex = 1; // Resets to default 1.0f (or set index 0 for 0.5f as specified)
                    std::cout << "Scroll Multiplier reset to default: " << scrollMultipliers[multiplierIndex] << std::endl;
                }
                break;
            }
        }

        // Continuous Keyboard Arrow / Home / End Polling
        const Uint8* keystate = SDL_GetKeyboardState(NULL);
        int baseSpeed = 4;
        int moveDx = 0;
        int moveDy = 0;

        if (keystate[SDL_SCANCODE_UP])    moveDy -= static_cast<int>(baseSpeed * scrollMultipliers[multiplierIndex]);
        if (keystate[SDL_SCANCODE_DOWN])  moveDy += static_cast<int>(baseSpeed * scrollMultipliers[multiplierIndex]);
        if (keystate[SDL_SCANCODE_LEFT])  moveDx -= static_cast<int>(baseSpeed * scrollMultipliers[multiplierIndex]);
        if (keystate[SDL_SCANCODE_RIGHT]) moveDx += static_cast<int>(baseSpeed * scrollMultipliers[multiplierIndex]);

        if (moveDx != 0 || moveDy != 0) {
            Scroll(moveDx, moveDy);
        }

        if (keystate[SDL_SCANCODE_HOME]) {
            viewWindow.x = 0;
            viewWindow.y = 0;
        }
        if (keystate[SDL_SCANCODE_END]) {
            viewWindow.x = GetMapPixelWidth() - viewWindow.w;
            viewWindow.y = GetMapPixelHeight() - viewWindow.h;
        }

        // Render Frame
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        RenderMap(renderer, tilesetTexture);

        SDL_RenderPresent(renderer);
    }

    // Cleanup
    SDL_DestroyTexture(tilesetTexture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}
