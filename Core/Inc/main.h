/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mode.h"
#include "delay.h"
#include "sx1280.h"
#include "sx1280_hal.h"
#include "sx1280_radio.h"
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "stdio.h"
#include "main.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
typedef struct
{ 
  uint8_t addr;
  uint8_t seq;  
}pkt_t;
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

typedef enum
{
    APP_LOWPOWER,
    APP_RUNNING,
    APP_RX,
    APP_RX_TIMEOUT,
    APP_RX_ERROR,
    APP_TX,
    APP_TX_TIMEOUT,
}AppStates_t;
extern AppStates_t AppState;
extern uint8_t mode;
extern uint16_t RxIrqMask;
extern uint16_t TxIrqMask;
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define DIO1_Pin GPIO_PIN_1
#define DIO1_GPIO_Port GPIOA
#define RESET_Pin GPIO_PIN_2
#define RESET_GPIO_Port GPIOB
#define NSS_Pin GPIO_PIN_12
#define NSS_GPIO_Port GPIOB
#define SCK_Pin GPIO_PIN_13
#define SCK_GPIO_Port GPIOB
#define MISO_AF_Pin GPIO_PIN_14
#define MISO_AF_GPIO_Port GPIOB
#define MOSI_AF_Pin GPIO_PIN_15
#define MOSI_AF_GPIO_Port GPIOB
#define BUSY_Pin GPIO_PIN_11
#define BUSY_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_15
#define LED1_GPIO_Port GPIOA
/* USER CODE BEGIN Private defines */

#define RX_TIMEOUT_VALUE                            1000 // ms
#define RX_TIMEOUT_TICK_SIZE                        RADIO_TICK_SIZE_4000_US

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
