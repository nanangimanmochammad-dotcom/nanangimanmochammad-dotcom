/*
 * ui_manager.c
 *
 *  Created on: Dec 12, 2025
 *      Author: admin
 */

#include "ui_manager.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h> // for rand()

// ==========================================
// Global variables and state management
// ==========================================

static UI_State_t currentState = STATE_SPLASH; 
static uint8_t cgram_loaded_type = 0; 

// Splash Rolling variable
static uint8_t splashFrame = 0;
static uint32_t lastSplashTick = 0;
#define SPLASH_FRAME_MS 400

// Game variable
typedef enum { GAME_WAIT, GAME_PLAY, GAME_OVER } GameSubState_t;
static GameSubState_t gameSubState = GAME_WAIT;
static uint32_t gameScore = 0;
static uint32_t lastFrameTick = 0;
static uint8_t playerY = 1;
static uint8_t isJumping = 0;
static uint8_t jumpFrame = 0;
static uint8_t runAnimState = 0;
static int8_t obstacleX = 16;
static uint32_t frameDelay_ms = 120;
#define JUMP_DURATION 4

// Animation Variable 
typedef enum {
    ANIM_RUN_RIGHT,
    ANIM_SLIP_RIGHT,
    ANIM_RUN_LEFT,
    ANIM_SLIP_LEFT,
    ANIM_DONE // The animation has ended
} AnimSubState_t;

static AnimSubState_t animSubState = ANIM_RUN_RIGHT;
static uint32_t lastAnimTick = 0;
static int animLoopIdx = 0;   // Corresponding to the variable 'a' in Arduino
static int animFrameStep = 0; // Corresponding to the step (0-3 or 0-2) within each action
#define ANIM_SPEED_MS 300     // Corresponding to Arduino int x = 300;

// ==========================================
// UI Resource Definition (Font)
// ==========================================

// --- Art Mode resources  ---
const uint8_t peace_L[8] = {0x01, 0x07, 0x09, 0x11, 0x15, 0x0B, 0x05, 0x03};
const uint8_t peace_R[8] = {0x10, 0x1C, 0x12, 0x11, 0x15, 0x1A, 0x14, 0x18};
const uint8_t love_L[8] = {0x06, 0x0F, 0x1F, 0x1F, 0x0F, 0x07, 0x03, 0x01};
const uint8_t love_R[8] = {0x0C, 0x1E, 0x1F, 0x1F, 0x1E, 0x1C, 0x18, 0x10};
const uint8_t code_L[8] = {0x02, 0x06, 0x0C, 0x18, 0x0C, 0x06, 0x02, 0x00};
const uint8_t code_R[8] = {0x08, 0x0C, 0x06, 0x03, 0x06, 0x0C, 0x08, 0x00};

// --- Game Mode resources  ---
const uint8_t dino_run1[8] = {0x0C, 0x0C, 0x0E, 0x0E, 0x1F, 0x0E, 0x0A, 0x0A};
const uint8_t dino_run2[8] = {0x0C, 0x0C, 0x0E, 0x0E, 0x1F, 0x0E, 0x12, 0x12};
const uint8_t dino_jump[8] = {0x0C, 0x0C, 0x0E, 0x0E, 0x1F, 0x1F, 0x08, 0x04};
const uint8_t obstacle[8] = {0x00, 0x04, 0x05, 0x15, 0x1F, 0x04, 0x04, 0x04};
const uint8_t ground[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};

// --- Animation mode resources  ---

// standing man (stepA)
const uint8_t stepA1[8] = {0b00011,0b00111,0b00111,0b00111,0b00111,0b00001,0b00011,0b00111};
const uint8_t stepA2[8] = {0b00001,0b00001,0b00011,0b00011,0b00011,0b00001,0b00000,0b00001};
const uint8_t stepA3[8] = {0b11111,0b11111,0b11111,0b11111,0b11111,0b11011,0b10001,0b10001};
const uint8_t stepA4[8] = {0b11111,0b11111,0b11101,0b11101,0b11111,0b11111,0b11111,0b11111};
const uint8_t stepA5[8] = {0b11000,0b11100,0b11100,0b11100,0b11100,0b10000,0b11000,0b11100};
const uint8_t stepA6[8] = {0b00000,0b10000,0b11000,0b00000,0b11000,0b11100,0b11000,0b10000};

