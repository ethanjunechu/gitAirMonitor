/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* printf重定向 */
//#ifdef __GNUC__
//#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
//#else
//#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
//#endif /* __GNUC__ */
//PUTCHAR_PROTOTYPE {
//	HAL_UART_Transmit(&huart1, (uint8_t*) &ch, 1, 0x000F);
//	return ch;
//}
/* 屏幕发送间隔 */
#define DISPLAYTIME 0xFFFF
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
int delayTime = 280;
int delayTime2 = 40;
float offTime = 9680;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void delay_us(uint16_t us);
void selfTest(void);
float readHCHO(void);
void displayHCHO(float HCHOVal);
float readTVOC(void);
void displayTVOC(float TVOCVal);
float readeCO2(void);
void displayeCO2(float eCO2Val);
float readDust(void);
void displayDust(float dustVal);
void enableScreenSampling(void);
void disableScreenSampling(void);
void changeFloatROM(unsigned int i, uint8_t *ROM);
unsigned char CheckSum(unsigned char *i, unsigned char ln);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_ADC1_Init();
	MX_I2C1_Init();
	MX_TIM4_Init();
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	/* USER CODE BEGIN 2 */
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
	/* 启动AD转换并使能DMA传输和中断 */
	HAL_ADCEx_Calibration_Start(&hadc1);
	disableScreenSampling();
	/* 开机第一次读取所有传感器 */
	displayHCHO(readHCHO());
	displayTVOC(readTVOC());
	displayeCO2(readeCO2());
	displayDust(readDust());
	enableScreenSampling();
	selfTest();
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
		displayHCHO(readHCHO());
		displayTVOC(readTVOC());
		displayeCO2(readeCO2());
		displayDust(readDust());
		/* USER CODE END WHILE */

		/* USER CODE BEGIN 3 */
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
	PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
		Error_Handler();
	}
}

/* USER CODE BEGIN 4 */
/**
 * @功能简介 : 延时N us
 * @入口参数 : 无
 * @出口参数 : 无
 * @历史版本 : V0.0.1 - Ethan - 2019/12/16
 */
void delay_us(uint16_t us) {
	uint16_t differ = 0xffff - us - 5;
	/*为防止因中断打断延时，造成计数错误.
	 如从0xfffE开始延时1us,但由于中断打断
	 （此时计数器仍在计数），本因计数至0xffff）
	 便停止计数，但由于错过计数值，并重载arr值，
	 导致实际延时(0xffff+1)us
	 */

	HAL_TIM_Base_Start(&htim4);

	__HAL_TIM_SET_COUNTER(&htim4, differ);

	while (differ < 0xffff - 5) {
		differ = __HAL_TIM_GET_COUNTER(&htim4);
	}

	HAL_TIM_Base_Stop(&htim4);
}
/**
 * @功能简介 : 开机自检，显示自检结果和进度条
 * @入口参数 : 无
 * @出口参数 : 无
 * @历史版本 : V0.0.1 - Ethan - 2019/12/16
 */
