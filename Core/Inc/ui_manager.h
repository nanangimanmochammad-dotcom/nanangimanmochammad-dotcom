/*
 * ui_manager.h
 *
 *  Created on: Dec 12, 2025
 *      Author: admin
 */

#ifndef INC_MANAGER_H_
#define INC_MANAGER_H_

#include "main.h"
#include "lcd1602.h"

// Define the system state
typedef enum {
    STATE_SPLASH,    // scrolling display

    STATE_INFO,      // Display the student number and date
    STATE_ART,       // Broken Image (Peace/Love/Code)
    STATE_ANIMATION, // [New Addition] Giant Mario Animation (3x2 characters)
    STATE_GAME       // Super Mario Parkour game
} UI_State_t;

// Initialize UI resources
void UI_Init(void);

//State machine handling function (placed in the main loop, an RTC handle needs to be passed in)
void UI_Update(RTC_HandleTypeDef *hrtc);

#endif /* INC_MANAGER_H_ */


