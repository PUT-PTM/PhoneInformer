#include "tm_stm32f4_pcd8544.h"
#include "ringtone.h"
#include "Nokia_Sms.h"
#include "bells.h"

char sign;
char number[13];
unsigned int j = 0;
unsigned int d = 0;
uint8_t is_calling = 0;
uint8_t sms_came = 0;
uint8_t sms_ringtone=0;
uint8_t bell_image=0;
void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		sign = USART1->DR;
		if(sign == 's') // jesli przyjdzie SMS
		{
			sms_came = 1;
			sms_ringtone = 1;
		}
		if(!sms_came) //polaczenie, zczytywanie kolejnych cyfr numberu
		{
			number[j] = sign;
			if(j == 11)
			{
				j = 0;
				is_calling = 1;
			}
			else if(sign != 'e') j++;
			if(sign == 'e')
			{
				is_calling = 0;
				PCD8544_Clear();
				PCD8544_GotoXY(0, 14);
				PCD8544_Puts("PHONE INFORMER" , PCD8544_Pixel_Set, PCD8544_FontSize_5x7);
				PCD8544_Refresh();
			}
			if(is_calling)
			{

				PCD8544_Clear();
				PCD8544_GotoXY(0, 14);
				PCD8544_Puts(number, PCD8544_Pixel_Set, PCD8544_FontSize_5x7);
				PCD8544_GotoXY(0, 28);
				PCD8544_Puts("is calling" , PCD8544_Pixel_Set, PCD8544_FontSize_5x7);
				PCD8544_Refresh();
			}
		}
		else  // SMS
		{
			if(sign != 's') //pomija znacznik ze przyszedl SMS i zczytuje kolejne cyfry numberu nadawcy
			{
				number[j] = sign;
				if(j == 11)
				{
					j = 0;
					sms_came = 0;
				}
				else j++;
				if(!sms_came)
				{
					PCD8544_Clear();
					PCD8544_GotoXY(0, 7);
					PCD8544_Puts("Dostales SMS", PCD8544_Pixel_Set, PCD8544_FontSize_5x7);
					PCD8544_GotoXY(0, 28);
					PCD8544_Puts(number, PCD8544_Pixel_Set, PCD8544_FontSize_5x7);
					PCD8544_Refresh();
				}
			}
		}
	}
}
void TIM3_IRQHandler(void) //dzwonek
{
         	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
         	{
         		if(is_calling == 1)
         		{
         			DAC_Cmd(DAC_Channel_1, ENABLE);
         			DAC_SetChannel1Data(DAC_Align_12b_R, ringtone[d]*2); //dzwonek polaczenia
         			d++;
         			if(d > 105677) d = 0;
         		}
         		else if(sms_ringtone == 1)
         		{
         			DAC_Cmd(DAC_Channel_1, ENABLE);
         			DAC_SetChannel1Data(DAC_Align_12b_R, nokia_sms[d]*1.5); // dzwonek sms
         			d++;
         			if(d > 73661)
         				{
         					sms_ringtone=0;
         					d = 0;
         				}//rs16 -> 3v
         		}
         		else
         		{
         			d = 0;
         			DAC_Cmd(DAC_Channel_1, DISABLE);
         		}
                	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
         	}
}
void TIM2_IRQHandler(void) //animacje
{
         	if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
         	{
         		if(is_calling)
         		{
         			switch(bell_image)
         			{
         			case 0: Draw_Bell(bell); bell_image++; break;
         			case 1: Draw_Bell(bell_left); bell_image++; break;
         			case 2: Draw_Bell(bell_right); bell_image=0; break;
         			}
         		}
         		else if(sms_came)
         		{

         		}
         		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
         	}
}
void EXTI0_IRQHandler(void)
{
         	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
         	{
         		PCD8544_Clear();
         		PCD8544_GotoXY(0, 14);
         		PCD8544_Puts("PHONE INFORMER" , PCD8544_Pixel_Set, PCD8544_FontSize_5x7);
         		PCD8544_Refresh();
         		EXTI_ClearITPendingBit(EXTI_Line0);
         	}
}

void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

  //inicjalizacja wyjœcia DAC
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  EXTI_InitTypeDef EXTI_InitStructure;
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
}
void USART_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(USART1, &USART_InitStructure);
  USART_Cmd(USART1, ENABLE);
}
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  NVIC_EnableIRQ(USART1_IRQn);

  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
  TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
void DAC_Configuration(void)
{
	DAC_InitTypeDef DAC_InitStructure;
	DAC_InitStructure.DAC_Trigger = DAC_Trigger_None;
	DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;
	DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
	DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	DAC_Init(DAC_Channel_1, &DAC_InitStructure);
	DAC_Cmd(DAC_Channel_1, ENABLE);
}
void TIM3_Configuration(void) // DAC
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 104;
	TIM_TimeBaseStructure.TIM_Prescaler = 49;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode =  TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	TIM_Cmd(TIM3, ENABLE);
}
void TIM2_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_TimeBaseStructure.TIM_Period = 6999;
	TIM_TimeBaseStructure.TIM_Prescaler = 2999;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStructure.TIM_CounterMode =  TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	TIM_Cmd(TIM2, ENABLE);
}
int main(void)
{
	SystemInit();
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE); // DAC
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_Configuration();
    GPIO_Configuration();
    USART_Configuration();
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	DAC_Configuration();
	TIM3_Configuration();
	TIM2_Configuration();

	PCD8544_Init(0x25);
	PCD8544_Clear();
	PCD8544_GotoXY(0, 14);
	PCD8544_Puts("PHONE INFORMER" , PCD8544_Pixel_Set, PCD8544_FontSize_5x7);
	PCD8544_Refresh();
	number[12]='\0';
    while(1){}
}