void selfTest(void) {
	/* 进度条显示 - 100% */
	/* EE B1 10 00 00 00 01 00 00 00 64 FF FC FF FF  */
	uint8_t sendPGCMD[15] = { 0xEE, 0xB1, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00,
			0x00, 0x00, 100, 0xFF, 0xFC, 0xFF, 0xFF };
	/* 跳转主界面 */
	uint8_t sendTurnMainPageCMD[9] = { 0xEE, 0xB1, 0x00, 0x00, 0x01, 0xFF, 0xFC,
			0xFF, 0xFF };
	/* 数据记录控件添加记录 - 核心MCU启动中      ......   1% */
	/* EE B1 52 00 00 00 02 BA CB D0 C4 4D 43 55 C6 F4 B6 AF D6 D0 20 20 20 20 20 20 2E 2E 2E 2E 2E 2E 20 20 20 31 25 FF FC FF FF  */
	/* EE B1 57 00 00 00 02 00 00 BA CB D0 C4 4D 43 55 C6 F4 B6 AF D6 D0 20 20 20 20 20 20 2E 2E 2E 2E 2E 2E 20 20 20 31 25 FF FC FF FF */
	uint8_t sendAddRECCMD[41] = { 0xEE, 0xB1, 0x52, 0x00, 0x00, 0x00, 0x02,
			0xBA, 0xCB, 0xD0, 0xC4, 0x4D, 0x43, 0x55, 0xC6, 0xF4, 0xB6, 0xAF,
			0xD6, 0xD0, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2E, 0x2E, 0x2E,
			0x2E, 0x2E, 0x2E, 0x20, 0x20, 0x20, 0x31, 0x25, 0xFF, 0xFC, 0xFF,
			0xFF };
	/* 数据记录控件修改记录 - 核心MCU启动中      ......   1% */
	/* EE B1 57 00 00 00 02 00 00 BA CB D0 C4 4D 43 55 C6 F4 B6 AF D6 D0 20 20 20 20 20 20 2E 2E 2E 2E 2E 2E 20 20 20 31 25 FF FC FF FF */
	uint8_t sendEditRECCMD[43] = { 0xEE, 0xB1, 0x57, 0x00, 0x00, 0x00, 0x02,
			0x00, 0x00, 0xBA, 0xCB, 0xD0, 0xC4, 0x4D, 0x43, 0x55, 0xC6, 0xF4,
			0xB6, 0xAF, 0xD6, 0xD0, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x2E,
			0x2E, 0x2E, 0x2E, 0x2E, 0x2E, 0x20, 0x20, 0x20, 0x31, 0x25, 0xFF,
			0xFC, 0xFF, 0xFF };
	uint8_t i = 0;
	/* 更新进度条 */
	sendPGCMD[10] = 0;
	HAL_UART_Transmit(&huart1, sendPGCMD, 15, DISPLAYTIME);
	HAL_UART_Transmit(&huart1, sendAddRECCMD, 41, DISPLAYTIME);
	HAL_Delay(20);
	for (i = 0; i < 99; i++) {
		/* 数据记录控件修改记录 - 核心MCU启动中      ......   1% */
		sendEditRECCMD[37] += 1;
		if (sendEditRECCMD[37] > 0x39) {
			sendEditRECCMD[37] = 0x30;
			sendEditRECCMD[36] = 0x31 + (i / 10);
			if (sendEditRECCMD[36] > 0x39) {
				sendEditRECCMD[36] = 0x30;
				sendEditRECCMD[35] = 0x31 + (i / 100);
				sendEditRECCMD[20] = 0xCD;
				sendEditRECCMD[21] = 0xEA;
				sendEditRECCMD[22] = 0xB3;
				sendEditRECCMD[23] = 0xC9;
			}
		}
		sendPGCMD[10] = i / 5;
		HAL_UART_Transmit(&huart1, sendPGCMD, 15, DISPLAYTIME);
		HAL_UART_Transmit(&huart1, sendEditRECCMD, 43, DISPLAYTIME);
		HAL_Delay(20);
	}
	/* 数据记录控件添加记录 - 甲醛传感器启动中      ......   1% */
	sendAddRECCMD[7] = 0xBC;
	sendAddRECCMD[8] = 0xD7;
	sendAddRECCMD[9] = 0xC8;
	sendAddRECCMD[10] = 0xA9;
	sendAddRECCMD[11] = 0xB4;
	sendAddRECCMD[12] = 0xAB;
	sendAddRECCMD[13] = 0xB8;
	sendAddRECCMD[14] = 0xD0;
	sendAddRECCMD[15] = 0xC6;
	sendAddRECCMD[16] = 0xF7;
	sendAddRECCMD[17] = 0xC6;
	sendAddRECCMD[18] = 0xF4;
	sendAddRECCMD[19] = 0xB6;
	sendAddRECCMD[20] = 0xAF;
	sendAddRECCMD[21] = 0xD6;
	sendAddRECCMD[22] = 0xD0;
	HAL_UART_Transmit(&huart1, sendAddRECCMD, 41, DISPLAYTIME);
	/* sendEditRECCMD[8] - 修改第几条记录 */
	sendEditRECCMD[8] = 0x01;
	sendEditRECCMD[9] = 0xBC;
	sendEditRECCMD[10] = 0xD7;
	sendEditRECCMD[11] = 0xC8;
	sendEditRECCMD[12] = 0xA9;
	sendEditRECCMD[13] = 0xB4;
	sendEditRECCMD[14] = 0xAB;
	sendEditRECCMD[15] = 0xB8;
	sendEditRECCMD[16] = 0xD0;
	sendEditRECCMD[17] = 0xC6;
	sendEditRECCMD[18] = 0xF7;
	sendEditRECCMD[19] = 0xC6;
	sendEditRECCMD[20] = 0xF4;
	sendEditRECCMD[21] = 0xB6;
	sendEditRECCMD[22] = 0xAF;
	sendEditRECCMD[23] = 0xD6;
	sendEditRECCMD[24] = 0xD0;
	sendEditRECCMD[35] = 0x20;
	sendEditRECCMD[36] = 0x20;
	sendEditRECCMD[37] = 0x31;

	for (i = 0; i < 99; i++) {
		/* 数据记录控件修改记录 - 甲醛传感器启动中      ......   1% */
		sendEditRECCMD[37] += 1;
		if (sendEditRECCMD[37] > 0x39) {
			sendEditRECCMD[37] = 0x30;
			sendEditRECCMD[36] = 0x31 + (i / 10);
			if (sendEditRECCMD[36] > 0x39) {
				sendEditRECCMD[36] = 0x30;
				sendEditRECCMD[35] = 0x31 + (i / 100);
				sendEditRECCMD[23] = 0xCD;
				sendEditRECCMD[24] = 0xEA;
				sendEditRECCMD[25] = 0xB3;
				sendEditRECCMD[26] = 0xC9;
			}
		}
		sendPGCMD[10] = 20 + i / 5;
		HAL_UART_Transmit(&huart1, sendPGCMD, 15, DISPLAYTIME);
		HAL_UART_Transmit(&huart1, sendEditRECCMD, 43, DISPLAYTIME);
		HAL_Delay(20);
	}
	/* 数据记录控件添加记录 - TVOC传感器启动中      ......   1% */
	sendAddRECCMD[7] = 0x54;
	sendAddRECCMD[8] = 0x56;
	sendAddRECCMD[9] = 0x4F;
	sendAddRECCMD[10] = 0x43;
	sendAddRECCMD[11] = 0xB4;
	sendAddRECCMD[12] = 0xAB;
	sendAddRECCMD[13] = 0xB8;
	sendAddRECCMD[14] = 0xD0;
	sendAddRECCMD[15] = 0xC6;
	sendAddRECCMD[16] = 0xF7;
	sendAddRECCMD[17] = 0xC6;
	sendAddRECCMD[18] = 0xF4;
	sendAddRECCMD[19] = 0xB6;
	sendAddRECCMD[20] = 0xAF;
	sendAddRECCMD[21] = 0xD6;
	sendAddRECCMD[22] = 0xD0;
	sendEditRECCMD[25] = 0x20;
	sendEditRECCMD[26] = 0x20;
	HAL_UART_Transmit(&huart1, sendAddRECCMD, 41, DISPLAYTIME);
	/* sendEditRECCMD[8] - 修改第几条记录 */
	sendEditRECCMD[8] = 0x02;
	sendEditRECCMD[9] = 0x54;
	sendEditRECCMD[10] = 0x56;
	sendEditRECCMD[11] = 0x4F;
	sendEditRECCMD[12] = 0x43;
	sendEditRECCMD[13] = 0xB4;
	sendEditRECCMD[14] = 0xAB;
	sendEditRECCMD[15] = 0xB8;
	sendEditRECCMD[16] = 0xD0;
	sendEditRECCMD[17] = 0xC6;
	sendEditRECCMD[18] = 0xF7;
	sendEditRECCMD[19] = 0xC6;
	sendEditRECCMD[20] = 0xF4;
	sendEditRECCMD[21] = 0xB6;
	sendEditRECCMD[22] = 0xAF;
	sendEditRECCMD[23] = 0xD6;
	sendEditRECCMD[24] = 0xD0;
	sendEditRECCMD[25] = 0x20;
	sendEditRECCMD[26] = 0x20;
	sendEditRECCMD[35] = 0x20;
	sendEditRECCMD[36] = 0x20;
	sendEditRECCMD[37] = 0x31;

	for (i = 0; i < 99; i++) {
		/* 数据记录控件修改记录 - TVOC传感器启动中      ......   1% */
		sendEditRECCMD[37] += 1;
		if (sendEditRECCMD[37] > 0x39) {
			sendEditRECCMD[37] = 0x30;
			sendEditRECCMD[36] = 0x31 + (i / 10);
			if (sendEditRECCMD[36] > 0x39) {
				sendEditRECCMD[36] = 0x30;
				sendEditRECCMD[35] = 0x31 + (i / 100);
				sendEditRECCMD[23] = 0xCD;
				sendEditRECCMD[24] = 0xEA;
				sendEditRECCMD[25] = 0xB3;
				sendEditRECCMD[26] = 0xC9;
			}
		}
		sendPGCMD[10] = 40 + i / 5;
		HAL_UART_Transmit(&huart1, sendPGCMD, 15, DISPLAYTIME);
		HAL_UART_Transmit(&huart1, sendEditRECCMD, 43, DISPLAYTIME);
		HAL_Delay(20);
	}
	/* 数据记录控件添加记录 - eCO2传感器启动中      ......   1% */
	sendAddRECCMD[7] = 0x65;
	sendAddRECCMD[8] = 0x43;
	sendAddRECCMD[9] = 0x4F;
	sendAddRECCMD[10] = 0x32;
	sendAddRECCMD[11] = 0xB4;
	sendAddRECCMD[12] = 0xAB;
	sendAddRECCMD[13] = 0xB8;
	sendAddRECCMD[14] = 0xD0;
	sendAddRECCMD[15] = 0xC6;
	sendAddRECCMD[16] = 0xF7;
	sendAddRECCMD[17] = 0xC6;
	sendAddRECCMD[18] = 0xF4;
	sendAddRECCMD[19] = 0xB6;
	sendAddRECCMD[20] = 0xAF;
	sendAddRECCMD[21] = 0xD6;
	sendAddRECCMD[22] = 0xD0;
	sendEditRECCMD[25] = 0x20;
	sendEditRECCMD[26] = 0x20;
	HAL_UART_Transmit(&huart1, sendAddRECCMD, 41, DISPLAYTIME);
	/* sendEditRECCMD[8] - 修改第几条记录 */
	sendEditRECCMD[8] = 0x03;
	sendEditRECCMD[9] = 0x65;
	sendEditRECCMD[10] = 0x43;
	sendEditRECCMD[11] = 0x4F;
	sendEditRECCMD[12] = 0x32;
	sendEditRECCMD[13] = 0xB4;
	sendEditRECCMD[14] = 0xAB;
	sendEditRECCMD[15] = 0xB8;
	sendEditRECCMD[16] = 0xD0;
	sendEditRECCMD[17] = 0xC6;
	sendEditRECCMD[18] = 0xF7;
	sendEditRECCMD[19] = 0xC6;
	sendEditRECCMD[20] = 0xF4;
	sendEditRECCMD[21] = 0xB6;
	sendEditRECCMD[22] = 0xAF;
	sendEditRECCMD[23] = 0xD6;
	sendEditRECCMD[24] = 0xD0;
	sendEditRECCMD[25] = 0x20;
	sendEditRECCMD[26] = 0x20;
	sendEditRECCMD[35] = 0x20;
	sendEditRECCMD[36] = 0x20;
	sendEditRECCMD[37] = 0x31;

	for (i = 0; i < 99; i++) {
		/* 数据记录控件修改记录 - eCO2传感器启动中      ......   1% */
		sendEditRECCMD[37] += 1;
		if (sendEditRECCMD[37] > 0x39) {
			sendEditRECCMD[37] = 0x30;
			sendEditRECCMD[36] = 0x31 + (i / 10);
			if (sendEditRECCMD[36] > 0x39) {
				sendEditRECCMD[36] = 0x30;
				sendEditRECCMD[35] = 0x31 + (i / 100);
				sendEditRECCMD[23] = 0xCD;
				sendEditRECCMD[24] = 0xEA;
				sendEditRECCMD[25] = 0xB3;
				sendEditRECCMD[26] = 0xC9;
			}
		}
		sendPGCMD[10] = 60 + i / 5;
		HAL_UART_Transmit(&huart1, sendPGCMD, 15, DISPLAYTIME);
		HAL_UART_Transmit(&huart1, sendEditRECCMD, 43, DISPLAYTIME);
		HAL_Delay(20);
	}
	/* 数据记录控件添加记录 - PM2.5传感器启动中      ......   1% */
	sendAddRECCMD[7] = 0x50;
	sendAddRECCMD[8] = 0x4D;
	sendAddRECCMD[9] = 0x32;
	sendAddRECCMD[10] = 0x2E;
	sendAddRECCMD[11] = 0x35;
	sendAddRECCMD[12] = 0xB4;
	sendAddRECCMD[13] = 0xAB;
	sendAddRECCMD[14] = 0xB8;
	sendAddRECCMD[15] = 0xD0;
	sendAddRECCMD[16] = 0xC6;
	sendAddRECCMD[17] = 0xF7;
	sendAddRECCMD[18] = 0xC6;
	sendAddRECCMD[19] = 0xF4;
	sendAddRECCMD[20] = 0xB6;
	sendAddRECCMD[21] = 0xAF;
	sendAddRECCMD[22] = 0xD6;
	sendAddRECCMD[23] = 0xD0;
	sendEditRECCMD[25] = 0x20;
	sendEditRECCMD[26] = 0x20;
	HAL_UART_Transmit(&huart1, sendAddRECCMD, 41, DISPLAYTIME);
	/* sendEditRECCMD[8] - 修改第几条记录 */
	sendEditRECCMD[8] = 0x04;
	sendEditRECCMD[9] = 0x50;
	sendEditRECCMD[10] = 0x4D;
	sendEditRECCMD[11] = 0x32;
	sendEditRECCMD[12] = 0x2E;
	sendEditRECCMD[13] = 0x35;
	sendEditRECCMD[14] = 0xB4;
	sendEditRECCMD[15] = 0xAB;
	sendEditRECCMD[16] = 0xB8;
	sendEditRECCMD[17] = 0xD0;
	sendEditRECCMD[18] = 0xC6;
	sendEditRECCMD[19] = 0xF7;
	sendEditRECCMD[20] = 0xC6;
	sendEditRECCMD[21] = 0xF4;
	sendEditRECCMD[22] = 0xB6;
	sendEditRECCMD[23] = 0xAF;
	sendEditRECCMD[24] = 0xD6;
	sendEditRECCMD[25] = 0xD0;
	sendEditRECCMD[26] = 0x20;
	sendEditRECCMD[35] = 0x20;
	sendEditRECCMD[36] = 0x20;
	sendEditRECCMD[37] = 0x31;

	for (i = 0; i < 99; i++) {
		/* 数据记录控件修改记录 - PM2.5传感器启动中      ......   1% */
		sendEditRECCMD[37] += 1;
		if (sendEditRECCMD[37] > 0x39) {
			sendEditRECCMD[37] = 0x30;
			sendEditRECCMD[36] = 0x31 + (i / 10);
			if (sendEditRECCMD[36] > 0x39) {
				sendEditRECCMD[36] = 0x30;
				sendEditRECCMD[35] = 0x31 + (i / 100);
				sendEditRECCMD[24] = 0xCD;
				sendEditRECCMD[25] = 0xEA;
				sendEditRECCMD[26] = 0xB3;
				sendEditRECCMD[27] = 0xC9;
			}
		}
		sendPGCMD[10] = 80 + i / 5;
		HAL_UART_Transmit(&huart1, sendPGCMD, 15, DISPLAYTIME);
		HAL_UART_Transmit(&huart1, sendEditRECCMD, 43, DISPLAYTIME);
		HAL_Delay(20);
	}
	sendPGCMD[10] = 100;
	HAL_UART_Transmit(&huart1, sendPGCMD, 15, DISPLAYTIME);
	HAL_Delay(500);
	HAL_UART_Transmit(&huart1, sendTurnMainPageCMD, 9, DISPLAYTIME);
}

