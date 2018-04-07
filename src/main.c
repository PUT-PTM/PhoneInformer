#include "tm_stm32f4_pcd8544.h"
#include "ringtone.h"

int d=0;
int cyfra;
char numer[13];
int j = 0;
int dzwoni = 0;

void USART1_IRQHandler(void) //odbior dzwoniacego numeru
{
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{
		cyfra = USART1->DR;
		numer[j] = cyfra;
		if(j == 11)
		{
			j = 0;
			dzwoni = 1;
		}
		else if(cyfra != 'e') j++;
		if(cyfra == 'e')
		{
			dzwoni = 0;
			PCD8544_Clear();
			PCD8544_Refresh();
		}
		 if(dzwoni == 1)
		{

			PCD8544_Clear();
			PCD8544_GotoXY(0, 14);
			PCD8544_Puts(numer, PCD8544_Pixel_Set, PCD8544_FontSize_5x7);
			PCD8544_GotoXY(0, 28);
			PCD8544_Puts("dzwoni..." , PCD8544_Pixel_Set, PCD8544_FontSize_5x7);
			PCD8544_Refresh();
		}
	}
}
void TIM3_IRQHandler(void) //dzwonek
{
         	if(TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
         	{
         		if(dzwoni == 1)
         		{
         			DAC_Cmd(DAC_Channel_1, ENABLE);
         			DAC_SetChannel1Data(DAC_Align_12b_R, rawData[d]*1.5);
         			d++;
         			if(d > 105677) d = 0;
         		}
         		else
         		{
         			d = 0;
         			DAC_Cmd(DAC_Channel_1, DISABLE);
         		}
                	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
         	}
}
void GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;


  /* Configure USART1 Tx (PA.09) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  // ustawienie funkcji alternatywnej dla pinów (USART)
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);



  GPIO_InitStructure.GPIO_Pin =
  GPIO_Pin_12 | GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOD, &GPIO_InitStructure);


  //inicjalizacja wyjœcia DAC
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

}
void USART_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;
  USART_InitStructure.USART_BaudRate = 9600;		// Baud Rate
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


	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
}
int main(void)
{
	SystemInit();
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA , ENABLE); // DAC
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    NVIC_Configuration();
    GPIO_Configuration();
    USART_Configuration();
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	DAC_Configuration();
	TIM3_Configuration();
	PCD8544_Init(0x25); // LCD
	PCD8544_Write(PCD8544_LCD_TEMP, 0x01);
	PCD8544_Clear();
	PCD8544_GotoXY(0, 14);
	PCD8544_Puts("PHONE INFORMER" , PCD8544_Pixel_Set, PCD8544_FontSize_5x7);
	PCD8544_Refresh();
	numer[12]='\0';
    while(1)
    {

    }
}
