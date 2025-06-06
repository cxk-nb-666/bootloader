/**
 ******************************************************************************
 * @file    Project/STM32F4xx_StdPeriph_Templates/stm32f4xx_it.c
 * @author  MCD Application Team
 * @version V1.5.0
 * @date    06-March-2015
 * @brief   Main Interrupt Service Routines.
 *          This file provides template for all exceptions handler and
 *          peripherals interrupt service routine.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2015 STMicroelectronics</center></h2>
 *
 * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
 * You may not use this file except in compliance with the License.
 * You may obtain a copy of the License at:
 *
 *        http://www.st.com/software_license_agreement_liberty_v2
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "usart.h"
/** @addtogroup Template_Project
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
 * @brief  This function handles NMI exception.
 * @param  None
 * @retval None
 */
void NMI_Handler(void)
{
}

/**
 * @brief  This function handles Hard Fault exception.
 * @param  None
 * @retval None
 */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
 * @brief  This function handles Memory Manage exception.
 * @param  None
 * @retval None
 */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
 * @brief  This function handles Bus Fault exception.
 * @param  None
 * @retval None
 */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
 * @brief  This function handles Usage Fault exception.
 * @param  None
 * @retval None
 */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
 * @brief  This function handles SVCall exception.
 * @param  None
 * @retval None
 */
void SVC_Handler(void)
{
}

/**
 * @brief  This function handles Debug Monitor exception.
 * @param  None
 * @retval None
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief  This function handles PendSVC exception.
 * @param  None
 * @retval None
 */
void PendSV_Handler(void)
{
}

/**
 * @brief  This function handles SysTick Handler.
 * @param  None
 * @retval None
 */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f4xx.s).                                               */
/******************************************************************************/

/**
 * @brief  This function handles PPP interrupt request.
 * @param  None
 * @retval None
 */
/*void PPP_IRQHandler(void)
{
}*/

void USART1_IRQHandler(void)
{
  if (USART_GetFlagStatus(USART1, USART_FLAG_IDLE) != RESET)
  {

    USART_ClearFlag(USART1, USART_FLAG_IDLE);                                        // 清除中断标志位
    USART_ReceiveData(USART1);                                                       // 清除接收数据寄存器的标志位
    U1_RXBufptr.U1RxCount += (U1_RX_MAX + 1) - DMA_GetCurrDataCounter(DMA2_Stream5); // 获取接收数据的长度
    U1_RXBufptr.U1RXDataIN->end = &USART1_RxBuffer[U1_RXBufptr.U1RxCount - 1];
    U1_RXBufptr.U1RXDataIN++;

    if (U1_RXBufptr.U1RXDataIN == U1_RXBufptr.U1RXDataend)
    {
      U1_RXBufptr.U1RXDataIN = &U1_RXBufptr.U1_RxDataPtr[0];
    }

    if (U1_RX_BUFFER_SIZE - U1_RXBufptr.U1RxCount > U1_RX_MAX)
    {
      U1_RXBufptr.U1RXDataIN->start = &USART1_RxBuffer[U1_RXBufptr.U1RxCount];
    }
    else
    {
      
      U1_RXBufptr.U1RXDataIN->start = USART1_RxBuffer;
      U1_RXBufptr.U1RxCount = 0;
    }

    DMA_Cmd(DMA2_Stream5, DISABLE);                      // 关闭DMA接收
    DMA_SetCurrDataCounter(DMA2_Stream5, U1_RX_MAX + 1); // 设置接收数据的长度
    DMA_MemoryTargetConfig(DMA2_Stream5, (uint32_t)U1_RXBufptr.U1RXDataIN->start, DMA_Memory_0);
    // DMA2_Stream5->M0AR = (uint32_t)U1_RXBufptr.U1RXDataIN->start;  // 设置内存地址（接收缓冲区）
    DMA_Cmd(DMA2_Stream5, ENABLE); // 重新使能DMA接收
  }
}

/**
 * @}
 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
