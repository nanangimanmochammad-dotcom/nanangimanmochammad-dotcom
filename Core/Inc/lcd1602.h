/*
 * lcd1602.h
 *
 *  Created on: Dec 12, 2025
 *      Author: admin
 */

#ifndef INC_LCD1602_H_
#define INC_LCD1602_H_

#include "main.h" // Introduce the HAL library and pin definitions

// Core functional function
void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCursor(uint8_t row, uint8_t col);
void LCD_Print(char *str);
void LCD_WriteChar(uint8_t data); // Write a single character
void LCD_CreateCustomChar(uint8_t location, uint8_t *charmap); //Create custom ICONS

#endif /* INC_LCD1602_H_ */

