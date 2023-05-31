/**********************************************************************
 * vga.h - frame buffer content's editor header file for KL05Z - VGA  *
 ******************************************8***************************/

#include "stdint.h"
#include "string.h"
#include "math.h"

void clearScreen(void);
void printPoint(uint8_t x, uint8_t y, uint8_t visible);
void printLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t visible);
void printChar(char character, uint8_t x, uint8_t y);
void printCircle(uint8_t x, uint8_t y, uint8_t radius, uint8_t visible);
void printString(const char* string, uint8_t x, uint8_t y);
void printRectangle(uint8_t x, uint8_t y, uint8_t x1, uint8_t y1, uint8_t visible);