/**
 * @功能简介 : 读取甲醛传感器 WS-Z-K 量程：0-2ppm | 分辨率：0.001ppm | ppb=1/1000ppm
 * @入口参数 : 无
 * @出口参数 : float型 ppm
 * @历史版本 : V0.0.1 - Ethan - 2019/12/16
 */
float readHCHO(void) {
	uint32_t dustAD = 0;
	float HCHOVal = 0;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
	delay_us(delayTime);
	HAL_ADC_Start(&hadc1);

	delay_us(delayTime2);
	dustAD = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
	delay_us(offTime);
	HCHOVal = 0.17 * dustAD * 3300 / 4095 - 100;
	if (HCHOVal > 2) {
		HCHOVal = 2;
	}
	if (HCHOVal < 0) {
		HCHOVal = 0;
	}
	return HCHOVal;
}
/**
 * @功能简介 : 显示甲醛
 * @入口参数 : 无
 * @出口参数 : 无
 * @历史版本 : V0.0.1 - Ethan - 2019/12/16
 */
void displayHCHO(float HCHOVal) {
	/* 画面1-历史控件5-通道1-float型 */
	/* EE B1 60 00 01 00 0C 41 20 00 00 FF FC FF FF */
	uint8_t sendHistoryCMD[15] = { 0xEE, 0xB1, 0x60, 0x00, 0x01, 0x00, 0x05,
			0x41, 0x20, 0x00, 0x00, 0xFF, 0xFC, 0xFF, 0xFF };
	/* 画面1-文本控件6 */
	/* EE B1 10 00 01 00 04 30 2E 30 30 FF FC FF FF */
	uint8_t sendTextCMD[16] = { 0xEE, 0xB1, 0x10, 0x00, 0x01, 0x00, 0x06, 0x30,
			0x2E, 0x30, 0x30, 0x30, 0xFF, 0xFC, 0xFF, 0xFF };
	sendTextCMD[7] = HCHOVal;
	sendTextCMD[9] = (HCHOVal - sendTextCMD[7]) * 10;
	sendTextCMD[10] = (HCHOVal - sendTextCMD[7] - sendTextCMD[9] / 10) * 100;
	sendTextCMD[11] = (HCHOVal - sendTextCMD[7] - sendTextCMD[9] / 10
			- sendTextCMD[10] / 100) * 1000 + 0x30;
	sendTextCMD[10] += 0x30;
	sendTextCMD[9] += 0x30;
	sendTextCMD[7] += 0x30;
	HAL_UART_Transmit(&huart1, sendTextCMD, 16, DISPLAYTIME);
	memcpy((&sendHistoryCMD[7]), &HCHOVal, 4);
	changeFloatROM(7, sendHistoryCMD);
	HAL_UART_Transmit(&huart1, sendHistoryCMD, 15, DISPLAYTIME);
}

