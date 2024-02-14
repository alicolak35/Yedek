#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_i2s.h"
#include "stm32f4xx_hal_spi.h"
#include "stm32f4xx_hal_flash.h"
#include <stdint.h>

#define SAMPLE_RATE 8000  // Adjust as needed
#define FLASH_START_ADDR 0x08E00000
// Adjust based on your STM32F407 memory layout

I2S_HandleTypeDef hi2s2;
SPI_HandleTypeDef hspi1;

void Error_Handler();
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2S2_Init(void);

void RecordAndSaveToFlash(void);

int main(void) {
    HAL_Init();

    SystemClock_Config();

    MX_GPIO_Init();

    MX_I2S2_Init();

    while (1) {
        RecordAndSaveToFlash();
    }
}

void RecordAndSaveToFlash(void) {
    // Buffer for sound samples
    uint16_t soundBuffer[SAMPLE_RATE];

    // Start I2S communication
    HAL_I2S_Receive(&hi2s2, soundBuffer, SAMPLE_RATE, HAL_MAX_DELAY);

    // Save sound data to flash memory
    HAL_FLASH_Unlock();

    for (int i = 0; i < SAMPLE_RATE; ++i) {
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, FLASH_START_ADDR + (i * 2), soundBuffer[i]);
        //HAL_FLASH fınksiyonlarında IRQRequest ile IT arasındaki fark ne?
    }

    HAL_FLASH_Lock();
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
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
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
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

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}


static void MX_I2S2_Init(void) {
    __HAL_RCC_SPI2_CLK_ENABLE();//APB1


    hi2s2.Instance = SPI2;
    hi2s2.Init.Mode = I2S_MODE_MASTER_RX;
    hi2s2.Init.Standard = I2S_STANDARD_LSB;
    hi2s2.Init.DataFormat = I2S_DATAFORMAT_16B;
    hi2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
    hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_16K;
    hi2s2.Init.CPOL = I2S_CPOL_LOW;
    hi2s2.Init.ClockSource = I2S_CLOCK_PLL;
    hi2s2.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;

    HAL_I2S_Init(&hi2s2);//ayarlama yapıldıktan sonra bu çağırılmalı

}


static void MX_GPIO_Init(void) {

    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOD_CLK_ENABLE();
    GPIO_InitTypeDef GPIO_InitStruct = {0};


    GPIO_InitStruct.Pin = GPIO_PIN_3 | GPIO_PIN_6;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.Alternate = GPIO_AF5_SPI2;

    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10 | GPIO_PIN_12;
    GPIO_InitStruct.Pull = GPIO_PULLUP;

    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);



    GPIO_InitStruct.Pin = GPIO_PIN_13 | GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

}

void Error_Handler(void)
{

  while (1)
  {
	  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
  }
}

