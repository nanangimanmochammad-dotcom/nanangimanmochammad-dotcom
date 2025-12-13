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

// 定义系统状态
typedef enum {
    STATE_SPLASH,    // 滚动显示
    STATE_INFO,      // 显示学号日期
    STATE_ART,       // 显示图案 (Peace/Love/Code)
    STATE_ANIMATION, // [新增] 巨型马里奥动画 (3x2 字符)
    STATE_GAME       // 超级玛丽跑酷游戏
} UI_State_t;

// 初始化 UI 资源
void UI_Init(void);

// 状态机处理函数 (放在主循环里，需传入 RTC 句柄)
void UI_Update(RTC_HandleTypeDef *hrtc);

#endif /* INC_MANAGER_H_ */
