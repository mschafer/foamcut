/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_tim.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* USER CODE BEGIN Includes */
#include "fifo.hpp"
#include <string.h>
#include <algorithm>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
fifo<uint8_t, 16> rxFifo;
uint16_t rxXferCount = 0;
int rxError = 0;
int rxGood = 0;
char exp = 'a';
fifo<uint8_t, 16> txFifo;
uint16_t txXferCount = 0;
const char message[] = "bcdefghijklmnopqrstuvwxyz\r\n";
unsigned int mSent = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Error_Handler(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void rxDMA();
void txDMA();

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM11_Init();
  MX_USART2_UART_Init();

  /* USER CODE BEGIN 2 */
  // enable the timer
  __HAL_TIM_ENABLE(&htim11);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  size_t sptr = 0;
  size_t mLen = strlen(message);
  size_t nextSend = 1000;
  while (1)
  {
  /* USER CODE END WHILE */

  /* USER CODE BEGIN 3 */
      uint32_t tick = HAL_GetTick() + 1;
      txDMA();
      if (HAL_GetTick() == nextSend) {
          while (sptr < mLen) {
              fifo<uint8_t, 16>::carray d = txFifo.space_carray();
              size_t n = std::min(mLen - sptr, d.len_);
              std::copy(&message[sptr], &message[sptr]+n, d.buf_);
              txFifo.contents_add(n);
              sptr += n;
              mSent += n;
              txDMA();
          }
          sptr = 0;
          nextSend += 500;
      }


#if 0
      rxDMA();


      fifo<uint8_t, 16>::carray d = rxFifo.contents_carray();
      for (uint16_t i=0; i<d.len_; ++i) {
          if (d.buf_[i] != exp) {
              rxError++;
              exp = d.buf_[i] + 1;
          } else {
              ++exp;
              ++rxGood;
          }
          if (exp == '{') exp = 'a';
          d.buf_[i] = 0;
      }
      rxFifo.contents_remove(d.len_);
#endif

  }
  /* USER CODE END 3 */

}

/** System Clock Configuration
*/
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

    /**Initializes the CPU, AHB and APB busses clocks 
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

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */

void rxDMA()
{
    // adjust rxFifo contents based on progress of DMA transfer
    if (rxXferCount != 0) {
        uint16_t b = __HAL_DMA_GET_COUNTER(huart2.hdmarx);
        rxFifo.contents_add(rxXferCount - b);
        rxXferCount = b;
    }

    // start a DMA tranfer to the rx fifo if there isn't one running
    if(huart2.RxState == HAL_UART_STATE_READY) {
        fifo<uint8_t, 16>::carray d = rxFifo.space_carray();
        if (d.len_ == 0) return;
        HAL_UART_Receive_DMA(&huart2, d.buf_, d.len_);
        rxXferCount = d.len_;
    }

}

void txDMA()
{
    // adjust rxFifo contents based on progress of DMA transfer
    if (txXferCount != 0) {
        uint16_t b = __HAL_DMA_GET_COUNTER(huart2.hdmatx);
        if ((txXferCount - b) > txFifo.contents_size()) {
            ++rxError;
        }
        txFifo.contents_remove(txXferCount - b);
        txXferCount = b;
    }

    // start a DMA transfer from the tx fifo if there isn't one running
    if(huart2.gState == HAL_UART_STATE_READY) {
        txFifo.contents_remove(txXferCount);
        txXferCount = 0;
        fifo<uint8_t, 16>::carray d = txFifo.contents_carray();
        if (d.len_ == 0) return;
        HAL_UART_Transmit_DMA(&huart2, d.buf_, d.len_);
        txXferCount = d.len_;
    }
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(htim);

  HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);

  // re-initialize counter and update registers.
  // counter and prescaler counter are cleared
  //htim->Instance->EGR = TIM_EGR_UG;
  // this will cause an interrupt unless



  // set the timer reload value
  //__HAL_TIM_SET_AUTORELOAD(htim, 10000);

  // this is done by HAL_TIM_IRQHandler
  // clear interrupt flag
  //__HAL_TIM_CLEAR_IT(htim, TIM_SR_UIF);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  None
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler */
  /* User can add his own implementation to report the HAL error return state */
  while(1) 
  {
  }
  /* USER CODE END Error_Handler */ 
}

#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
