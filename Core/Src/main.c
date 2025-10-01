/*
 * 1. Link: https://github.com/0geder/EEE3096S-Prac-4
 * 2. Group Number: 40
 * 3. Members: OKTSAM001 PTXNYA001
 */
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "stm32f4xx.h"
#include "lcd_stm32f4.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
// Task 2: Define core parameters
#define NS        256     // Number of samples in LUT
#define TIM2CLK   16000000 // STM Clock frequency for TIM2 (16 MHz)
#define F_SIGNAL  440   	// Frequency of output analog signal (A4 note, 440 Hz)
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
DMA_HandleTypeDef hdma_tim2_ch1;

/* USER CODE BEGIN PV */
// Task 1: Generate Lookup Tables (LUTs) - DATA NOW POPULATED
uint32_t Sin_LUT[256] = {
    2048, 2098, 2148, 2198, 2248, 2298, 2348, 2398, 2447, 2496, 2545, 2594, 2642, 2690, 2737, 2784,
    2831, 2877, 2923, 2968, 3013, 3057, 3100, 3143, 3185, 3226, 3267, 3307, 3346, 3385, 3423, 3459,
    3495, 3530, 3565, 3598, 3630, 3662, 3692, 3722, 3750, 3777, 3804, 3829, 3853, 3876, 3898, 3919,
    3939, 3958, 3975, 3992, 4007, 4021, 4034, 4045, 4056, 4065, 4073, 4080, 4085, 4089, 4093, 4094,
    4095, 4094, 4093, 4089, 4085, 4080, 4073, 4065, 4056, 4045, 4034, 4021, 4007, 3992, 3975, 3958,
    3939, 3919, 3898, 3876, 3853, 3829, 3804, 3777, 3750, 3722, 3692, 3662, 3630, 3598, 3565, 3530,
    3495, 3459, 3423, 3385, 3346, 3307, 3267, 3226, 3185, 3143, 3100, 3057, 3013, 2968, 2923, 2877,
    2831, 2784, 2737, 2690, 2642, 2594, 2545, 2496, 2447, 2398, 2348, 2298, 2248, 2198, 2148, 2098,
    2048, 1997, 1947, 1897, 1847, 1797, 1747, 1697, 1648, 1599, 1550, 1501, 1453, 1405, 1358, 1311,
    1264, 1218, 1172, 1127, 1082, 1038, 995, 952, 910, 869, 828, 788, 749, 710, 672, 636,
    600, 565, 530, 497, 465, 433, 403, 373, 345, 318, 291, 266, 242, 219, 197, 176,
    156, 137, 120, 103, 88, 74, 61, 50, 39, 30, 22, 15, 10, 6, 2, 1,
    0, 1, 2, 6, 10, 15, 22, 30, 39, 50, 61, 74, 88, 103, 120, 137,
    156, 176, 197, 219, 242, 266, 291, 318, 345, 373, 403, 433, 465, 497, 530, 565,
    600, 636, 672, 710, 749, 788, 828, 869, 910, 952, 995, 1038, 1082, 1127, 1172, 1218,
    1264, 1311, 1358, 1405, 1453, 1501, 1550, 1599, 1648, 1697, 1747, 1797, 1847, 1897, 1947, 1997,
};

