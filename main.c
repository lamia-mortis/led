#include <stm32f3xx.h>

#include "main.h"

void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void fill_modes_array();

mode_function mode_functions[MODE_CNT] = {
    [MODE_WHEEL] = NULL,
    [MODE_ALL_ON_OFF] = NULL,
    [MODE_SAME_COLOR] = NULL,
    [MODE_ODD_EVEN] = NULL,
};

volatile led_mode_e current_mode = MODE_WHEEL;

int main(void)
{
    /* MCU Configuration--------------------------------------------------------*/

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();

    fill_modes_array();

    while(1) {
      get_mode_function(current_mode)();
    }
}

mode_function get_mode_function(led_mode_e mode)
{
    return mode_functions[mode];
}

led_mode_e get_current_mode(void)
{
    return current_mode;
}

void set_current_mode(led_mode_e mode)
{
    current_mode = mode;
}

void delay(uint32_t time)
{
    while (time--);
}

static void fill_modes_array()
{
    mode_functions[MODE_WHEEL] = apply_wheel_mode;
    mode_functions[MODE_ALL_ON_OFF] = apply_all_on_off_mode;
    mode_functions[MODE_SAME_COLOR] = apply_same_color_mode;
    mode_functions[MODE_ODD_EVEN] = apply_odd_even_mode;
}

void apply_wheel_mode(void)
{
    GPIOE->ODR |= (uint32_t)GPIO_PIN_8;
    delay(100000);
    GPIOE->ODR |= (uint32_t)GPIO_PIN_9;
    delay(100000);
    GPIOE->ODR |= (uint32_t)GPIO_PIN_10;
    delay(100000);
    GPIOE->ODR |= (uint32_t)GPIO_PIN_11;
    delay(100000);
    GPIOE->ODR |= (uint32_t)GPIO_PIN_12;
    delay(100000);
    GPIOE->ODR |= (uint32_t)GPIO_PIN_13;
    delay(100000);
    GPIOE->ODR |= (uint32_t)GPIO_PIN_14;
    delay(100000);
    GPIOE->ODR |= (uint32_t)GPIO_PIN_15;
    delay(100000);

    GPIOE->ODR &= ~((uint32_t)GPIO_PIN_8);
    delay(100000);
    GPIOE->ODR &= ~((uint32_t)GPIO_PIN_9);
    delay(100000);
    GPIOE->ODR &= ~((uint32_t)GPIO_PIN_10);
    delay(100000);
    GPIOE->ODR &= ~((uint32_t)GPIO_PIN_11);
    delay(100000);
    GPIOE->ODR &= ~((uint32_t)GPIO_PIN_12);
    delay(100000);
    GPIOE->ODR &= ~((uint32_t)GPIO_PIN_13);
    delay(100000);
    GPIOE->ODR &= ~((uint32_t)GPIO_PIN_14);
    delay(100000);
    GPIOE->ODR &= ~((uint32_t)GPIO_PIN_15);
    delay(100000);
}

void apply_all_on_off_mode(void)
{
    GPIO_PinState current_state = (GPIOE->IDR && GPIO_PIN_12) == (uint32_t)GPIO_PIN_SET;
    GPIO_PinState set_state = current_state ? GPIO_PIN_RESET : GPIO_PIN_SET;

    if (set_state)
    {
    	GPIOE->ODR |= (uint32_t)GPIO_PIN_All;
    } else {
    	GPIOE->ODR &= ~((uint32_t)GPIO_PIN_All);
    }

    delay(1000000);
}

void apply_same_color_mode(void)
{
    GPIOE->ODR &= ~((uint32_t)(GPIO_PIN_10|GPIO_PIN_14));
    GPIOE->ODR |= (uint32_t)(GPIO_PIN_9|GPIO_PIN_13);

    delay(1000000);

    GPIOE->ODR &= ~((uint32_t)(GPIO_PIN_9|GPIO_PIN_13));
    GPIOE->ODR |= (uint32_t)(GPIO_PIN_8|GPIO_PIN_12);

    delay(1000000);

    GPIOE->ODR &= ~((uint32_t)(GPIO_PIN_8|GPIO_PIN_12));
    GPIOE->ODR |= (uint32_t)(GPIO_PIN_11|GPIO_PIN_15);

    delay(1000000);

    GPIOE->ODR &= ~((uint32_t)(GPIO_PIN_11|GPIO_PIN_15));
    GPIOE->ODR |= (uint32_t)(GPIO_PIN_10|GPIO_PIN_14);

    delay(1000000);
}

void apply_odd_even_mode(void)
{
    GPIOE->ODR &= ~((uint32_t)(GPIO_PIN_8|GPIO_PIN_10|GPIO_PIN_12|GPIO_PIN_14));
    GPIOE->ODR |= (uint32_t)(GPIO_PIN_9|GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_15);

    delay(1000000);

    GPIOE->ODR &= ~((uint32_t)(GPIO_PIN_9|GPIO_PIN_11|GPIO_PIN_13|GPIO_PIN_15));
    GPIOE->ODR |= (uint32_t)(GPIO_PIN_8|GPIO_PIN_10|GPIO_PIN_12|GPIO_PIN_14);

    delay(1000000);
}

void EXTI0_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);

    GPIOE->ODR &= ~((uint32_t)GPIO_PIN_All);

    led_mode_e current_mode = get_current_mode();

    led_mode_e next_mode = current_mode + 1;

    set_current_mode(next_mode == MODE_CNT ? MODE_WHEEL : next_mode);
}

void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /** Initializes the RCC Oscillators according to the specified parameters
    * in the RCC_OscInitTypeDef structure.
    */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
      Error_Handler();
    }

    /** Initializes the CPU, AHB and APB buses clocks
    */
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

static void MX_GPIO_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOE_CLK_ENABLE();

    /*Configure GPIO pin Output Level */
    HAL_GPIO_WritePin(GPIOE, GPIO_PIN_All, GPIO_PIN_RESET);

    /*Configure GPIO pin : PA0 */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /*Configure GPIO pins : PE8 PE9 PE10 PE11
                             PE12 PE13 PE14 PE15 */
    GPIO_InitStruct.Pin = GPIO_PIN_All;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

    /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void Error_Handler(void)
{
    __disable_irq();
    while (1)
    {
    }
}
