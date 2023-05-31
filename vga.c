/***************************************************************
 * vga.c - frame buffer content's editor file for KL05Z - VGA  *
 ***************************************************************/


#include "vga.h"

uint8_t fb[140][25];
extern const char fontChars[];
/*
	void clearScreen()
	Clears all bits in the frame buffer, thus it clears the screen.
*/
void clearScreen()
{
	for(uint8_t y = 0; y<140;y++)
		{
			for(uint8_t x =0; x<25;x++)
			{
				fb[y][x] = 0;
			}
		}
}
/*
	void printPoint(u8 x, u8 y, u8 visible)
	Sets/resets bit in the frame buffer, chosen by [x,y] coorinates; Basic function to
	draw more complex objects like circles, lines etc.
	visible = 1 - bit is set,
	visible = 0 - bit is cleared.
	Visible flag is added to improve the effinciency, as it's not necessary to
	clear the whole screen to remove objects/points from the buffer.
*/
void printPoint(uint8_t x, uint8_t y, uint8_t visible)
{
	if(x>207)return;
	if(x==0) fb[y][0] = 1<<7;
	else
	{
		if(visible)fb[y][(uint8_t)x/8] |= (1<<7)>>x%8;	
		else fb[y][(uint8_t)x/8] &= ~((1<<7)>>x%8);
	}
}

/*
	void printLine(u8 x1, u8 y1, u8 x2, u8 y2, u8 visible)
	Draws line from point (x1,y1) to (x2,y2). Visible flag draws/erases line from buffer.
*/
void printLine(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t visible)
{
	int16_t		dx, dy, i, e;
	int16_t		incx, incy, inc1, inc2;
	int16_t		x, y;

	dx = x2 - x1;
	dy = y2 - y1;

	if(dx < 0) dx = -dx;
	if(dy < 0) dy = -dy;
	incx = 1;
	if(x2 < x1) incx = -1;
	incy = 1;
	if(y2 < y1) incy = -1;
	x=x1;
	y=y1;

	if (dx > dy) {
		printPoint(x, y, visible);
		e = 2*dy - dx;
		inc1 = 2 * ( dy -dx);
		inc2 = 2 * dy;
		for (i = 0; i < dx; i++) {
			if (e >= 0) {
				y += incy;
				e += inc1;
			}
			else {
				e += inc2;
			}
			x += incx;
			printPoint(x, y, visible);
		}
	} else {
			printPoint(x, y, visible);
		e = 2 * dx - dy;
		inc1 = 2 * (dx - dy);
		inc2 = 2 * dx;
		for(i = 0; i < dy; i++) {
			if (e >= 0) {
				x += incx;
				e += inc1;
			} else {
				e += inc2;
			}
			y += incy;
			printPoint(x, y, visible);
		}
	}
}
/*
	void printChar(char character, uint8_t x, uint8_t y)
	Draws character on the screen on specified coordinates. Characters are imported from font.h.
*/

void printChar(char character, uint8_t x, uint8_t y)
{
	for(uint8_t j = 0;j<7;j++)
	{
		fb[y+j][x] = fontChars[(character-0x20)*9+1+j];
	}
}
/*
	void printCircle(uint8_t x, uint8_t y, uint8_t radius, uint8_t visible)
	Draws a circle with set radius on specified coordinates.
*/
void printCircle(uint8_t x, uint8_t y, uint8_t radius, uint8_t visible)
{
	int xx, yy;
	for(uint16_t a = 0; a<360;a++)
	{
		xx = radius*cos((double)a);
		yy = radius*sin((double)a);
		printPoint(x+xx, y+yy, visible);
	}
}
/*
	void printString(const char* string, uint8_t x, uint8_t y)
	Draws string on the screen by looping printChar function.
*/
void printString(const char* string, uint8_t x, uint8_t y)
{	
	for(uint8_t i = 0; i<strlen(string);i++)
	{
		printChar(string[i], x+i, y);
	}
}
/*
	void printRectangle(uint8_t x, uint8_t y, uint8_t x1, uint8_t y1,uint8_t visible)
	Draws rectangle from point (x,y) to (x1,y1). Visible flag draws/erases rectangle from buffer.
*/
void printRectangle(uint8_t x, uint8_t y, uint8_t x1, uint8_t y1,uint8_t visible)
{
	for(uint8_t i = 0; i<y1-y;i++)
	{
		printLine(x, y+i, x1, y+i, visible);
	}
}