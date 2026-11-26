#include "RCC.h"

void RCC_Config() {
	RCC_CR |=(1<<16) ;//Enable HSE
	while(!(RCC_CR & (1<<17))) ;
	FLASH_ACR |= (1<<2) ;

	RCC_CFGR &=~ ((0xF<<18) | (1<<16)) ; // Reset PLLSRC bit and PLLMUL bits
	RCC_CFGR |= (1<<16) ; //Clock from PREDIV1 selected as PLL input clock
	RCC_CFGR |=(6<<18) ; //PLL input clock x 8

	RCC_CR |=(1<<24) ; //On PLL
	while(!(RCC_CR & (1<<25))) ; //Wait for PLL lock

	RCC_CFGR &=~(0xF<<4) ; //AHB clock not divided (/1)
	RCC_CFGR &=~(7<<11) ; //APB2 HCLK not divided
	RCC_CFGR |=(4<<8) ; //APB1 HCLK divided by 2

	RCC_CFGR &=~(3<<0) ;// Reset SW bit
	RCC_CFGR |= (2<<0) ;// PLL selected as system clock
	while((RCC_CFGR & (3 << 2)) != (2 << 2));//wait
}

void RCC_Enable_PortA(void) {
	RCC_APB2ENR |= (1<<2) ;
}
void RCC_Enable_PortB(void) {
	RCC_APB2ENR |= (1<<3) ;
}
void RCC_Enable_PortC(void) {
	RCC_APB2ENR |= (1<<4) ;
}
void RCC_Enable_AFIO(void) {
	RCC_APB2ENR |= (1<<0) ;
}
void RCC_Enable_TIM2(void) {
	RCC_APB1ENR |= (1<<0) ;
}
void RCC_Enable_TIM3(void) {
	RCC_APB1ENR |= (1<<1) ;
}
void RCC_Enable_ADC01(void) {
	RCC_APB2ENR |= (1<<9) ;
}
void RCC_Enable_UART1(void) {
	RCC_APB2ENR |= (1<<14) ;
}
void RCC_Enable_SPI01(void) {
	RCC_APB2ENR |= (1<<12) ;
}
void RCC_Enable_I2C01(void) {
	RCC_APB1ENR |= (1<<21) ;
}
