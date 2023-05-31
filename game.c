/****************************************************
 * game.c - Ball Game logic file for KL05Z - VGA    *
 ****************************************************/

#include "game.h"
#include "tsi.h"
#define delay_ms(x) for(uint32_t i=0; i<x*10000;i++)__nop();
#define BAR_LENGTH 10 
#define BAR_THICK 4 //bar thickness
#define PLAYER_HEIGHT 2 //player thickness
//private variables/flags
unsigned char misses = 0, flag = 0, ballMove = 0;
char txtBuf[20];
//frame buffer
extern unsigned char fb[140][25];

/* Design Pattern : State */
enum GameState
{
	Init,
	GetReady,
	InGame,
	Missed,
	GameOver
}_state;


//Bar is an object to destroy by a Ball. There are 57 bars.
//Each bar has an ID, x and y coordinate.
struct Bar 
	{
		unsigned char id;
		unsigned char x;
		unsigned char y;
	}bars[57];
	
//Player is a bar on the bottom on the screen, used to bounce a ball and destroy bars.
//Player has a length and x,y coordinates.
struct Player
	{
		unsigned char x;
		unsigned char y;
		unsigned char length;
	}player;
//Ball is an object that bounces off the walls, bars and player. 
//Ball has a direction of movement, xy coordinates.
struct Ball 
{
	unsigned char x;
	unsigned char y;
	unsigned char dir;	
}ball;