uint32_t Saw_LUT[256] = {
    0, 15, 31, 47, 63, 79, 95, 111, 127, 143, 159, 175, 191, 207, 223, 239,
    255, 271, 287, 303, 319, 335, 351, 367, 383, 399, 415, 431, 447, 463, 479, 495,
    511, 527, 543, 559, 575, 591, 607, 623, 639, 655, 671, 687, 703, 719, 735, 751,
    767, 783, 799, 815, 831, 847, 863, 879, 895, 911, 927, 943, 959, 975, 991, 1007,
    1023, 1039, 1055, 1071, 1087, 1103, 1119, 1135, 1151, 1167, 1183, 1199, 1215, 1231, 1247, 1263,
    1279, 1295, 1311, 1327, 1343, 1359, 1375, 1391, 1407, 1423, 1439, 1455, 1471, 1487, 1503, 1519,
    1535, 1551, 1567, 1583, 1599, 1615, 1631, 1647, 1663, 1679, 1695, 1711, 1727, 1743, 1759, 1775,
    1791, 1807, 1823, 1839, 1855, 1871, 1887, 1903, 1919, 1935, 1951, 1967, 1983, 1999, 2015, 2031,
    2047, 2063, 2079, 2095, 2111, 2127, 2143, 2159, 2175, 2191, 2207, 2223, 2239, 2255, 2271, 2287,
    2303, 2319, 2335, 2351, 2367, 2383, 2399, 2415, 2431, 2447, 2463, 2479, 2495, 2511, 2527, 2543,
    2559, 2575, 2591, 2607, 2623, 2639, 2655, 2671, 2687, 2703, 2719, 2735, 2751, 2767, 2783, 2799,
    2815, 2831, 2847, 2863, 2879, 2895, 2911, 2927, 2943, 2959, 2975, 2991, 3007, 3023, 3039, 3055,
    3071, 3087, 3103, 3119, 3135, 3151, 3167, 3183, 3199, 3215, 3231, 3247, 3263, 3279, 3295, 3311,
    3327, 3343, 3359, 3375, 3391, 3407, 3423, 3439, 3455, 3471, 3487, 3503, 3519, 3535, 3551, 3567,
    3583, 3599, 3615, 3631, 3647, 3663, 3679, 3695, 3711, 3727, 3743, 3759, 3775, 3791, 3807, 3823,
    3839, 3855, 3871, 3887, 3903, 3919, 3935, 3951, 3967, 3983, 3999, 4015, 4031, 4047, 4063, 4079,
};

uint32_t Triangle_LUT[256] = {
    0, 32, 64, 96, 128, 161, 193, 225, 257, 290, 322, 354, 386, 419, 451, 483,
    515, 548, 580, 612, 644, 677, 709, 741, 773, 806, 838, 870, 902, 935, 967, 999,
    1031, 1064, 1096, 1128, 1152, 1184, 1216, 1248, 1289, 1322, 1354, 1386, 1418, 1450, 1483, 1515,
    1547, 1579, 1612, 1644, 1676, 1708, 1741, 1773, 1805, 1837, 1870, 1902, 1934, 1966, 1999, 2031,
    2063, 2095, 2128, 2160, 2192, 2224, 2257, 2289, 2321, 2353, 2386, 2418, 2450, 2482, 2515, 2547,
    2579, 2611, 2644, 2676, 2708, 2740, 2772, 2805, 2837, 2869, 2901, 2934, 2966, 2998, 3030, 3063,
    3095, 3127, 3159, 3192, 3224, 3256, 3288, 3321, 3353, 3385, 3417, 3450, 3482, 3514, 3546, 3579,
    3611, 3643, 3675, 3708, 3740, 3772, 3804, 3837, 3869, 3901, 3933, 3966, 3998, 4030, 4062, 4095,
    4095, 4062, 4030, 3998, 3966, 3933, 3901, 3869, 3837, 3804, 3772, 3740, 3708, 3675, 3643, 3611,
    3579, 3546, 3514, 3482, 3450, 3417, 3385, 3353, 3321, 3288, 3256, 3224, 3192, 3159, 3127, 3095,
    3063, 3030, 2998, 2966, 2934, 2901, 2869, 2837, 2805, 2772, 2740, 2708, 2676, 2644, 2611, 2579,
    2547, 2515, 2482, 2450, 2418, 2386, 2353, 2321, 2289, 2257, 2224, 2192, 2160, 2128, 2095, 2063,
    2031, 1999, 1966, 1934, 1902, 1870, 1837, 1805, 1773, 1741, 1708, 1676, 1644, 1612, 1579, 1547,
    1515, 1483, 1450, 1418, 1386, 1354, 1322, 1289, 1257, 1225, 1193, 1160, 1128, 1096, 1064, 1031,
    999, 967, 935, 902, 870, 838, 806, 773, 741, 709, 677, 644, 612, 580, 548, 515,
    483, 451, 419, 386, 354, 322, 290, 257, 225, 193, 161, 128, 96, 64, 32, 0,
};

// These instrument LUTs are declared here and will be populated with placeholder data
uint32_t Piano_LUT[NS] = {};
uint32_t Guitar_LUT[NS] = {};
uint32_t Drum_LUT[NS] = {};

// --- Pointers for easy waveform switching ---
#define NUM_WAVEFORMS 6
const uint32_t* lut_p[NUM_WAVEFORMS] = {Sin_LUT, Saw_LUT, Triangle_LUT, Piano_LUT, Guitar_LUT, Drum_LUT};
const char* lut_names[NUM_WAVEFORMS] = {"Sine", "Sawtooth", "Triangle", "Piano", "Guitar", "Drum"};
volatile uint8_t current_waveform = 0; // 'volatile' is crucial as it's modified in an ISR

