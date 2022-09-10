#include "stm32f10x.h"                  // Device header
#include "stm32f10x_gpio.h"             // Keil::Device:StdPeriph Drivers:GPIO
#include <stdio.h> 


float mesafe=0;

void delay(uint32_t time){
	
	uint32_t newTime=time*24 ;//delayi bir cycle süresiyle degil mikrosaniyeyle yapabilmek için
	while(newTime--);
}

float HCSR04_Read(){ //mesafe okuma fonksiyonu
	uint32_t time=0;
	float tempDistance=0;
	
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
	delay(10); //data sheetteki bilgilere göre delay
	GPIO_SetBits(GPIOA,GPIO_Pin_1);
	delay(10);
	GPIO_ResetBits(GPIOA,GPIO_Pin_1);
	delay(10);
	
	while(!(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0))); //mesafe okunmayan zamani geçirmek için 
	
	while(GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0)){
		time++;
		delay(1);}
	
	tempDistance=(float)time*0.034; //cycle mesafe dönüsümü
	delay(10000);
	
	return tempDistance; }
	

void gpioConfig(){
	
	GPIO_InitTypeDef gpioStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	//trigger pin 
	gpioStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	gpioStructure.GPIO_Pin=GPIO_Pin_1;
	gpioStructure.GPIO_Speed=GPIO_Speed_50MHz; 
	GPIO_Init(GPIOA,&gpioStructure);
	
	//echo pin
	gpioStructure.GPIO_Mode=GPIO_Mode_IPD;
	gpioStructure.GPIO_Pin=GPIO_Pin_0;
	gpioStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&gpioStructure);
	
	
	//ledler
	gpioStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	gpioStructure.GPIO_Pin=GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 ; //sirasiyla sari-yesil-kirmizi
	gpioStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB , &gpioStructure);
}

void motorConfig(GPIO_TypeDef *port, uint16_t ileriPin , uint16_t geriPin){ //ileri geri döndürmek için L293D entegresi kullanildi
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	GPIO_InitTypeDef motorConfig;
	
	motorConfig.GPIO_Mode=GPIO_Mode_Out_PP;
	motorConfig.GPIO_Pin= ileriPin | geriPin;
	motorConfig.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(port, &motorConfig); }

	
void goForward(GPIO_TypeDef *port, uint16_t ileriPin, uint16_t geriPin) { //motorun saat yönünde dönmesi icin
	GPIO_SetBits(port, ileriPin);
	GPIO_ResetBits(port, geriPin);
	}
	
void goBackward(GPIO_TypeDef *port, uint16_t ileriPin, uint16_t geriPin) {	//motorun saat yonunun tersine donmesi icin
	GPIO_SetBits(port, geriPin);
	GPIO_ResetBits(port, ileriPin);
	}
void stop(GPIO_TypeDef *port, uint16_t ileriPin, uint16_t geriPin) { //motorun durmasi icin
	GPIO_ResetBits(port, geriPin);
	GPIO_ResetBits(port, ileriPin);
	
	}
	
	
int main(){
	gpioConfig();
	motorConfig(GPIOC, GPIO_Pin_0, GPIO_Pin_1); 
	
	while(1){
		mesafe=HCSR04_Read();
		if (mesafe>=550){
			GPIO_SetBits(GPIOB, GPIO_Pin_0); //yesil led yanar
			GPIO_ResetBits(GPIOB, GPIO_Pin_1 | GPIO_Pin_2 );//sari ve kirmizi led soner
			goForward(GPIOC, GPIO_Pin_0, GPIO_Pin_1); //motor saat yonunde doner
		}
		else if(mesafe>=450){
			GPIO_SetBits(GPIOB, GPIO_Pin_1); //sari led yanar
			GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_2 );
			stop(GPIOC, GPIO_Pin_0, GPIO_Pin_1); //motor durur
		}
		else {
			GPIO_SetBits(GPIOB, GPIO_Pin_2); //kirmizi led yanar
			GPIO_ResetBits(GPIOB, GPIO_Pin_0 | GPIO_Pin_1 );
			goBackward(GPIOC, GPIO_Pin_0, GPIO_Pin_1); //motor saat yonunun tersine doner 
		
		}
	}}