// running man right (stepB, C, D)
// step 1 of 3
const uint8_t stepB1[8] = {0b00011,0b00011,0b00011,0b00011,0b00001,0b00000,0b00000,0b00000};
const uint8_t stepB2[8] = {0b00000,0b00001,0b00001,0b00011,0b00011,0b00011,0b00000,0b00001};
const uint8_t stepB3[8] = {0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11110};
const uint8_t stepB4[8] = {0b11111,0b11111,0b11101,0b11101,0b11111,0b11111,0b11111,0b11111};
const uint8_t stepB5[8] = {0b00000,0b00000,0b10000,0b10000,0b10000,0b00000,0b10000,0b00000};
const uint8_t stepB6[8] = {0b00000,0b11000,0b00000,0b11000,0b11100,0b11000,0b10000,0b00000};
// step 2 of 3
const uint8_t stepC1[8] = {0b11111,0b11111,0b11101,0b00011,0b00111,0b01111,0b01111,0b00111};
const uint8_t stepC2[8] = {0b00000,0b00001,0b00001,0b00011,0b00011,0b00011,0b00000,0b00111};
const uint8_t stepC3[8] = {0b11111,0b11111,0b11111,0b11111,0b11111,0b10011,0b00000,0b00000};
const uint8_t stepC4[8] = {0b11111,0b11111,0b11101,0b11101,0b11111,0b11111,0b11111,0b11111};
const uint8_t stepC5[8] = {0b11110,0b11110,0b01100,0b11100,0b11100,0b11100,0b00000,0b00000};
const uint8_t stepC6[8] = {0b00000,0b11000,0b00000,0b11000,0b11100,0b11000,0b10000,0b00000};
// step 3 of 3
const uint8_t stepD1[8] = {0b00001,0b00011,0b00111,0b00111,0b00111,0b01111,0b01100,0b00000};
const uint8_t stepD2[8] = {0b00000,0b00000,0b00001,0b00001,0b00011,0b00011,0b00011,0b00000};
const uint8_t stepD3[8] = {0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11110,0b11111};
const uint8_t stepD4[8] = {0b00000,0b11111,0b11111,0b11101,0b11101,0b11111,0b11111,0b11111};
const uint8_t stepD5[8] = {0b10000,0b11000,0b10000,0b00000,0b00000,0b00000,0b00000,0b00000};
const uint8_t stepD6[8] = {0b00000,0b00000,0b11000,0b00000,0b11000,0b11100,0b11000,0b10000};

// man slipping right (slipA, B, C)
// step 1 of 3
const uint8_t slipA1[8] = {0b00111,0b00111,0b00011,0b00011,0b00001,0b01111,0b01111,0b00111};
const uint8_t slipA2[8] = {0b00000,0b00011,0b00111,0b01111,0b01111,0b00111,0b00011,0b00111};
const uint8_t slipA3[8] = {0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11000,0b10000};
const uint8_t slipA4[8] = {0b11111,0b11111,0b11101,0b11101,0b11111,0b11111,0b11111,0b11111};
const uint8_t slipA5[8] = {0b11000,0b11000,0b10000,0b00000,0b00000,0b00000,0b00000,0b00000};
const uint8_t slipA6[8] = {0b00000,0b11000,0b00000,0b11100,0b11100,0b11000,0b10000,0b11000};
// step 2 of 3
const uint8_t slipB1[8] = {0b00011,0b00011,0b00001,0b00001,0b00000,0b00111,0b00111,0b00011};
const uint8_t slipB2[8] = {0b00000,0b00001,0b00011,0b00111,0b00111,0b00011,0b00001,0b00011};
const uint8_t slipB3[8] = {0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11100,0b11000};
const uint8_t slipB4[8] = {0b11111,0b11111,0b11110,0b11110,0b11111,0b11111,0b11111,0b11111};
const uint8_t slipB5[8] = {0b11100,0b11100,0b11000,0b10000,0b10000,0b00000,0b00000,0b00000};
const uint8_t slipB6[8] = {0b00000,0b11000,0b10000,0b11100,0b11110,0b11100,0b11000,0b11100};
// step 3 of 3
const uint8_t slipC1[8] = {0b00001,0b00001,0b00000,0b00000,0b00000,0b00011,0b00011,0b00001};
const uint8_t slipC2[8] = {0b00000,0b00000,0b00001,0b00011,0b00011,0b00001,0b00000,0b00001};
const uint8_t slipC3[8] = {0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11110,0b11100};
const uint8_t slipC4[8] = {0b01111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111};
const uint8_t slipC5[8] = {0b11110,0b11110,0b11100,0b11000,0b11000,0b10000,0b00000,0b00000};
const uint8_t slipC6[8] = {0b10000,0b11100,0b11000,0b11110,0b11111,0b11110,0b11100,0b11110};