// Task 3: Calculate TIM2_Ticks
// This formula determines the sample rate.
uint32_t TIM2_Ticks = (TIM2CLK / (NS * F_SIGNAL)) - 1; // How often to write new LUT value
uint32_t DestAddress = (uint32_t) &(TIM3->CCR3); // The destination for DMA: TIM3's duty cycle register

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void EXTI0_IRQHandler(void);
void copy_luts(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
// Helper function to populate the instrument LUTs with placeholder data
void copy_luts(void) {
    for (int i = 0; i < NS; i++) {
        Piano_LUT[i] = Sin_LUT[i];      // Piano will sound like a Sine wave
        Guitar_LUT[i] = Triangle_LUT[i]; // Guitar will sound like a Triangle wave
        Drum_LUT[i] = Saw_LUT[i];       // Drum will sound like a Sawtooth wave
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  // Populate the placeholder WAV file LUTs
  copy_luts();
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/
  HAL_Init();
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  // Initialize the LCD display (ensure function name matches your library, e.g., lcd_init)
  lcd_init();

  // --- Task 4: Configure and Start the Sound Generation ---
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  HAL_TIM_Base_Start(&htim2);
  HAL_DMA_Start_IT(&hdma_tim2_ch1, (uint32_t)Sin_LUT, DestAddress, NS);
  lcd_putstr("Sine");
  __HAL_TIM_ENABLE_DMA(&htim2, TIM_DMA_CC1);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // The main loop is empty. Sound is generated entirely by hardware.
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = TIM2_Ticks;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_OC_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_OC1REF;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_TIMING;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_OC_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  hdma_tim2_ch1.Instance = DMA1_Stream5;
  hdma_tim2_ch1.Init.Channel = DMA_CHANNEL_3;
  hdma_tim2_ch1.Init.Direction = DMA_MEMORY_TO_PERIPH;
  hdma_tim2_ch1.Init.PeriphInc = DMA_PINC_DISABLE;
  hdma_tim2_ch1.Init.MemInc = DMA_MINC_ENABLE;
  hdma_tim2_ch1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
  hdma_tim2_ch1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
  hdma_tim2_ch1.Init.Mode = DMA_CIRCULAR;
  hdma_tim2_ch1.Init.Priority = DMA_PRIORITY_HIGH;
  hdma_tim2_ch1.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
  if (HAL_DMA_Init(&hdma_tim2_ch1) != HAL_OK)
  {
      Error_Handler();
  }
  __HAL_LINKDMA(&htim2, hdma[TIM_DMA_ID_CC1], hdma_tim2_ch1);
}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 0;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 4095;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_TIM_MspPostInit(&htim3);
}

/**
  * @brief DMA Initialization Function
  */
static void MX_DMA_Init(void)
{
  __HAL_RCC_DMA1_CLK_ENABLE();
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}

/**
  * @brief GPIO Initialization Function
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  // Your LCD Init code would typically be here or called from main
  
  GPIO_InitStruct.Pin = Button0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Button0_GPIO_Port, &GPIO_InitStruct);

  HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

/* USER CODE BEGIN 4 */
// Task 5: Pushbutton Interrupt Service Routine
void EXTI0_IRQHandler(void){
	static uint32_t last_interrupt_time = 0;
	uint32_t interrupt_time = HAL_GetTick();

	// Debounce Logic
	if (interrupt_time - last_interrupt_time > 200)
	{
		last_interrupt_time = interrupt_time;

		// Stop, reconfigure, and restart the DMA
		HAL_TIM_DISABLE_DMA(&htim2, TIM_DMA_CC1);
		HAL_DMA_Abort_IT(&hdma_tim2_ch1);
		current_waveform = (current_waveform + 1) % NUM_WAVEFORMS;
		lcd_clear();
		lcd_putstr((char*)lut_names[current_waveform]);
		HAL_DMA_Start_IT(&hdma_tim2_ch1, (uint32_t)lut_p[current_waveform], DestAddress, NS);
		__HAL_TIM_ENABLE_DMA(&htim2, TIM_DMA_CC1);
	}
	HAL_GPIO_EXTI_IRQHandler(Button0_Pin); // Clear the interrupt flag
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  */
void Error_Handler(void)
{
  __disable_irq();
  while (1) {}
}

#ifdef USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line) {}
#endif /* USE_FULL_ASSERT */