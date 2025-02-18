#include <SDL2/SDL.h>
#include <vector>

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
const int TANK_SIZE = 40;
const int BULLET_SIZE = 10;

struct Tank {
    SDL_Rect rect;
    int speed;
    SDL_Color color;
    int direction; // 0:上 1:右 2:下 3:左
    bool active;
};

std::vector<Tank> enemyTanks;

struct Bullet {
    SDL_Rect rect;
    int speed;
    int direction;
    bool active;
};

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
Tank playerTank;
std::vector<Bullet> bullets;
bool gameRunning = true;

void initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    window = SDL_CreateWindow("Tank Battle", 
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // 初始化玩家坦克
    playerTank.rect = {SCREEN_WIDTH/2 - TANK_SIZE/2, 
                      SCREEN_HEIGHT - TANK_SIZE - 10, 
                      TANK_SIZE, TANK_SIZE};
    playerTank.speed = 5;
    playerTank.color = {0, 255, 0, 255};
    playerTank.direction = 0;
}

void handleInput() {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            gameRunning = false;
        }
    }

    const Uint8* keystates = SDL_GetKeyboardState(NULL);
    
    if (keystates[SDL_SCANCODE_W]) playerTank.direction = 0;
    if (keystates[SDL_SCANCODE_D]) playerTank.direction = 1;
    if (keystates[SDL_SCANCODE_S]) playerTank.direction = 2;
    if (keystates[SDL_SCANCODE_A]) playerTank.direction = 3;

    if (keystates[SDL_SCANCODE_SPACE]) {
        Bullet newBullet;
        newBullet.rect = {playerTank.rect.x + TANK_SIZE/2 - BULLET_SIZE/2,
                         playerTank.rect.y + TANK_SIZE/2 - BULLET_SIZE/2,
                         BULLET_SIZE, BULLET_SIZE};
        newBullet.speed = 8;
        newBullet.direction = playerTank.direction;
        newBullet.active = true;
        bullets.push_back(newBullet);
    }
}

void update() {
    // 更新坦克位置
    switch(playerTank.direction) {
        case 0: if(playerTank.rect.y > 0) playerTank.rect.y -= playerTank.speed; break;
        case 1: if(playerTank.rect.x < SCREEN_WIDTH - TANK_SIZE) playerTank.rect.x += playerTank.speed; break;
        case 2: if(playerTank.rect.y < SCREEN_HEIGHT - TANK_SIZE) playerTank.rect.y += playerTank.speed; break;
        case 3: if(playerTank.rect.x > 0) playerTank.rect.x -= playerTank.speed; break;
    }

    // 更新子弹位置
    for (auto& bullet : bullets) {
        if (bullet.active) {
            switch(bullet.direction) {
                case 0: bullet.rect.y -= bullet.speed; break;
                case 1: bullet.rect.x += bullet.speed; break;
                case 2: bullet.rect.y += bullet.speed; break;
                case 3: bullet.rect.x -= bullet.speed; break;
            }
            
            // 边界检测
            if (bullet.rect.x < 0 || bullet.rect.x > SCREEN_WIDTH ||
                bullet.rect.y < 0 || bullet.rect.y > SCREEN_HEIGHT) {
                bullet.active = false;
            }
        }
    }
}

void render() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // 绘制坦克
    SDL_SetRenderDrawColor(renderer, 
        playerTank.color.r, playerTank.color.g, playerTank.color.b, 255);
    SDL_RenderFillRect(renderer, &playerTank.rect);

    // 绘制子弹
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    for (const auto& bullet : bullets) {
        if (bullet.active) {
            SDL_RenderFillRect(renderer, &bullet.rect);
        }
    }

    SDL_RenderPresent(renderer);
}

void cleanUp() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    initialize();

    while (gameRunning) {
        handleInput();
        update();
        render();
        SDL_Delay(16); // ~60 FPS
    }

    cleanUp();
    return 0;
}