// running man left (stepE, F, G)
// step 1 of 3
const uint8_t stepE1[8] = {0b00000,0b00001,0b00001,0b00001,0b00000,0b00000,0b00001,0b00000};
const uint8_t stepE2[8] = {0b00000,0b00011,0b00000,0b00011,0b00111,0b00011,0b00001,0b00000};
const uint8_t stepE3[8] = {0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b01111};
const uint8_t stepE4[8] = {0b11111,0b11111,0b10111,0b10111,0b11111,0b11111,0b11111,0b11111};
const uint8_t stepE5[8] = {0b11000,0b11000,0b11000,0b11000,0b10000,0b00000,0b00000,0b00000};
const uint8_t stepE6[8] = {0b00000,0b10000,0b10000,0b11000,0b11000,0b11000,0b10000,0b10000};
// step 2 of 3
const uint8_t stepF1[8] = {0b01111,0b01111,0b01100,0b00111,0b00111,0b00111,0b00000,0b00000};
const uint8_t stepF2[8] = {0b00000,0b00011,0b00000,0b00011,0b00111,0b00011,0b00001,0b00000};
const uint8_t stepF3[8] = {0b11111,0b11111,0b11111,0b11111,0b11111,0b11001,0b00000,0b00000};
const uint8_t stepF4[8] = {0b11111,0b11111,0b10111,0b10111,0b11111,0b11111,0b11111,0b11111};
const uint8_t stepF5[8] = {0b11111,0b11111,0b10111,0b11000,0b11100,0b11110,0b11110,0b11100};
const uint8_t stepF6[8] = {0b00000,0b10000,0b10000,0b11000,0b11000,0b11000,0b00000,0b11100};
// step 3 of 3
const uint8_t stepG1[8] = {0b00001,0b00011,0b00001,0b00000,0b00000,0b00000,0b00000,0b00000};
const uint8_t stepG2[8] = {0b00000,0b00000,0b00011,0b00000,0b00011,0b00111,0b00011,0b00001};
const uint8_t stepG3[8] = {0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b01111,0b11111};
const uint8_t stepG4[8] = {0b00000,0b11111,0b11111,0b10111,0b10111,0b11111,0b11111,0b11111};
const uint8_t stepG5[8] = {0b10000,0b11000,0b11100,0b11100,0b11100,0b11110,0b00110,0b00000};
const uint8_t stepG6[8] = {0b00000,0b00000,0b10000,0b10000,0b11000,0b11000,0b11000,0b00000};

// man slipping left (slipD, E, F)
// step 1 of 3
const uint8_t slipD1[8] = {0b00011,0b00011,0b00001,0b00000,0b00000,0b00000,0b00000,0b00000};
const uint8_t slipD2[8] = {0b00000,0b00001,0b00000,0b00011,0b00111,0b00011,0b00001,0b00011};
const uint8_t slipD3[8] = {0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b00011,0b00001};
const uint8_t slipD4[8] = {0b11111,0b11111,0b10111,0b10111,0b11111,0b11111,0b11111,0b11111};
const uint8_t slipD5[8] = {0b11100,0b11100,0b11000,0b11000,0b10000,0b11110,0b11110,0b11100};
const uint8_t slipD6[8] = {0b00000,0b11000,0b11100,0b11110,0b11110,0b11100,0b11000,0b11100};
// step 2 of 3
const uint8_t slipE1[8] = {0b00111,0b00111,0b00111,0b00011,0b00001,0b00000,0b00000,0b00000};
const uint8_t slipE2[8] = {0b00000,0b00011,0b00001,0b00111,0b01111,0b00111,0b00011,0b00111};
const uint8_t slipE3[8] = {0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b00111,0b00011};
const uint8_t slipE4[8] = {0b11111,0b11111,0b01111,0b01111,0b11111,0b11111,0b11111,0b11111};
const uint8_t slipE5[8] = {0b11000,0b11000,0b10000,0b00000,0b10000,0b11100,0b11100,0b11000};
const uint8_t slipE6[8] = {0b00000,0b00000,0b11000,0b11100,0b11100,0b11000,0b10000,0b11000};
// step 3 of 3
const uint8_t slipF1[8] = {0b01111,0b01111,0b00111,0b00111,0b00011,0b00001,0b00000,0b00000};
const uint8_t slipF2[8] = {0b00001,0b00111,0b00011,0b01111,0b11111,0b01111,0b00111,0b01111};
const uint8_t slipF3[8] = {0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b01111,0b00111};
const uint8_t slipF4[8] = {0b11110,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111,0b11111};
const uint8_t slipF5[8] = {0b00000,0b00000,0b00000,0b00000,0b00000,0b11000,0b11000,0b10000};
const uint8_t slipF6[8] = {0b00000,0b00000,0b10000,0b11000,0b11000,0b10000,0b00000,0b10000};

