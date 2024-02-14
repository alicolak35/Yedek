/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
24.08_15.31:i==2 bloğndaki togle RESET konumundaki pini SET yapmıyo.
-toggle önüne ve ardına delay fonksiyonu koyunca while(1) döngüsü içinde gibi davranıyo
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal_exti.h"

GPIO_InitTypeDef Init;
EXTI_ConfigTypeDef EXTI_InitStr;
EXTI_HandleTypeDef EXTI_Handle;
int i = 0, sure = 1900;
//birinci adımda LEDler tek kesme ile yanıp sönüyo
//

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//GPIO_EXTI_Callback fonksiyonunda,her artan kesmede ledi yakıp söndürme tanımlandı

void GPIO_EXTI_Config(){
	HAL_EXTI_SetConfigLine(&EXTI_Handle, &EXTI_InitStr);

	EXTI_InitStr.Line = EXTI_LINE_0;
	EXTI_InitStr.Mode = EXTI_MODE_INTERRUPT;
	EXTI_InitStr.Trigger = EXTI_TRIGGER_RISING;
	EXTI_InitStr.GPIOSel = EXTI_GPIOA;

	HAL_EXTI_GetConfigLine(&EXTI_Handle, &EXTI_InitStr);

}

/*
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if (GPIO_Pin == GPIO_PIN_0){
		//uint32_t idr;
		//idr = GPIOA->IDR;
		if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_0) != 0){
			delay(200);
		}
		i++;
		sure = sure - 400;
		if (i == 4){//ark olmasına rağmen baştan başladı
			i = 0;
			sure = 1900;
		}
//exti config ne işe yarıyo?:A portunun P0 pininin kesme işlevi olduğunu söylüyo
		}

}

*/
uint32_t lastDebounceTime = 0;
const uint32_t debounceDelay = 200; // Debounce delay in milliseconds

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
     uint32_t currentTime = HAL_GetTick(); // Get current time in milliseconds

     if ((currentTime - lastDebounceTime) >= debounceDelay) {
         i++;
         sure = sure - 400;
         if (i == 4) {
             i = 0;
             sure = 1900;
            }

         lastDebounceTime = currentTime;
        }
    }


int main(void)
{
  HAL_Init();

  SystemClock_Config();

  GPIO_EXTI_Config();

  MX_GPIO_Init();

  HAL_GPIO_EXTI_Callback(GPIO_PIN_0);


  while (1)
  {
	  // Predetermined state
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, SET);
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14|GPIO_PIN_15, RESET);

	  if (i == 1){
	 			  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, SET);
	 		  }
	  else if (i == 2){

		  HAL_Delay(sure);

	 	  HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_13 | GPIO_PIN_15);
		  	 }
	  else if (i == 3){//i=3 iken if bloğu tamamlanıyo ve çıktıktan sonra while(1) döngüsü içinde olduğu için ve i=3 olduğu için yeniden bu ifade içine girip pin değerlerini değiştiriyo.
		  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, SET);

		  HAL_Delay(sure);

		  HAL_GPIO_TogglePin(GPIOD,  GPIO_PIN_13| GPIO_PIN_14);

		  HAL_Delay(sure);

	 		  }

  }
  /* USER CODE END 3 */
}


void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 50;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15 | GPIO_PIN_14 |GPIO_PIN_13, GPIO_PIN_RESET);

  /*Configure GPIO pins : PA0 PA1 PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PD15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15 | GPIO_PIN_14 | GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);


/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
