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
#define DISPLAYTIME 0x000F
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
float readDust(void);
void displayDust(float dustVal);
void changeFloatROM(unsigned int i, uint8_t *ROM);
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
	/* USER CODE END 2 */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {
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
 * @功能简介 : 读取PM2.5传感器 GP2Y1010AU
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
	DustVal = 0.17 * dustAD * 3300 / 4095 - 0.1;
	return DustVal;
}

void displayDust(float dustVal) {
	/* 画面1-历史控件12-通道1-float型 */
	/* EE B1 60 00 01 00 0C 41 20 00 00 FF FC FF FF */
	uint8_t sendCMD[15] = { 0xEE, 0xB1, 0x60, 0x00, 0x01, 0x00, 0x0C, 0x41,
			0x20, 0x00, 0x00, 0xFF, 0xFC, 0xFF, 0xFF };
	memcpy((&sendCMD[7]), dustVal, 8);
	changeFloatROM(7, sendCMD);
	HAL_UART_Transmit(&huart1, &sendCMD, 15, DISPLAYTIME);
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
