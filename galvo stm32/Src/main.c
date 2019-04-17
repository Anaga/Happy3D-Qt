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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdbool.h>// for bool type
#include <string.h> // for string
#include <math.h> 
#include <stdio.h>// sprintf 
#include <assert.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
DAC_HandleTypeDef hdac;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;

/* USER CODE BEGIN PV */
  double valVolt1 = 3.2;//voltage of dac1
	double valVolt2 = 2.2;//voltage of dac2
	uint32_t valByte1;//number of dac1
	uint32_t valByte2;//number of dac2
	bool laser_on = 0;
	float e = 10;  // distance betweeen two mirrors 10 mm
  float d = 200;  // distance between mirror and powder bed 200 mm
  int exptime = 100; // us
  int pointdis = 500; // um
  int hatchdis = 500; // um
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_DAC_Init(void);
/* USER CODE BEGIN PFP */
char myRxData[30];//uart received data
//char ch[4];// temp vx and vy
char myTxData[30]="Hello ";// for test
#define MAX_RECV_LEN 100 // maximum received data length
char msg_buff[MAX_RECV_LEN] = {0};// received data buffer
char *msg = msg_buff; //a pointer of data buffer
int flag = false; //receive finish flag
int len = 0; //received data length

char *data;
char *p;	
char *p1;
char *p2;
char *x1;
char *x2;
char *y1;
char *y2;
int fputc(int ch, FILE *f) // used for printf
{
    HAL_UART_Transmit_IT(&huart2, (uint8_t *)&ch, 1);
    HAL_Delay(1); 
    return 0;
}

// self atof
int is_digit(char ch)
{
	if(ch>='0'&&ch<='9')
		return 1;
	else
		return 0;
}
int is_space(char ch)
{
	if(ch==' ')
		return 1;
	else
		return 0;
}
double atof(char *s)
{
	double power,value;
	int i,sign;
	assert(s!=NULL);//?????????
	for(i=0;is_space(s[i]);i++);//?????????
	sign=(s[i]=='-')?-1:1;
	if(s[i]=='-'||s[i]=='+')//???????????
		i++;
	for(value=0.0;is_digit(s[i]);i++)//?????????
		value=value*10.0+(s[i]-'0');
	if(s[i]=='.')
		i++;
	for(power=1.0;is_digit(s[i]);i++)//?????????
	{
		value=value*10.0+(s[i]-'0');
		power*=10.0;
	}
	return sign*value/power;
}

void laserpoint(double x, double y) {
	double dx = atan(x / (e + sqrt(d * d + y * y))) * 180 / 3.1415926 / 2;  // degree of the mirror
	double dy = atan(y / d) * 180 / 3.1415926 / 2;  // degree of the mirror
	int  vx = 4096 * (dx * 5 / 30 / 3.3) ; // degree number of the mirror, 5V is galvo 30 degree   
	int  vy = 4096 * (dy * 5 / 30 / 3.3) ; // degree number of the mirror  

	
	printf("x = %f\r\n",x);
	//printf("vx = %d\r\n",vx);
	printf("y = %f\r\n",y);
	//printf("vx = %d\r\n",vy); 
	HAL_DAC_SetValue(&hdac,DAC1_CHANNEL_1, DAC_ALIGN_12B_R, vx);//set number of dac1
	HAL_DAC_SetValue(&hdac,DAC1_CHANNEL_2, DAC_ALIGN_12B_R, vy);//set number of dac1
}

void drawline(double x1, double y1, double x2, double y2) 
{
	int pointNum;
  pointNum = 	sqrt((x2 -x1) * (x2 - x1) +  (y2 -y1) * (y2 - y1)) * 10000 / pointdis;
	for (int i = 0; i <= pointNum; i++)
	{
		double x = x1 + i * (x2 - x1) / pointNum ;
		double y = y1 + i * (y2 - y1) / pointNum;
		
		laser_on = 1;
		laserpoint(x, y);
		//HAL_Delay(exptime);
		laser_on = 0;
	
	}
	
}

