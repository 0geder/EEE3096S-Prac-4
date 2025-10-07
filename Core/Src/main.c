/*
 * 1. Link: https://github.com/0geder/EEE3096S-Prac-4
 * 2. Group Number: 40
 * 3. Members: OKTSAM001 PTXNYA001
 */
/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body for Real-Time Audio Synthesizer
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

// --- DUAL MODE SWITCH: Set to 1 for Oscilloscope Demo, 0 for Audible Sound ---
#define DEMO_MODE 1

// --- General Parameters ---
#define TIM2CLK   16000000 // STM Clock frequency for TIM2

// --- Sample Sizes for Improved Audio Quality ---
#define NS                128   // Samples for standard, repeating waveforms
#define NS_INSTRUMENT     8192  // Larger sample size for richer instrument sounds

#if DEMO_MODE == 1
  // --- Frequencies for Oscilloscope Viewing (Sub-sonic, for visual verification) ---
  #define F_SIGNAL          0.5   // Plays one cycle over 2 seconds
  #define F_SIGNAL_PIANO    0.06
  #define F_SIGNAL_GUITAR   0.1
  #define F_SIGNAL_DRUM     0.09
#else
  // --- Frequencies for Audible Sound ---
  #define F_SIGNAL          440   // A4 Note for Sine/Saw/Triangle
  #define F_SIGNAL_PIANO    262   // C4 Note (Middle C)
  #define F_SIGNAL_GUITAR   330   // E4 Note
  #define F_SIGNAL_DRUM     100   // Low frequency for a kick-like sound
#endif

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;
DMA_HandleTypeDef hdma_tim2_ch1;

/* USER CODE BEGIN PV */

// --- Lookup Tables ---
// Standard waveforms are kept in main.c for simplicity
uint32_t Sin_LUT[NS] = {2048,2148,2248,2348,2447,2545,2642,2737,2831,2923,3013,3100,3185,3267,3346,3423,3495,3565,3630,3692,3750,3804,3853,3898,3939,3975,4007,4034,4056,4073,4085,4093,4095,4093,4085,4073,4056,4034,4007,3975,3939,3898,3853,3804,3750,3692,3630,3565,3495,3423,3346,3267,3185,3100,3013,2923,2831,2737,2642,2545,2447,2348,2248,2148,2048,1947,1847,1747,1648,1550,1453,1358,1264,1172,1082,995,910,828,749,672,600,530,465,403,345,291,242,197,156,120,88,61,39,22,10,2,0,2,10,22,39,61,88,120,156,197,242,291,345,403,465,530,600,672,749,828,910,995,1082,1172,1264,1358,1453,1550,1648,1747,1847,1947};
uint32_t Saw_LUT[NS] = {0,32,64,97,129,161,193,226,258,290,322,355,387,419,451,484,516,548,580,613,645,677,709,742,774,806,838,871,903,935,967,1000,1032,1064,1096,1129,1161,1193,1225,1258,1290,1322,1354,1386,1419,1451,1483,1515,1548,1580,1612,1644,1677,1709,1741,1773,1806,1838,1870,1902,1935,1967,1999,2031,2064,2096,2128,2160,2193,2225,2257,2289,2322,2354,2386,2418,2451,2483,2515,2547,2580,2612,2644,2676,2709,2741,2773,2805,2837,2870,2902,2934,2966,2999,3031,3063,3095,3128,3160,3192,3224,3257,3289,3321,3353,3386,3418,3450,3482,3515,3547,3579,3611,3644,3676,3708,3740,3773,3805,3837,3869,3902,3934,3966,3998,4031,4063,4095};
uint32_t Triangle_LUT[NS] = {0,64,128,192,256,320,384,448,512,576,640,704,768,832,896,960,1024,1088,1152,1216,1280,1344,1408,1472,1536,1600,1664,1728,1792,1856,1920,1984,2048,2111,2175,2239,2303,2367,2431,2495,2559,2623,2687,2751,2815,2879,2943,3007,3071,3135,3199,3263,3327,3391,3455,3519,3583,3647,3711,3775,3839,3903,3967,4031,4095,4030,3965,3900,3835,3770,3705,3640,3575,3510,3445,3380,3315,3250,3185,3120,3055,2990,2925,2860,2795,2730,2665,2600,2535,2470,2405,2340,2275,2210,2145,2080,2015,1950,1885,1820,1755,1690,1625,1560,1495,1430,1365,1300,1235,1170,1105,1040,975,910,845,780,715,650,585,520,455,390,325,260,195,130,65,0};