// ==========================================
// Logic implementation part
// ==========================================

// --- auxiliary function ---

// Dynamic speed regulation
static void Update_Speed(void) {
    uint32_t newDelay = 120 - (gameScore / 4) * 10;
    if (newDelay < 60) newDelay = 60;
    frameDelay_ms = newDelay;
}

static void Load_Art_Assets(void) {
    if (cgram_loaded_type == 1) return;
    LCD_CreateCustomChar(0, (uint8_t*)peace_L);
    LCD_CreateCustomChar(1, (uint8_t*)peace_R);
    LCD_CreateCustomChar(2, (uint8_t*)love_L);
    LCD_CreateCustomChar(3, (uint8_t*)love_R);
    LCD_CreateCustomChar(4, (uint8_t*)code_L);
    LCD_CreateCustomChar(5, (uint8_t*)code_R);
    cgram_loaded_type = 1;
}

static void Load_Game_Assets(void) {
    if (cgram_loaded_type == 2) return;
    LCD_CreateCustomChar(0, (uint8_t*)dino_run1);
    LCD_CreateCustomChar(1, (uint8_t*)dino_run2);
    LCD_CreateCustomChar(2, (uint8_t*)dino_jump);
    LCD_CreateCustomChar(3, (uint8_t*)obstacle);
    LCD_CreateCustomChar(4, (uint8_t*)ground);
    cgram_loaded_type = 2;
}

//Animation-specific rendering assistance: Load 6 characters and draw them to the specified column
static void Render_Anim_Frame(uint8_t col1, uint8_t col2, uint8_t col3,
                              const uint8_t* c1, const uint8_t* c2, const uint8_t* c3,
                              const uint8_t* c4, const uint8_t* c5, const uint8_t* c6) {
    // 1. Load the font model to CGRAM 0-5
    LCD_CreateCustomChar(0, (uint8_t*)c1);
    LCD_CreateCustomChar(1, (uint8_t*)c2);
    LCD_CreateCustomChar(2, (uint8_t*)c3);
    LCD_CreateCustomChar(3, (uint8_t*)c4);
    LCD_CreateCustomChar(4, (uint8_t*)c5);
    LCD_CreateCustomChar(5, (uint8_t*)c6);

    // 2. Clear the screen 
    LCD_Clear();

    // 3.draw
    // Table 1
    if(col1 < 16) { LCD_SetCursor(1, col1); LCD_WriteChar(0); } // Bottom
    if(col1 < 16) { LCD_SetCursor(0, col1); LCD_WriteChar(1); } // Top
    // Table 2
    if(col2 < 16) { LCD_SetCursor(1, col2); LCD_WriteChar(2); }
    if(col2 < 16) { LCD_SetCursor(0, col2); LCD_WriteChar(3); }
    // Table 3
    if(col3 < 16) { LCD_SetCursor(1, col3); LCD_WriteChar(4); }
    if(col3 < 16) { LCD_SetCursor(0, col3); LCD_WriteChar(5); }
}

// Non-blocking key detection
static uint8_t Check_Button_Event(void) {
    static uint32_t lastBtnTick = 0;
    static uint8_t btnState = 1;
    uint8_t currentLevel = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);

    if (HAL_GetTick() - lastBtnTick < 50) return 0;

    if (currentLevel == GPIO_PIN_RESET && btnState == 1) {
        btnState = 0;
        lastBtnTick = HAL_GetTick();
        return 1;
    } else if (currentLevel == GPIO_PIN_SET && btnState == 0) {
        btnState = 1;
        lastBtnTick = HAL_GetTick();
    }
    return 0;
}