/**
 * @功能简介 : 读取TVOC传感器 CCS811 量程TVOC： 0-1000ppb | 分辨率：1ppb
 * @入口参数 : 无
 * @出口参数 : float型 ppm
 * @历史版本 : V0.0.1 - Ethan - 2019/12/16
 */
float readTVOC(void) {
	uint32_t dustAD = 0;
	float TVOCVal = 0;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
	delay_us(delayTime);
	HAL_ADC_Start(&hadc1);

	delay_us(delayTime2);
	dustAD = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
	delay_us(offTime);
	TVOCVal = 0.17 * dustAD * 3300 / 4095 - 100;
	if (TVOCVal > 1000) {
		TVOCVal = 1000;
	}
	if (TVOCVal < 0) {
		TVOCVal = 0;
	}
	return TVOCVal;
}
/**
 * @功能简介 : 显示TVOC
 * @入口参数 : 无
 * @出口参数 : 无
 * @历史版本 : V0.0.1 - Ethan - 2019/12/16
 */
void displayTVOC(float TVOCVal) {
	/* 画面1-历史控件22-通道1-float型 */
	/* EE B1 60 00 01 00 0C 41 20 00 00 FF FC FF FF */
	uint8_t sendHistoryCMD[15] = { 0xEE, 0xB1, 0x60, 0x00, 0x01, 0x00, 22, 0x41,
			0x20, 0x00, 0x00, 0xFF, 0xFC, 0xFF, 0xFF };
	/* 画面1-文本控件24 */
	/* EE B1 10 00 01 00 04 30 30 30 30 FF FC FF FF */
	uint8_t sendTextCMD[15] = { 0xEE, 0xB1, 0x10, 0x00, 0x01, 0x00, 24, 0x30,
			0x30, 0x30, 0x30, 0xFF, 0xFC, 0xFF, 0xFF };
	uint8_t i = 0;
	sendTextCMD[7] = TVOCVal / 1000;
	sendTextCMD[8] = (TVOCVal - sendTextCMD[7] * 1000) / 100;
	sendTextCMD[9] = (TVOCVal - sendTextCMD[7] * 1000 - sendTextCMD[8] * 100)
			/ 10;
	sendTextCMD[10] = TVOCVal - sendTextCMD[7] * 1000 - sendTextCMD[8] * 100
			- sendTextCMD[9] * 10 + 0x30;
	sendTextCMD[9] += 0x30;
	sendTextCMD[8] += 0x30;
	sendTextCMD[7] += 0x30;
	for (i = 0; i < 3; i++) {
		if (sendTextCMD[7] == 0x30) {
			sendTextCMD[7] = sendTextCMD[8];
			sendTextCMD[8] = sendTextCMD[9];
			sendTextCMD[9] = sendTextCMD[10];
			sendTextCMD[10] = 0x20;
		}
	}
	HAL_UART_Transmit(&huart1, sendTextCMD, 15, DISPLAYTIME);
	memcpy((&sendHistoryCMD[7]), &TVOCVal, 4);
	changeFloatROM(7, sendHistoryCMD);
	HAL_UART_Transmit(&huart1, sendHistoryCMD, 15, DISPLAYTIME);
}
/**
 * @功能简介 : 读取eCO2传感器 CCS811 量程eCO2： 400-5000ppm | 分辨率：1ppm
 * @入口参数 : 无
 * @出口参数 : float型 ppm
 * @历史版本 : V0.0.1 - Ethan - 2019/12/16
 */
