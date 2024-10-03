#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define NUM_SYMBOLS 4
#define NUM_REELS 3
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

// you can choose any symbols and add png files
const char *symbols[NUM_SYMBOLS] = {ASSETS_PATH "cherries.png", ASSETS_PATH "lemon.png", ASSETS_PATH "melon.png", ASSETS_PATH "Lucky7_rainbow.png"}; 
const int payouts[NUM_SYMBOLS] = {2, 5, 10, 20};

typedef struct {
    char name[50];
    int balance;
    int current_bet;
} User;

typedef struct {
    int reels[NUM_REELS];
    int spin_timer;
} SlotMachine;

typedef enum {
    GAME_STATE_SPINNING,
    GAME_STATE_WAITING,
    GAME_STATE_BET_CHANGE,
    GAME_STATE_EXIT
} GameState;

void initialize_user(User *user) {
    printf("Enter your name: ");
    scanf("%49s", user->name);
    printf("Enter your deposit money: ");
    scanf("%d", &user->balance);
    printf("Enter your bet: ");
    scanf("%d", &user->current_bet);
}

void spin_slot_machine(SlotMachine *machine) {
    for (int i = 0; i < NUM_REELS; i++) {
        machine->reels[i] = rand() % NUM_SYMBOLS;
    }
}

void display_reels(SlotMachine *machine, Texture2D textures[NUM_SYMBOLS]) {
    for (int i = 0; i < NUM_REELS; i++) {
        DrawTexture(textures[machine->reels[i]], 150 + i * 200, 200, WHITE);
    }
}

int calculate_payout(SlotMachine *machine, int bet) {
    int payout = 0;

    if (machine->reels[0] == machine->reels[1] && machine->reels[1] == machine->reels[2]) {
        payout = bet * payouts[machine->reels[0]];
    } else if (machine->reels[0] == machine->reels[1]) {
        payout = bet * payouts[machine->reels[0]] / 2;
    }

    return payout;
}

void update_balance(User *user, int payout) {
    user->balance += payout;
}

int main() {
    srand(time(NULL));

    User user;
    // initialize_user(&user);
    user.balance = 100000;
    user.current_bet = 1;

    SlotMachine machine = { .spin_timer = 0 };

    // Initialize Raylib
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Slot Machine");
    InitAudioDevice();  // Initialize audio device

    // Load Textures
    Texture2D textures[NUM_SYMBOLS];
    textures[0] = LoadTexture(ASSETS_PATH "cherries.png");
    textures[1] = LoadTexture(ASSETS_PATH "lemon.png");
    textures[2] = LoadTexture(ASSETS_PATH "melon.png");
    textures[3] = LoadTexture(ASSETS_PATH "Lucky7_rainbow.png");

    // Load Background Music
    Music bgMusic = LoadMusicStream(ASSETS_PATH "bonus.mp3");
    PlayMusicStream(bgMusic);  // Start playing music

    int lastPayout = -1;
    bool showBetChange = false;
    char betInput[10] = "";
    int betInputIndex = 0;
    GameState gameState = GAME_STATE_WAITING;

    while (!WindowShouldClose() && gameState != GAME_STATE_EXIT) {
        UpdateMusicStream(bgMusic);  // Update music stream every frame

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Draw the slot name at the top of the window
        DrawText("Pokere.bet", (SCREEN_WIDTH - MeasureText("Pokere.bet", 30)) / 2, 20, 30, BLACK);

        switch (gameState) {
            case GAME_STATE_SPINNING:
                machine.spin_timer++;
                for (int i = 0; i < NUM_REELS; i++) {
                    if (machine.spin_timer > i * 20) {
                        machine.reels[i] = rand() % NUM_SYMBOLS;
                    }
                }
                if (machine.spin_timer > 60) {
                    gameState = GAME_STATE_WAITING;
                    machine.spin_timer = 0;
                    int payout = calculate_payout(&machine, user.current_bet);
                    lastPayout = payout;
                    if (payout <= 0) {
                        payout = -user.current_bet;
                    }
                    update_balance(&user, payout);
                }
                break;

            case GAME_STATE_WAITING:
                display_reels(&machine, textures);

                if (lastPayout > 0) {
                    DrawText(TextFormat("You won %d!", lastPayout), 350, 100, 20, GREEN);
                } else if (lastPayout == 0) {
                    DrawText("You lost your bet.", 350, 100, 20, RED);
                }

                DrawText(TextFormat("Balance: %d", user.balance), 350, 150, 20, BLACK);
                DrawText("Press [ENTER] to spin, [B] to change bet, [ESC] to exit", 150, 500, 20, DARKGRAY);

                if (IsKeyPressed(KEY_ENTER)) {
                    if (user.current_bet <= user.balance) {
                        gameState = GAME_STATE_SPINNING;
                        lastPayout = -1;
                        spin_slot_machine(&machine);
                    } else {
                        DrawText("Bet exceeds balance! Change bet.", 300, 350, 20, RED);
                    }
                } else if (IsKeyPressed(KEY_B)) {
                    showBetChange = true;
                    gameState = GAME_STATE_BET_CHANGE;
                } else if (IsKeyPressed(KEY_ESCAPE)) {
                    gameState = GAME_STATE_EXIT;
                }
                break;

            case GAME_STATE_BET_CHANGE:
                DrawRectangle(200, 200, 400, 200, GRAY);
                DrawText("Enter new bet: ", 220, 240, 20, BLACK);
                DrawText(betInput, 220, 280, 20, BLACK);

                int key = GetKeyPressed();
                if (key >= '0' && key <= '9' && betInputIndex < sizeof(betInput) - 1) {
                    betInput[betInputIndex++] = (char)key;
                    betInput[betInputIndex] = '\0';
                }

                if (key == KEY_BACKSPACE && betInputIndex > 0) {
                    betInput[--betInputIndex] = '\0';
                }

                if (IsKeyPressed(KEY_ENTER)) {
                    int newBet = atoi(betInput);
                    if (newBet > user.balance) {
                        DrawText("Not enough balance!", 320, 320, 20, RED);
                    } else {
                        user.current_bet = newBet;
                        gameState = GAME_STATE_WAITING;
                    }
                    betInput[0] = '\0';
                    betInputIndex = 0;
                } else if (IsKeyPressed(KEY_ESCAPE)) {
                    gameState = GAME_STATE_WAITING;
                }
                break;

            case GAME_STATE_EXIT:
                break;
        }

        EndDrawing();
    }

    // Unload textures
    for (int i = 0; i < NUM_SYMBOLS; i++) {
        UnloadTexture(textures[i]);
    }

    // Unload and stop music
    StopMusicStream(bgMusic);
    UnloadMusicStream(bgMusic);
    CloseAudioDevice();  // Close audio device

    CloseWindow();

    printf("Final balance: %d\n", user.balance);
    return 0;
}