// ==========================================
// game logic
// ==========================================

void Game_Reset(void) {
    gameScore = 0;
    playerY = 1;
    isJumping = 0;
    obstacleX = 16;
    gameSubState = GAME_PLAY;
    frameDelay_ms = 120;
    LCD_Clear();
}

void Game_Physics_Update(void) {
    if (isJumping) {
        jumpFrame++;
        if (jumpFrame < 2) playerY = 1;
        else if (jumpFrame < JUMP_DURATION + 2) playerY = 0;
        else {
            playerY = 1;
            isJumping = 0;
            jumpFrame = 0;
        }
    }
    obstacleX--;
    if (obstacleX < 0) {
        obstacleX = 15;
        gameScore++;
        Update_Speed();
    }
    if (obstacleX == 1 && playerY == 1) {
        gameSubState = GAME_OVER;
    }
    runAnimState = !runAnimState;
}

void Game_Render(void) {
    char line0[17];
    char line1[17];
    memset(line0, ' ', 16);
    memset(line1, ' ', 16);
    line0[16] = line1[16] = '\0';

    char scoreBuf[6];
    sprintf(scoreBuf, "%lu", gameScore);
    int scoreLen = strlen(scoreBuf);
    for(int i=0; i<scoreLen; i++) {
        line0[16 - scoreLen + i] = scoreBuf[i];
    }

    if (gameSubState == GAME_OVER) {
        line1[1] = 'X';
    } else {
        uint8_t charIdx;
        if (playerY == 0) charIdx = 2;
        else charIdx = (runAnimState) ? 0 : 1;

        if (playerY == 0) line0[1] = charIdx;
        else line1[1] = charIdx;
    }

    if (obstacleX >= 0 && obstacleX < 16) {
        line1[obstacleX] = 3;
    }

    LCD_SetCursor(0, 0); LCD_Print(line0);
    LCD_SetCursor(1, 0); LCD_Print(line1);
}

// ==========================================
// Animation state machine logic 
// ==========================================

void Animation_Reset(void) {
    animSubState = ANIM_RUN_RIGHT;
    animLoopIdx = 0; // 'a' in Arduino
    animFrameStep = 0;
    cgram_loaded_type = 0; // Mark CGRAM as "Dirty"，Reloading is required after exiting
}

