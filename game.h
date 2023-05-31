#include "vga.h"

void gameInit(void);
void gameWelcomeScreen(void);
void gamePrintPlayer(unsigned char visibility);
void gameMovePlayer(unsigned char x);
void gameChangePlayerWidth(unsigned char length);
void gamePrintBars(void);
void gamePrintBall(unsigned char visible);
void gameMoveBall(unsigned char x, unsigned char y);
void calculateBallPosition(void);
void removeBar(unsigned char id);
void updateFlagSet(void);

