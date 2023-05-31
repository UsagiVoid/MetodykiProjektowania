/**********************************************************************************
 * tim.c - timer initialization file for KL05Z - VGA horizontal and vertical sync *
 **********************************************************************************/

#include "tim.h"
#include "main.h"

void HSYNC_Init(void)
{
	//TPM0 - horizontal sync pulse generator
	SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;
	SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

	PORTB->PCR[9] |= PORT_PCR_MUX(2);
	PORTB->PCR[10] |= PORT_PCR_MUX(2);
	//1 CPU tick  = 0.0208us
	TPM0->SC |= TPM_SC_PS(0);
	TPM0->SC |= TPM_SC_CMOD(1);
	TPM0->MOD = 1190;
	TPM0->SC &= ~TPM_SC_CPWMS_MASK;
	TPM0->CONTROLS[2].CnSC |= (TPM_CnSC_ELSB_MASK | TPM_CnSC_MSB_MASK);
	//35.1 kHz with 2us pulse width
	TPM0->CONTROLS[2].CnV = 86;
	TPM0->CONTROLS[1].CnSC |= (TPM_CnSC_ELSB_MASK | TPM_CnSC_MSA_MASK);
	TPM0->CONTROLS[1].CnV = 124;
	TPM0->CONTROLS[1].CnSC |= (TPM_CnSC_CHIE_MASK);
	//interrupt comes after 4 us - backporch
	NVIC_SetPriority(TPM0_IRQn, 1);
	NVIC_ClearPendingIRQ(TPM0_IRQn);
	NVIC_EnableIRQ(TPM0_IRQn);
}
void VSYNC_Init(void)
{
//TPM1 CH1 CH2 - synchronized with TPM0
	PORTB->PCR[13] |= PORT_PCR_MUX(2);
	PORTB->PCR[6] |= PORT_PCR_MUX(3); //PB6 - TPM_CLKIN1 - TPM0 output
	
	SIM->SCGC6 |=SIM_SCGC6_TPM1_MASK;
	SIM->SOPT4 |= SIM_SOPT4_TPM1CLKSEL_MASK;
	
	TPM1->SC |= TPM_SC_CMOD(2);
	TPM1->SC |= TPM_SC_PS(0);
	TPM1->SC &=~TPM_SC_CPWMS_MASK;
	TPM1->SC |= TPM_SC_TOIE_MASK;
	//56Hz refresh rate - 625 lines; 1 line = 1 sync pulse from TPM0
	TPM1->MOD = 625;
	TPM1->CONTROLS[0].CnSC |= (TPM_CnSC_ELSB_MASK | TPM_CnSC_MSA_MASK);
	TPM1->CONTROLS[0].CnV = 23;//vertical backporch
	TPM1->CONTROLS[0].CnSC |= TPM_CnSC_CHIE_MASK;
	
	TPM1->CONTROLS[1].CnSC |= (TPM_CnSC_ELSB_MASK | TPM_CnSC_MSB_MASK);
	TPM1->CONTROLS[1].CnV = 2;//vertical pulse with = 2 lines
	NVIC_SetPriority(TPM1_IRQn, 1);
	NVIC_ClearPendingIRQ(TPM1_IRQn);
	NVIC_EnableIRQ(TPM1_IRQn);
}