void Animation_Update(void) {
    // The next frame will only be updated when the time is up
    if (HAL_GetTick() - lastAnimTick < ANIM_SPEED_MS) return;
    lastAnimTick = HAL_GetTick();

    int a = animLoopIdx;

    switch (animSubState) {
        // ------------------------------------------------
        // 1. Running Right (loop a: 0 -> 11, step 4)
        // ------------------------------------------------
        case ANIM_RUN_RIGHT:
            // There are 4 frames of animation inside (step 0-3)
            if (animFrameStep == 0) {
                // b(a+1), c(a+2), d(a+3)
                Render_Anim_Frame(a+1, a+2, a+3, stepB1, stepB2, stepB3, stepB4, stepB5, stepB6);
            } else if (animFrameStep == 1) {
                // c(a+2), d(a+3), e(a+4)
                Render_Anim_Frame(a+2, a+3, a+4, stepC1, stepC2, stepC3, stepC4, stepC5, stepC6);
            } else if (animFrameStep == 2) {
                // d(a+3), e(a+4), f(a+5) - reuse stepB assets
                Render_Anim_Frame(a+3, a+4, a+5, stepB1, stepB2, stepB3, stepB4, stepB5, stepB6);
            } else if (animFrameStep == 3) {
                // e(a+4), f(a+5), g(a+6)
                Render_Anim_Frame(a+4, a+5, a+6, stepD1, stepD2, stepD3, stepD4, stepD5, stepD6);
            }

            animFrameStep++;
            if (animFrameStep > 3) {
                animFrameStep = 0;
                animLoopIdx += 4; // a += 4
                if (animLoopIdx >= 11) {
                    animSubState = ANIM_SLIP_RIGHT;
                    animLoopIdx = 13; // Set 'a' for next state
                }
            }
            break;

        // ------------------------------------------------
        // 2. Slip Right (static a=13, 3 frames)
        // ------------------------------------------------
        case ANIM_SLIP_RIGHT:
            // a=13, b=14, c=15
            if (animFrameStep == 0) {
                Render_Anim_Frame(a, a+1, a+2, slipA1, slipA2, slipA3, slipA4, slipA5, slipA6);
            } else if (animFrameStep == 1) {
                Render_Anim_Frame(a, a+1, a+2, slipB1, slipB2, slipB3, slipB4, slipB5, slipB6);
            } else if (animFrameStep == 2) {
                Render_Anim_Frame(a, a+1, a+2, slipC1, slipC2, slipC3, slipC4, slipC5, slipC6);
            }

            animFrameStep++;
            if (animFrameStep > 2) {
                animFrameStep = 0;
                animSubState = ANIM_RUN_LEFT;
                animLoopIdx = 16; // Set 'a' start for running left
            }
            break;

        // ------------------------------------------------
        // 3. Running Left (loop a: 16 -> 4, step -4)
        // ------------------------------------------------
        case ANIM_RUN_LEFT:
            if (animFrameStep == 0) {
                // d(a-3), c(a-2), b(a-1)
                Render_Anim_Frame(a-3, a-2, a-1, stepE1, stepE2, stepE3, stepE4, stepE5, stepE6);
            } else if (animFrameStep == 1) {
                // e(a-4), d(a-3), c(a-2)
                Render_Anim_Frame(a-4, a-3, a-2, stepF1, stepF2, stepF3, stepF4, stepF5, stepF6);
            } else if (animFrameStep == 2) {
                // f(a-5), e(a-4), d(a-3) - reuse E
                Render_Anim_Frame(a-5, a-4, a-3, stepE1, stepE2, stepE3, stepE4, stepE5, stepE6);
            } else if (animFrameStep == 3) {
                // g(a-6), f(a-5), e(a-4)
                Render_Anim_Frame(a-6, a-5, a-4, stepG1, stepG2, stepG3, stepG4, stepG5, stepG6);
            }

            animFrameStep++;
            if (animFrameStep > 3) {
                animFrameStep = 0;
                animLoopIdx -= 4; // a -= 4
                if (animLoopIdx <= 4) {
                    animSubState = ANIM_SLIP_LEFT;
                    animLoopIdx = 1; // Set 'a' for slip left
                }
            }
            break;

        // ------------------------------------------------
        // 4. Slip Left (static a=1, 3 frames)
        // ------------------------------------------------
        case ANIM_SLIP_LEFT:
            // a=1, b=2, c=3
            if (animFrameStep == 0) {
                Render_Anim_Frame(a, a+1, a+2, slipD1, slipD2, slipD3, slipD4, slipD5, slipD6);
            } else if (animFrameStep == 1) {
                Render_Anim_Frame(a, a+1, a+2, slipE1, slipE2, slipE3, slipE4, slipE5, slipE6);
            } else if (animFrameStep == 2) {
                Render_Anim_Frame(a, a+1, a+2, slipF1, slipF2, slipF3, slipF4, slipF5, slipF6);
            }

            animFrameStep++;
            if (animFrameStep > 2) {
                animFrameStep = 0;
                animSubState = ANIM_DONE; // Mark the completion of a single loop
            }
            break;

        case ANIM_DONE:
            // When the animation finishes playing, restart the loop or wait for the button to be pressed
            // According to the logic of demand：RunR -> SlipR -> RunL -> SlipL -> (Loop or Wait)
            // Here, reset to RunR to keep the loop playing until the user presses the button
            animSubState = ANIM_RUN_RIGHT;
            animLoopIdx = 0;
            break;
    }
}

// ==========================================
// external interface
// ==========================================

void UI_Init(void) {
    LCD_Init();
    srand(HAL_GetTick());
}