float readeCO2(void) {
	uint32_t dustAD = 0;
	float eCO2Val = 0;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
	delay_us(delayTime);
	HAL_ADC_Start(&hadc1);

	delay_us(delayTime2);
	dustAD = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
	delay_us(offTime);
	eCO2Val = 0.17 * dustAD * 3300 / 4095 - 100;
	if (eCO2Val > 5000) {
		eCO2Val = 5000;
	}
	if (eCO2Val < 400) {
		eCO2Val = 400;
	}
	return eCO2Val;
}
/**
 * @功能简介 : 显示eCO2
 * @入口参数 : 无
 * @出口参数 : 无
 * @历史版本 : V0.0.1 - Ethan - 2019/12/16
 */
void displayeCO2(float eCO2Val) {
	/* 画面1-历史控件8-通道1-float型 */
	/* EE B1 60 00 01 00 0C 41 20 00 00 FF FC FF FF */
	uint8_t sendHistoryCMD[15] = { 0xEE, 0xB1, 0x60, 0x00, 0x01, 0x00, 8, 0x41,
			0x20, 0x00, 0x00, 0xFF, 0xFC, 0xFF, 0xFF };
	/* 画面1-文本控件20 */
	/* EE B1 10 00 01 00 04 30 30 30 30 FF FC FF FF */
	uint8_t sendTextCMD[15] = { 0xEE, 0xB1, 0x10, 0x00, 0x01, 0x00, 20, 0x30,
			0x30, 0x30, 0x30, 0xFF, 0xFC, 0xFF, 0xFF };
	uint8_t i = 0;
	sendTextCMD[7] = eCO2Val / 1000;
	sendTextCMD[8] = (eCO2Val - sendTextCMD[7] * 1000) / 100;
	sendTextCMD[9] = (eCO2Val - sendTextCMD[7] * 1000 - sendTextCMD[8] * 100)
			/ 10;
	sendTextCMD[10] = eCO2Val - sendTextCMD[7] * 1000 - sendTextCMD[8] * 100
			- sendTextCMD[9] * 10 + 0x30;
	sendTextCMD[9] += 0x30;
	sendTextCMD[8] += 0x30;
	sendTextCMD[7] += 0x30;
	for (i = 0; i < 3; i++) {
		if (sendTextCMD[7] == 0x30) {
			sendTextCMD[7] = sendTextCMD[8];
			sendTextCMD[8] = sendTextCMD[9];
			sendTextCMD[9] = sendTextCMD[10];
			sendTextCMD[10] = 0x20;
		}
	}
	HAL_UART_Transmit(&huart1, sendTextCMD, 15, DISPLAYTIME);
	memcpy((&sendHistoryCMD[7]), &eCO2Val, 4);
	changeFloatROM(7, sendHistoryCMD);
	HAL_UART_Transmit(&huart1, sendHistoryCMD, 15, DISPLAYTIME);
}
/**
 * @功能简介 : 读取PM2.5传感器 GP2Y1010AU 量程：0-500mg/m3
 * @入口参数 : 无
 * @出口参数 : float型 mg/m3
 * @历史版本 : V0.0.1 - Ethan - 2019/12/16
 */