//void mainLoop()
//Main game loop.
void mainLoop()
{
	double b = 0;
	unsigned char ready = 0;
		ballMove = 0;
	//TSI value variable
	unsigned char val = 0;
	_state = Init;
	while(1)
	{
		/* State Design Pattern implementation */
		switch(_state)
		{
			case Init:
				misses = 0;
				ball.dir = 1;
				player.x = 90;
				player.y = 125;	
				ball.x = 100;
				ball.y = 124;
				ball.dir = 1;
				clearScreen();
				gamePrintPlayer(1);
				gamePrintBars();
				gamePrintBall(1);
				_state = GetReady;
				break;
			case GetReady:
				printString("Get Ready!", 7, 60);
				delay_ms(2000);
				_state = InGame;
				break;
			case InGame:
				printString("           ", 7, 60); // clear "Get Ready" string from buffer
				ballMove = 1; //start ball movement
				val = TSI_ReadSlider();
				if(val>0)gameMovePlayer(val *1.85);
				if(flag == 1)
				{
					flag = 0;
					calculateBallPosition();
				}
				delay_ms(10);	
				break;
			case Missed:
					misses ++;
					if(misses <5)
					{
						sprintf(txtBuf, "Missed  %d/5", misses);
						printString(txtBuf, 2, 80);
						delay_ms(3000);
						printString("            ", 2, 80);
						gameMoveBall(player.x + 10, player.y-1);
						ball.dir =  1;
						_state = InGame;

					}
					else
					{
						_state = GameOver;
					}
				break;
			case GameOver:
					clearScreen();
					printString("GAME OVER!", 2, 60);
					delay_ms(5000);
					_state = Init;
				break;
			default:
				_state = Init;
				break;
		}
		
	}
}
//void gameInit()
//Initialaze variables
void gameInit()
{
	misses = 0;
	TSI_Init();
	player.length = 20;
	player.x = 90;
	player.y = 125;
	
	ball.x = 100;
	ball.y = 124;
	ball.dir = 1;
	//gameWelcomeScreen();
	//delay_ms(3000);

	mainLoop();
}
//void gameWelcomeScreen()
//Shows a walceome message on the screen.
void gameWelcomeScreen()
{
	clearScreen();
	printString("KL-05z VGA", 7, 3);
	delay_ms(3000);
	printString("Simple Ball Game", 5, 40);
	delay_ms(3000);
	printString("Designed by:", 7, 70);
	printString("Mikolaj Pienko", 6, 90);
	printString("Marlena Krysiuk", 5, 100);
	delay_ms(3000);
}
//void gamePrintPlayer(unsigned char visibility)
//Print player's bar on the bottom
void gamePrintPlayer(unsigned char visibility)
{
	for(unsigned char i=0;i<PLAYER_HEIGHT;i++)
	{
		printLine(player.x, player.y+i,player.x+player.length, player.y+i, visibility);
	}
}
//void gameMovePlayer(unsigned char x)
//Moves player to the position of x.
void gameMovePlayer(unsigned char x)
{
	gamePrintPlayer(0);
	player.x = x;
	gamePrintPlayer(1);
}
//void gamePrintBars()
//Prints bars on the screen
void gamePrintBars()
{
	for(unsigned char i = 0; i< sizeof(bars)/sizeof(bars[0]);i++)
	{
		bars[i].id = i;
		
	
		if(i>19 && i<=38)
		{
			bars[i].x = i*BAR_LENGTH - 200;
			bars[i].y = BAR_THICK + 2;
		}
		else if(i>38)
		{
			bars[i].x = (i*BAR_LENGTH-390);
			bars[i].y = BAR_THICK*2 + 3;
		}
		else
		{
			bars[i].x = i*BAR_LENGTH;
			bars[i].y = 1;
		}
		printRectangle(bars[i].x+1, bars[i].y, (bars[i].x)+BAR_LENGTH-1, bars[i].y+BAR_THICK, 1);
	}
}
//void removeBar(unsigned char id)
//Removes bar from the screen based on its id
void removeBar(unsigned char id)
{
	printRectangle(bars[id].x+1, bars[id].y, (bars[id].x)+BAR_LENGTH-1, bars[id].y+BAR_THICK, 0);
}
//void gamePrintBall(unsigned char visible)
//Prints/erases ball from the screen
void gamePrintBall(unsigned char visible)
{
	printPoint(ball.x, ball.y, visible);
}
//void gameMoveBall(unsigned char x, unsigned char y)
//Moves ball to specified x/y coordinates
void gameMoveBall(unsigned char x, unsigned char y)
{
	gamePrintBall(0);
	ball.x = x;
	ball.y = y;
	gamePrintBall(1);
}
//unsigned char checkForCollision(unsigned char x, unsigned char y)
//Checks if x/y points belong to an object.
//If certain bit in the frame buffer is set it means that there is something drawn.
unsigned char checkForCollision(unsigned char x, unsigned char y)
{
	if((fb[y][(uint8_t)x/8] & (1<<7)>>x%8))//check for bit in the frame buffer
	{
		if(ball.y>=125) // if something is drawn further than y = 125, it's a player.
		{
			if(x<player.x+(player.length/2))
			{
				ball.dir = 4;
			}
			else ball.dir = 1;
		}
		else if(y <120)//else it's a bar
		{
			switch(ball.dir)
			{
				case 1:
					for(unsigned int i = 0; i< sizeof(bars)/sizeof(bars[0]);i++)
					{
						if(x < bars[i].x+BAR_LENGTH  && x>bars[i].x && y>=bars[i].y && y<=bars[i].y+BAR_THICK)
						{
							removeBar(i);
							if(y == bars[i].y+BAR_THICK-1)ball.dir = 2;
							else ball.dir = 4;
						}
					}
					break;
				case 2:
					for(unsigned int i = 0; i< sizeof(bars)/sizeof(bars[0]);i++)
					{
						if(x < bars[i].x+BAR_LENGTH  && x>bars[i].x&& y>=bars[i].y && y<=bars[i].y+BAR_THICK)
						{
							removeBar(i);
							if( y == bars[i].y)	ball.dir = 1;
							else ball.dir = 3;
						}
					}
					break;
				case 3:
					for(unsigned int i = 0; i< sizeof(bars)/sizeof(bars[0]);i++)
					{
						if(x < bars[i].x+BAR_LENGTH  && x>bars[i].x&& y>=bars[i].y && y<=bars[i].y+BAR_THICK)
						{
							removeBar(i);
							if( y == bars[i].y)	ball.dir = 4;
							else ball.dir = 2;
						}
					}
					break;
				case 4:
					for(unsigned int i = 0; i< sizeof(bars)/sizeof(bars[0]);i++)
					{
						if(x < bars[i].x+BAR_LENGTH  && x>bars[i].x&& y>=bars[i].y && y<=bars[i].y+BAR_THICK)
						{
							removeBar(i);
							if( y == bars[i].y+BAR_THICK-1)	ball.dir = 3;
							else ball.dir = 1;
						}
					}
					break;
				default:
					
					break;
			}
		}
		return 1;
	}
	else if(x<=1)//or a wall
	{
		switch(ball.dir)
		{
			case 3:
				ball.dir = 2;
				break;
			case 4:
				ball.dir = 1;
				break;
			default:
				
				break;
		}
		return 1;
	}
	else if(x >= 200)//or a wall
	{
		switch(ball.dir)
		{
			case 1:
				ball.dir = 4;
				break;
			case 2:
				ball.dir = 3;
				break;
			default:
				
				break;
		}
		return 1;
	}
	else if(y<=1)//or a ceiling
	{
		switch(ball.dir)
		{
			case 1:
				ball.dir = 2;
				break;
			case 4:
				ball.dir = 3;
				break;
			default:
				
				break;
		}
		return 1;
	}
	else if(y>=130)//or a floor
	{
		_state = Missed;
		
		return 1;
	}
	else return 1;
}
//void calculateBallPosition()
//Moves ball based on its direction
void calculateBallPosition()
{
	if(ballMove)
	{
		switch(ball.dir)
		{
			case 1:
				if(checkForCollision(ball.x+1, ball.y-1))
				{
					gamePrintBall(0);
					ball.x += 1;
					ball.y -= 1;
					gamePrintBall(1);
				}
				break;
			case 2:
				if(checkForCollision(ball.x+1, ball.y+1))
				{
					gamePrintBall(0);
					ball.x += 1;
					ball.y += 1;
					gamePrintBall(1);
				}
			break;
			case 3:
				if(checkForCollision(ball.x-1, ball.y+1))
				{
					gamePrintBall(0);
					ball.x -= 1;
					ball.y += 1;
					gamePrintBall(1);
				}
				break;
			case 4:
				if(checkForCollision(ball.x-1, ball.y-1))
				{
					gamePrintBall(0);
					ball.x -= 1;
					ball.y -= 1;
					gamePrintBall(1);
				}
				break;
			
			default:
				
				break;
		}
	}
}
//void updateFlagSet()
//Sets flag to update ball's position
void updateFlagSet()
{
	flag = 1;
}