// Instrument LUTs are included from header files for modularity
#include "lut_headers/piano_lut.h"
#include "lut_headers/guitar_lut.h"
#include "lut_headers/drum_lut.h"

// --- Pointers for easy waveform switching ---
#define NUM_WAVEFORMS 6
const uint32_t* lut_p[NUM_WAVEFORMS] = {Sin_LUT, Saw_LUT, Triangle_LUT, Piano_LUT, Guitar_LUT, Drum_LUT};
const char* lut_names[NUM_WAVEFORMS] = {"Sine", "Sawtooth", "Triangle", "Piano", "Guitar", "Drum"};

// --- State-tracking variables ---
volatile uint32_t active_lut_id = 0;
uint32_t TIM2_Ticks;
uint32_t DestAddress = (uint32_t) &(TIM3->CCR3);

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM3_Init(void);
void EXTI0_IRQHandler(void);
void Update_Timer_And_Start_DMA(void); // Helper function for dynamic updates

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/**
  * @brief  Dynamically updates timer period and restarts DMA for the active waveform.
  *         This handles the variable sample sizes and frequencies.
  */
void Update_Timer_And_Start_DMA(void) {
    uint32_t current_ns;
    float current_f_signal;

    // Select the correct sample size and frequency based on the active LUT
    if (active_lut_id >= 3) { // Instrument sounds
        current_ns = NS_INSTRUMENT;
        if (active_lut_id == 3) current_f_signal = F_SIGNAL_PIANO;
        else if (active_lut_id == 4) current_f_signal = F_SIGNAL_GUITAR;
        else current_f_signal = F_SIGNAL_DRUM;
    } else { // Standard waveforms
        current_ns = NS;
        current_f_signal = F_SIGNAL;
    }

    // Recalculate TIM2 period for the new settings
    TIM2_Ticks = (uint32_t)((float)TIM2CLK / ((float)current_ns * current_f_signal)) - 1;
    __HAL_TIM_SET_AUTORELOAD(&htim2, TIM2_Ticks);

    // Restart DMA with the correct LUT pointer and sample size
    HAL_DMA_Start_IT(&hdma_tim2_ch1, (uint32_t)lut_p[active_lut_id], DestAddress, current_ns);
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  */
int main(void)
{
  HAL_Init();
  SystemClock_Config();

  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();

  /* USER CODE BEGIN 2 */
  // Safe LCD Initialization Sequence
  HAL_Delay(30);
  lcd_command(POWER_UP);
  lcd_command(FOURBIT_MODE);
  lcd_command(DISPLAY_ON);
  lcd_command(TWOLINE_MODE);
  lcd_command(CLEAR);

  // --- Start Sound Generation ---
  HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_3);
  HAL_TIM_Base_Start(&htim2);

  // Initial setup for the first waveform (Sine)
  Update_Timer_And_Start_DMA();
  lcd_putstring((char*)lut_names[active_lut_id]);

  __HAL_TIM_ENABLE_DMA(&htim2, TIM_DMA_CC1);
  /* USER CODE END 2 */

  while (1) {}
}

/**
  * @brief System Clock Configuration
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
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
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
  */
static void MX_TIM2_Init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 0;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 0; // Will be set dynamically by Update_Timer_And_Start_DMA
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
  
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level*/
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_12|GPIO_PIN_15, GPIO_PIN_RESET);

  /*Configure GPIO pins : PC14 (RS) and PC15 (E) */
  GPIO_InitStruct.Pin = GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PB8 (D4) and PB9 (D5) */
  GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PA12 (D6) and PA15 (D7) */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_15;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /*Configure GPIO pin : Button0_Pin (PA0) */
  GPIO_InitStruct.Pin = Button0_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(Button0_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 2, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

/* USER CODE BEGIN 4 */
// Task 5: Pushbutton Interrupt Service Routine
void EXTI0_IRQHandler(void){
    static uint32_t last_button_tick = 0;
    uint32_t now = HAL_GetTick();

    // Debounce Logic
    if ((now - last_button_tick) < 300) { // Increased debounce time
        HAL_GPIO_EXTI_IRQHandler(Button0_Pin);
        return;
    }
    last_button_tick = now;

    // Safely Stop DMA
    HAL_DMA_Abort_IT(&hdma_tim2_ch1);

    // Update waveform ID
    active_lut_id = (active_lut_id + 1) % NUM_WAVEFORMS;

    // Update LCD
    lcd_command(CLEAR);
    lcd_putstring((char*)lut_names[active_lut_id]);

    // Recalculate timer period and restart DMA with new settings
    Update_Timer_And_Start_DMA();

    HAL_GPIO_EXTI_IRQHandler(Button0_Pin);
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