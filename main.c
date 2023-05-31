/*******************************************************
 * main.c - main program file for KL05Z - VGA display  *
 ******************************************************/
/*
			Freescale KL05Z VGA Display
	
	Pin assingment:
		- PB13: vertical sync pin
		- PB6: TPM Clock input connected via wire to PB6
		- PA7: SPI output, pixel data
		- PB9: horizontal sync pulse
		
	Video Information:
		Resolution: 800x600
		Refresh rate: 56Hz
		Pixel clock: 36MHz
		HSync pulse frequency: 31.5kHz
		Jitter: A lot.
	
*/
#define delay_ms(x) for(uint32_t i=0; i<x*10000;i++)__nop();

#include "main.h"
#include "vga.h"
#include "game.h"
/*
	1 bit sent to the display lasts for 4 pixels(SPI Clock is slower than pixel clock for this resolution),
	thats why we need to "thicken" the line. lineToDraw is incrementing every four lines sent.
*/
uint8_t lineToDraw = 0;
uint16_t vline = 0; //current line being drawn
uint8_t vready = 0;//vertical ready status 
uint8_t busy = 0;//DMA busy user flag
extern uint8_t fb[140][25];//main video frame buffer - 140 lines, 25 bytes per line = 200 bits(pixels) per line
char textBuf[20]; //temp char buffer

/*
	void sendSPIDMA(uint8_t* buff, uint16_t size)
	Initializes and performs DMA setup to send whole video frame buffer by SPI.
*/
void sendSPIDMA(uint8_t* buff, uint16_t size)
{ 	
	if(!busy)
	{
		DMA0->DMA->DSR_BCR = DMA_DSR_BCR_DONE_MASK;
		DMA0->DMA->SAR = (uint32_t) buff;
		DMA0->DMA->DAR = (uint32_t) &SPI0->D;
		DMA0->DMA->DSR_BCR |= DMA_DSR_BCR_BCR(size);
		DMA0->DMA->DCR |= DMA_DCR_EINT_MASK | DMA_DCR_SSIZE(1)|DMA_DCR_DSIZE(1)|DMA_DCR_ERQ_MASK|DMA_DCR_SINC_MASK|DMA_DCR_D_REQ_MASK|DMA_DCR_CS_MASK;
		SPI0->C1 |= SPI_C1_SPIE_MASK | SPI_C1_SPE_MASK | SPI_C1_SPTIE_MASK;
		SPI0->C2 |= SPI_C2_TXDMAE_MASK;
		busy = 1;
	}
	else
		{
		return;
	}
}

int main(void)
{
	//PORTs clock gating
	SIM->SCGC5 |= SIM_SCGC5_PORTB_MASK;
	SIM->SCGC5 |= SIM_SCGC5_PORTA_MASK;
	//enable LEDs for debugging 
	PORTB->PCR[8] = PORT_PCR_MUX(1);
	PORTB->PCR[0] = PORT_PCR_MUX(3);		
	PORTA->PCR[7] = PORT_PCR_MUX(3);
	PTB->PDDR |= (1<<8);
	//SPI, DMA, DMAMUX clock gating
	SIM->SCGC4 |= SIM_SCGC4_SPI0_MASK;
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
	//DMAMUX set for SPI
	DMAMUX0->CHCFG[0] |= DMAMUX_CHCFG_SOURCE(17);
	DMAMUX0->CHCFG[0] |= DMAMUX_CHCFG_ENBL_MASK;
	//SPI set as master with highest possible baudrate
	SPI0->C1 |= SPI_C1_MSTR_MASK;
	SPI0->BR |= SPI_BR_SPPR(0);
	SPI0->BR |= SPI_BR_SPR(0);
	//DMA interrupt vector settings
	NVIC_SetPriority(DMA0_IRQn, 0);
	NVIC_ClearPendingIRQ(DMA0_IRQn);
	NVIC_EnableIRQ(DMA0_IRQn);
	//dummy send - to init DMA and set all values
	sendSPIDMA(&fb[0][0], 25);
	//timers init
	HSYNC_Init();
	VSYNC_Init();
	
	uint8_t temp =0;
	gameWelcomeScreen();
	gameInit();
	while(1)
	{
	/*		
		printString(textBuf, 1, 120);
		for(double a=0;a<2*3.1415;a+=3.1415/24)
		{
			printLine(100,50, (uint8_t)(100+44*cos(a)), (uint8_t)(50+44*sin(a)), 1);
			delay_ms(10);
			printLine(100,50, (uint8_t)(100+44*cos(a)), (uint8_t)(50+44*sin(a)), 0);
			printLine(100,50, (uint8_t)(100+22*cos(b)), (uint8_t)(50+22*sin(b)), 1);

		}	
		printLine(100,50, (uint8_t)(100+22*cos(b)), (uint8_t)(50+22*sin(b)), 0);
		if(b<2*3.1415)b+=3.1415/24;
		else b =0; */
  }
		
}
void DMA0_IRQHandler()
{
	//clear flags
	DMA0->DMA->DSR_BCR = DMA_DSR_BCR_DONE_MASK;
	busy = 0;
}
void TPM0_IRQHandler(void)
{
	//new line is ready, clear interrupt flag
	TPM0->CONTROLS[1].CnSC |= TPM_CnSC_CHF_MASK;
	if(vready) //if not in backporch - send new line
	{
		sendSPIDMA(&fb[lineToDraw][0],25);
		if((vline++)%4 == 0 && lineToDraw<130 && vline !=1)lineToDraw++; //make line 4 pixels thick
	}
}
void TPM1_IRQHandler(void)
{
	//all lines sent, clear interrupt flag
	PTB->PTOR |= (1<<8); //debug led
	if(	TPM1->CONTROLS[0].CnSC & TPM_CnSC_CHF_MASK)
	{
		TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHF_MASK;
		vready = 1; //after vertical backporch, set ready flag
	}
	if(TPM1->SC & TPM_SC_TOF_MASK)
	{
		TPM1->SC |= TPM_SC_TOF_MASK;
		updateFlagSet(); // set flag to calculate a new ball position in game
		vline = lineToDraw = vready = 0;//clear flags and variables as the whole screen is drawn; it's backporch time
	}
}