float readDust(void) {
	uint32_t dustAD = 0;
	float DustVal = 0;
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);
	delay_us(delayTime);
	HAL_ADC_Start(&hadc1);

	delay_us(delayTime2);
	dustAD = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);
	delay_us(offTime);
	DustVal = 0.17 * dustAD * 3300 / 4095 - 100;
	if (DustVal > 500) {
		DustVal = 500;
	}
	if (DustVal < 0) {
		DustVal = 0;
	}
	return DustVal;
}
/**
 * @功能简介 : 显示PM2.5
 * @入口参数 : 无
 * @出口参数 : 无
 * @历史版本 : V0.0.1 - Ethan - 2019/12/16
 */
void displayDust(float dustVal) {
	/* 画面1-历史控件12-通道1-float型 */
	/* EE B1 60 00 01 00 0C 41 20 00 00 FF FC FF FF */
	uint8_t sendHistoryCMD[15] = { 0xEE, 0xB1, 0x60, 0x00, 0x01, 0x00, 0x0C,
			0x41, 0x20, 0x00, 0x00, 0xFF, 0xFC, 0xFF, 0xFF };
	/* 画面1-文本控件4 */
	/* EE B1 10 00 01 00 04 30 30 30 FF FC FF FF */
	uint8_t sendTextCMD[15] = { 0xEE, 0xB1, 0x10, 0x00, 0x01, 0x00, 0x04, 0x30,
			0x30, 0x30, 0x30, 0xFF, 0xFC, 0xFF, 0xFF };
	uint8_t i = 0;
	sendTextCMD[7] = dustVal / 100;
	sendTextCMD[8] = (dustVal - sendTextCMD[7] * 100) / 10;
	sendTextCMD[9] = dustVal - sendTextCMD[7] * 100 - sendTextCMD[8] * 10
			+ 0x30;
	sendTextCMD[10] = 0x20;
	sendTextCMD[8] += 0x30;
	sendTextCMD[7] += 0x30;
	for (i = 0; i < 2; i++) {
		if (sendTextCMD[7] == 0x30) {
			sendTextCMD[7] = sendTextCMD[8];
			sendTextCMD[8] = sendTextCMD[9];
			sendTextCMD[9] = sendTextCMD[10];
			sendTextCMD[10] = 0x20;
		}
	}
	HAL_UART_Transmit(&huart1, sendTextCMD, 15, DISPLAYTIME);
	memcpy((&sendHistoryCMD[7]), &dustVal, 4);
	changeFloatROM(7, sendHistoryCMD);
	HAL_UART_Transmit(&huart1, sendHistoryCMD, 15, DISPLAYTIME);
}