void drawcube(double x0, double y0, double dx, double dy) 
{
	for (int i = 0; i <= 1000 * dx / hatchdis; i++) {
		for (int j = 0; j <= 1000 * dy / pointdis; j++) {
			double fi = i;
			double fj = j;

			if (i % 2) {
				double x1 = x0 + fi * hatchdis / 1000;
				double y1 = y0 + fj * pointdis / 1000;
				laser_on = 1;;
				laserpoint(x1, y1);
				//HAL_Delay(5);
				laser_on = 0;;
			}
			else {
				double x1 = x0 + fi * hatchdis / 1000;
				double y1 = dy - fj * pointdis / 1000;
				laser_on = 1;
				laserpoint(x1, y1);
				//HAL_Delay(5);
				laser_on = 0;
			}
		}
	}
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
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
  MX_USART2_UART_Init();
  MX_DAC_Init();
  /* USER CODE BEGIN 2 */
	valByte1 = (uint32_t)((valVolt1 / 3.3)*4096);//number of dac1
	valByte2 = (uint32_t)((valVolt2 / 3.3)*4096);//number of dac2
  HAL_DAC_Start(&hdac,DAC_CHANNEL_1);//start of dac1
	HAL_DAC_Start(&hdac,DAC_CHANNEL_2);//start of dac1
	HAL_DAC_SetValue(&hdac,DAC1_CHANNEL_1, DAC_ALIGN_12B_R, 0);//set initional number of dac1
	HAL_DAC_SetValue(&hdac,DAC1_CHANNEL_2, DAC_ALIGN_12B_R, 0);//set initional number of dac2
	//HAL_UART_Receive_DMA(&huart2,(uint8_t *)myRxData,3);//uart received data saved to myRxData
	//HAL_UART_Receive_IT(&huart2,(uint8_t *)&myRxData,1);//???????????,?????????????????value?,??????????????????????????????????????
   HAL_UART_Receive_IT(&huart2, (uint8_t *)msg, 1); 
	/* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
			if(laser_on == 1) 
			{
				 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_SET);
			}
			else if(laser_on == 0) 
			{
				 HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_RESET);
			}
		
		
			if (flag == true) // get serial command  
			{ 
				printf("command received: %s length of command = %d\r\n",msg_buff,len-2); // print serial command  
				
				if(msg_buff[0] == '#')
				 {
				    // character array to string
						int i;
						data = (char *)malloc(len - 1);// release me
						for(i=0;i<len-2;i++)  
						{ 
								data[i] = msg_buff[i];
						}
						data[len] = '\0';
			      
						// first split by "="
						p = strtok(data,"=");
						if(p) p1 = p;
						p = strtok(NULL, "=");
						if(p) p2 = p;	
						if (strcmp(p1,"#line") == 0)
						{	
							  // second split by ","
								p = strtok(p2,",");
								if(p) x1 = p;
								p = strtok(NULL, ",");
								if(p) y1 = p;
								p = strtok(NULL, ",");
								if(p) x2 = p;
								p = strtok(NULL, ",");
								if(p) y2 = p;		
							
                // 	string to double						
								double x_1;
								double y_1;
								double x_2;
								double y_2;
								x_1 = atof(x1);
								y_1 = atof(y1);
								x_2 = atof(x2);
								y_2 = atof(y2);
								printf("x1 = %f y1 = %f\r\n",x_1,y_1);	
								printf("x2 = %f y2 = %f\r\n",x_2,y_2);
								drawline(x_1,y_1,x_2,y_2); // draw line
								printf("LINE_OK\r\n");
						}
						//free(data);
						
				  }
					else if(msg_buff[0] == 'a')
				  {
					
						drawcube(0,0,5,5);
						HAL_DAC_SetValue(&hdac,DAC1_CHANNEL_1, DAC_ALIGN_12B_R, valByte1);//set 2.2v number of dac1
						HAL_DAC_SetValue(&hdac,DAC1_CHANNEL_2, DAC_ALIGN_12B_R, valByte2);//set 2.2v number of dac1
					}
				  else if(msg_buff[0] == 'b')
				  {
						HAL_DAC_SetValue(&hdac,DAC1_CHANNEL_1, DAC_ALIGN_12B_R, 0);//set 0 number of dac1
						HAL_DAC_SetValue(&hdac,DAC1_CHANNEL_2, DAC_ALIGN_12B_R, 0);//set 0 number of dac1
					}

				 memset(msg_buff, 0, sizeof(msg_buff));   //clear the buffer
				 msg = msg_buff;			 // point to the head of buffer
				(&huart2)->pRxBuffPtr = (uint8_t *)msg;
				 flag = false;
				 len = 0;
					
			 }
			 HAL_Delay(10);

		
		
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage 
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief DAC Initialization Function
  * @param None
  * @retval None
  */
static void MX_DAC_Init(void)
{

  /* USER CODE BEGIN DAC_Init 0 */

  /* USER CODE END DAC_Init 0 */

  DAC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN DAC_Init 1 */

  /* USER CODE END DAC_Init 1 */
  /** DAC Initialization 
  */
  hdac.Instance = DAC;
  if (HAL_DAC_Init(&hdac) != HAL_OK)
  {
    Error_Handler();
  }
  /** DAC channel OUT1 config 
  */
  sConfig.DAC_Trigger = DAC_TRIGGER_NONE;
  sConfig.DAC_OutputBuffer = DAC_OUTPUTBUFFER_ENABLE;
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  /** DAC channel OUT2 config 
  */
  if (HAL_DAC_ConfigChannel(&hdac, &sConfig, DAC_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN DAC_Init 2 */

  /* USER CODE END DAC_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/** 
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void) 
{
  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET);

  /*Configure GPIO pin : PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : PC3 */
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
    uint8_t ret = HAL_OK;
    msg++;
	  len++;
    if( msg == msg_buff + MAX_RECV_LEN)
    {
        msg = msg_buff;
     }
     do
    {
        ret = HAL_UART_Receive_IT(&huart2,(uint8_t *)msg,1);
		}while(ret != HAL_OK);

    if(*(msg-1) == '\n')   // receive finish
     {
        flag  = true;
     }
    if(flag == true) 
		{
			
		}

	
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
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
