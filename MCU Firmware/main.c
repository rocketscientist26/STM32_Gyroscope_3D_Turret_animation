#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_i2c.h"
#include "stm32f103rbt6.h"
#include "HMC5883L.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define L3G4200D_CTRL_REG1 0x20
#define L3G4200D_CTRL_REG4 0x23
#define L3G4200D_CTRL_REG5 0x24
#define L3G4200D_OUT_X_L 0x28

int16_t gX = 0;
int16_t gY = 0;
int16_t gZ = 0;

float gXdeg = 0;
float gYdeg = 0;
float gZdeg = 0;

uint8_t gyrobuf[7] = {0,0,0,0,0,0};
char buffer[40];
uint8_t stbt[1] = {0x00};
int dstat = 0;

void init_uart(){
	//USART1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate = 230400;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	NVIC_EnableIRQ(USART1_IRQn);
}
void init_i2c(){
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitTypeDef  GPIO_InitStructure;
	GPIO_PinRemapConfig(GPIO_Remap_I2C1,ENABLE);
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	I2C_InitTypeDef  I2C_InitStructure;
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x00;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Disable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = 50000;
	I2C_Cmd(I2C1, ENABLE);
	I2C_Init(I2C1, &I2C_InitStructure);
}
void print(uint8_t *array, uint16_t length){
	uint16_t i = 0;
	while (i != length){
		USART_SendData(USART1, array[i]);
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET){}
		i++;
	}
}
void read_gyro(){
	HMC5883L_I2C_BufferRead(211, gyrobuf, L3G4200D_OUT_X_L|0x80, 7);
	gX = (((uint16_t)gyrobuf[1]) << 8) | gyrobuf[0];
	gY = (((uint16_t)gyrobuf[3]) << 8) | gyrobuf[2];
	gZ = (((uint16_t)gyrobuf[5]) << 8) | gyrobuf[4];
}
int Gyro_GetReadyStatus(){
	stbt[0] = 0;
	HMC5883L_I2C_BufferRead(211, stbt, 0x27, 1);
	int p = ((stbt[0] >> 3) & 1);
	return p;
}
void init_l3g4200d(){
	uint8_t tmp = 0x0F;
	HMC5883L_I2C_ByteWrite(210, &tmp, L3G4200D_CTRL_REG1);
	tmp = 0x80;
	HMC5883L_I2C_ByteWrite(210, &tmp, L3G4200D_CTRL_REG4);
	tmp = 0x80;
	HMC5883L_I2C_ByteWrite(210, &tmp, L3G4200D_CTRL_REG5);
}

void USART1_IRQHandler(void){
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET){
		if (USART_ReceiveData(USART1)=='R'){
			dstat = 1;
		}
		else if (USART_ReceiveData(USART1)=='T'){
			dstat = 2;
		}
	}
}

void main(){
	init_uart();
	init_i2c();
	init_l3g4200d();

	while(1){
		while(Gyro_GetReadyStatus() != 1){}
		read_gyro();
		if (dstat == 2){
			gXdeg = 0;
			gZdeg = 0;
			gYdeg = 0;
			dstat = 0;
		}
		gXdeg += gX * 0.00875 * 0.01052631;
		while (gXdeg<0){gXdeg +=360;}
		while (gXdeg>360){gXdeg -=360;}
		gZdeg += gZ * 0.00875 * 0.01052631;
		while (gZdeg<0){gZdeg +=360;}
		while (gZdeg>360){gZdeg -=360;}
		gYdeg += gY * 0.00875 * 0.01052631;
		while (gYdeg<0){gYdeg +=360;}
		while (gYdeg>360){gYdeg -=360;}
		if (dstat == 1){
			print("S ", 2);
			sprintf(buffer, "X=%.3d ",(int16_t)gXdeg);
			print(buffer, 6);
			sprintf(buffer, "Z=%.3d ",(int16_t)gZdeg);
			print(buffer, 6);
			sprintf(buffer, "Y=%.3d ",(int16_t)gYdeg);
			print(buffer, 6);
			print("E\n", 2);
			dstat = 0;
		}
	}
}