/**
 * @功能简介 : 开启屏幕采样
 * @入口参数 : 无
 * @出口参数 : 无
 * @历史版本 : V0.0.1 - Ethan - 2019/12/16
 */
void enableScreenSampling(void) {
	/* EE B1 61 00 01 00 05 01 FF FC FF FF */
	uint8_t sendCMD[12] = { 0xEE, 0xB1, 0x61, 0x00, 0x01, 0x00, 0x05, 0x01,
			0xFF, 0xFC, 0xFF, 0xFF };
	HAL_UART_Transmit(&huart1, sendCMD, 12, DISPLAYTIME);
	sendCMD[6] = 22;
	HAL_UART_Transmit(&huart1, sendCMD, 12, DISPLAYTIME);
	sendCMD[6] = 8;
	HAL_UART_Transmit(&huart1, sendCMD, 12, DISPLAYTIME);
	sendCMD[6] = 12;
	HAL_UART_Transmit(&huart1, sendCMD, 12, DISPLAYTIME);
}

/**
 * @功能简介 : 关闭屏幕采样
 * @入口参数 : 无
 * @出口参数 : 无
 * @历史版本 : V0.0.1 - Ethan - 2019/12/16
 */
void disableScreenSampling(void) {
	/* EE B1 61 00 01 00 05 00 FF FC FF FF */
	uint8_t sendCMD[12] = { 0xEE, 0xB1, 0x61, 0x00, 0x01, 0x00, 0x05, 0x00,
			0xFF, 0xFC, 0xFF, 0xFF };
	HAL_UART_Transmit(&huart1, sendCMD, 12, DISPLAYTIME);
	sendCMD[6] = 22;
	HAL_UART_Transmit(&huart1, sendCMD, 12, DISPLAYTIME);
	sendCMD[6] = 8;
	HAL_UART_Transmit(&huart1, sendCMD, 12, DISPLAYTIME);
	sendCMD[6] = 12;
	HAL_UART_Transmit(&huart1, sendCMD, 12, DISPLAYTIME);
}
/**
 * @功能简介 : 对换浮点数在数组中的大小端
 * @入口参数 : 数组中的位置, 数组
 * @出口参数 : 无
 * @历史版本 : V0.0.1 - Ethan - 2019/12/16
 */
void changeFloatROM(unsigned int i, uint8_t *ROM) {
	char temp = 0;

	temp = ROM[i + 3];
	ROM[i + 3] = ROM[i + 0];
	ROM[i + 0] = temp;

	temp = ROM[i + 2];
	ROM[i + 2] = ROM[i + 1];
	ROM[i + 1] = temp;
}

/**
 * @功能简介 : 求和校验(取发送、接收协议的 1\2\3\4\5\6\7 的和取反+1) - 将组数的元素 1-倒数第二个元素相加后取反+1(元素个数必须大于 2)
 * @入口参数 : 数组 ,数组长度
 * @出口参数 : 无
 * @历史版本 : V0.0.1 - Ethan - 2019/12/16
 */
unsigned char CheckSum(unsigned char *i, unsigned char ln) {
	unsigned char j, tempq = 0;
	i += 1;
	for (j = 0; j < (ln - 2); j++) {
		tempq += *i;
		i++;
	}
	tempq = (~tempq) + 1;
	return (tempq);
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