void UI_Update(RTC_HandleTypeDef *hrtc) {
    uint8_t btnPressed = Check_Button_Event();

    switch (currentState) {
        // ---------------- STATE: SPLASH ----------------
        case STATE_SPLASH:
            if (HAL_GetTick() - lastSplashTick >= SPLASH_FRAME_MS) {
                if (splashFrame < 16) splashFrame++;
                lastSplashTick = HAL_GetTick();
            }

            uint8_t col = splashFrame;
            char line0[17], line1[17];
            memset(line0, ' ', 16);
            memset(line1, ' ', 16);
            line0[16] = line1[16] = '\0';

            const char *str0 = "MES204TC";
            uint8_t len0 = 8;
            int8_t pos0 = 16 - col;
            if (pos0 >= 0 && pos0 < 16) {
                for (uint8_t i = 0; i < len0 && pos0 + i < 16; i++)
                    line0[pos0 + i] = str0[i];
            }

            const char *str1 = "Dr.:QiLi";
            uint8_t len1 = 8;
            int8_t pos1 = -len1 + col;
            for (uint8_t i = 0; i < len1; i++) {
                int8_t x = pos1 + i;
                if (x >= 0 && x < 16)
                    line1[x] = str1[i];
            }

            LCD_SetCursor(0, 0); LCD_Print(line0);
            LCD_SetCursor(1, 0); LCD_Print(line1);

            if (btnPressed && splashFrame >= 15) {
                currentState = STATE_INFO;
                splashFrame = 0;
                cgram_loaded_type = 0;//Forced reload
                LCD_Clear();
            }
            break;

        // ---------------- STATE: INFO ----------------
        case STATE_INFO:
            if (btnPressed) {
                currentState = STATE_ART;
                cgram_loaded_type = 0;
                LCD_Clear();
                return;
            }

            static uint32_t lastInfoUpdate = 0;
            if (HAL_GetTick() - lastInfoUpdate > 200) {
                RTC_DateTypeDef sDate;
                RTC_TimeTypeDef sTime;
                HAL_RTC_GetTime(hrtc, &sTime, RTC_FORMAT_BIN);
                HAL_RTC_GetDate(hrtc, &sDate, RTC_FORMAT_BIN);

                LCD_SetCursor(0, 0);
                LCD_Print("ID: 2364116     ");

                char buf[17];
                sprintf(buf, "20%02d/%02d/%02d      ", sDate.Year, sDate.Month, sDate.Date);
                LCD_SetCursor(1, 0);
                LCD_Print(buf);
                lastInfoUpdate = HAL_GetTick();
            }
            break;

        // ---------------- STATE: ART ----------------
        case STATE_ART:
            Load_Art_Assets();

            if (btnPressed) {
                currentState = STATE_ANIMATION; // Modify: Jump to Animation
                Animation_Reset(); // Reset the animation status
                cgram_loaded_type = 0;
                LCD_Clear();
                return;
            }

            static uint8_t artRendered = 0;
            if (!artRendered) {
                LCD_SetCursor(1, 0);
                LCD_Print("Peace Love Code ");
                LCD_SetCursor(0, 1); LCD_WriteChar(0); LCD_WriteChar(1);
                LCD_SetCursor(0, 7); LCD_WriteChar(2); LCD_WriteChar(3);
                LCD_SetCursor(0, 12); LCD_WriteChar(4); LCD_WriteChar(5);
                artRendered = 1;
            }
            if (currentState != STATE_ART) artRendered = 0;
            break;

        // ---------------- STATE: ANIMATION ----------------
        case STATE_ANIMATION:
            // The animation state is not required Load_Asset Preload because each frame is changing

            if (btnPressed) {
                currentState = STATE_GAME;
                gameSubState = GAME_WAIT;
                cgram_loaded_type = 0;
                LCD_Clear();
                return;
            }

            Animation_Update();
            break;

        // ---------------- STATE: GAME ----------------
        case STATE_GAME:
            Load_Game_Assets(); // Make sure to load the game font (as Animation has disrupted the CGRAM)

            switch (gameSubState) {
                case GAME_WAIT:
                    LCD_SetCursor(0, 0);
                    LCD_Print(" PRESS BUTTON   ");
                    LCD_SetCursor(1, 0);
                    LCD_Print(" TO START       ");

                    if (btnPressed) {
                        Game_Reset();
                    }
                    break;

                case GAME_PLAY:
                    if (btnPressed && playerY == 1) {
                        isJumping = 1;
                        jumpFrame = 0;
                    }

                    if (HAL_GetTick() - lastFrameTick > frameDelay_ms) {
                        Game_Physics_Update();
                        Game_Render();
                        lastFrameTick = HAL_GetTick();
                    }
                    break;

                case GAME_OVER:
                    LCD_SetCursor(0, 3);
                    LCD_Print("GAME OVER!");

                    if (btnPressed) {
                        currentState = STATE_INFO; // Loop back to INFO
                        LCD_Clear();
                    }
                    break;
            }
            break;
    }
}



