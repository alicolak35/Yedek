/* USER CODE BEGIN Header */
/*
 * Konfigürasyonu değiştiriyorum, pull-up ve pull-down değiştirince başlangıç durumu değiştiğ varsayımında ledlerde bir yanma yada sönme olmuyo.
 * 		1- Portların RCC saatini aktifleştirdim.
 * kesme tanımlamalarımda hata olabilir(?kesme konfigurasyonu?)
 *     1-NVIC kaydedicinsde aktifleştirdim =>HAL_NVIC_EnableIrq()
		2-KOnfigurasyonu yaptım
		3-HAL_GPIO kütüphanesindenIRQHandler fonksiyonunu çağırdım. Bu fonksiyon Callback'i otomatik çağrıyo ve hangi hattan kesme eldiği  elirtiliyo
		SON-Kırmızı led bir defa konumunu değiştirip sabit kalıyo.
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

ADC_HandleTypeDef hadc1;

uint16_t adc_degeri;
float sicaklik = 2;


void SystemClock_Config(void);
static void MX_ADC1_Init(void);
static void GPIO_Init();

static void EXTI_Config();
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
void EXTI0_IRQHandler();

int main(void)
{

  HAL_Init();
  EXTI_Config();
  SystemClock_Config();
  GPIO_Init();
  MX_ADC1_Init();
  EXTI0_IRQHandler();
  //HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);

  HAL_ADC_Start(&hadc1);

  while (1)
  {

	  HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);

	  adc_degeri = HAL_ADC_GetValue(&hadc1);
	  sicaklik = ((float)adc_degeri) / 4095*3300;
	  sicaklik = ((sicaklik - 760) / 2.5) + 25 ;
	  if(sicaklik > 40){
		  HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, SET);//
	  }

  }
}


static void MX_ADC1_Init(void)
{

  ADC_ChannelConfTypeDef sConfig = {0};

  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.EOCSelection = ADC_EOC_SEQ_CONV;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConv = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.DMAContinuousRequests = DISABLE;

  HAL_ADC_Init(&hadc1);

  sConfig.Channel = ADC_CHANNEL_TEMPSENSOR;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_480CYCLES;
  HAL_ADC_ConfigChannel(&hadc1, &sConfig);


}

static void EXTI_Config(){
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);

	EXTI->IMR = 0x00000001;
	EXTI->FTSR = 0x00000001;

}

void EXTI0_IRQHandler(){
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == GPIO_PIN_0){// ŞU ANDA BU PİN "INPUT" OLARAK AYARLANDI-MODER0=00-
		uint32_t start = HAL_GetTick();
		while((HAL_GetTick() - start) <= 3300){//aşağıdaki haliyle togglepin gibi yalnızca yandı.
			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, SET);
			HAL_Delay(1100);
			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, RESET);
			HAL_Delay(1100);
			HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, SET);
			//HAL_GPIO_TogglePin(GPIOG, GPIO_PIN_14);
			//eğer başka bir kesme aktif olsaydı erteleme fonksiyonunda dolayı işleme girmicekti.
		}
	//GPIOG->BSRR = 0x40000000;
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, RESET);
	}
}


static void GPIO_Init(){

	GPIO_InitTypeDef igpio;

    __HAL_RCC_GPIOG_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

	igpio.Mode = GPIO_MODE_OUTPUT_PP;
	igpio.Pin = GPIO_PIN_13 | GPIO_PIN_14;
	igpio.Pull = GPIO_NOPULL;
	igpio.Speed = GPIO_SPEED_FREQ_MEDIUM;
	HAL_GPIO_Init(GPIOG, &igpio);

	igpio.Mode = GPIO_MODE_INPUT;
	igpio.Pin = GPIO_PIN_0;
	igpio.Pull = GPIO_PULLDOWN;
	igpio.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &igpio);

	/*
	GPIOG -> MODER = 0x14000000;//g13-g14 pinleri çıktı verecek
	GPIOG -> OSPEEDR = 0x14000000;//orta hız
	GPIOG -> PUPDR = 0x28000000;//

	GPIOA -> OSPEEDR = 0x00000004;
	GPIOA -> PUPDR = 0x00000002;//PULL-UP olursa pin hep yüksek mantık durumunda olacak. Bu yüzden fınksiyonu != olarak ayarlamak durumunda kalacağım ve başka pin kullanımında dahi bu koşul sağlandığından kesme devreye girecek.
*/
}

void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

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
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV8;
  RCC_OscInitStruct.PLL.PLLQ = 3;

  HAL_RCC_OscConfig(&RCC_OscInitStruct);


  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2);

}
