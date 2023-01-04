/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUFFER_SIZE 8
#define RF_BL_ADV_CHANNEL_38                        2478000000 // Hz
#define RF_FREQUENCY                                RF_BL_ADV_CHANNEL_38 // Hz
#define TX_OUTPUT_POWER                             13
#define TX_TIMEOUT_VALUE                            1000 // ms

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
bool isMaster = false;//true 为发送 false 为接受
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void OnTxDone( void );
void OnRxDone( void );
void OnTxTimeout( void );
void OnRxTimeout( void );
void OnRxError( IrqErrorCode_t );
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


RadioCallbacks_t Callbacks =
{
    &OnTxDone,        // txDone
    &OnRxDone,        // rxDone
    NULL,             // syncWordDone
    NULL,             // headerDone
    &OnTxTimeout,     // txTimeout
    &OnRxTimeout,     // rxTimeout
    &OnRxError,       // rxError
    NULL,             // rangingDone
    NULL,             // cadDone
};






uint8_t BufferSize = BUFFER_SIZE;

uint8_t Buffer[BUFFER_SIZE];
int8_t rssi=0;
uint8_t Tx_Msg[4],Rx_Msg[4];
uint16_t RxIrqMask = IRQ_RX_DONE | IRQ_RX_TX_TIMEOUT | IRQ_CRC_ERROR;
uint16_t TxIrqMask = IRQ_TX_DONE | IRQ_RX_TX_TIMEOUT | IRQ_CRC_ERROR;
AppStates_t AppState = APP_LOWPOWER;
PacketStatus_t packetStatus;

pkt_t pkt;
uint8_t mode=0;// 0 lora 1 GFSK 2 FLRC

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
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  MX_TIM6_Init();

  
  /* USER CODE BEGIN 2 */
   Radio.Init( &Callbacks );
   Radio.SetRegulatorMode( USE_DCDC ); // Can also be set in LDO mode but consume more power
   memset( &Buffer, 0x00, BufferSize );
   FLRC_SetParams();
//   GFSK_SetParams();
//   LORA_SetParams();
   SX1280_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

    /* USER CODE END WHILE */
    
    if(isMaster==true)//send  mode
    {  
      pkt.seq=0;
      Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE );
      Radio.SendPayload( (uint8_t*)&pkt, sizeof(pkt), ( TickTime_t ){ RX_TIMEOUT_TICK_SIZE, TX_TIMEOUT_VALUE } );
      AppState=APP_LOWPOWER;
      while(1)
      {
        SX1280ProcessIrqs( );
        
        switch( AppState )
        {
        case APP_TX:
          AppState=APP_LOWPOWER;
          pkt.addr=0x01;
          pkt.seq ++;
          Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE );
          Radio.SendPayload( (uint8_t*)&pkt, sizeof(pkt), ( TickTime_t ){ RX_TIMEOUT_TICK_SIZE, TX_TIMEOUT_VALUE } );
          HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
          HAL_Delay(20);
          break;
        default:
          break;    
        }

      }

    }  
    else if(isMaster==false)//receive  mode
    {
      
      AppState=APP_LOWPOWER;
      Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE );
      Radio.SetRx( ( TickTime_t ) { RX_TIMEOUT_TICK_SIZE, RX_TIMEOUT_VALUE } );
      printf("start\n");
      while(1)
      {
        SX1280ProcessIrqs( );

        switch( AppState )
        {
        case APP_RX:
          AppState = APP_LOWPOWER;        
          Radio.GetPayload( Buffer, &BufferSize, BUFFER_SIZE );//接收的数据装载到buffer
          SX1280GetPacketStatus(&packetStatus);
          rssi = SX1280GetRssiInst();
          printf("%d--Rssi:%d\n", Buffer[1],rssi);
        
          
          Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE );
          Radio.SetRx( ( TickTime_t ) { RX_TIMEOUT_TICK_SIZE, RX_TIMEOUT_VALUE } );
          HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
          break;
        case APP_RX_TIMEOUT:  
        case APP_RX_ERROR:
          Radio.SetDioIrqParams( RxIrqMask, RxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE );
          Radio.SetRx( ( TickTime_t ) { RX_TIMEOUT_TICK_SIZE, RX_TIMEOUT_VALUE } );
          break;
        default:
          break;    
        }
      }
      
    }
    
    
    
    /* USER CODE BEGIN 3 */
}
  /* USER CODE END 3 */


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim->Instance==TIM6)
  {
   
//    if(isMaster==true)
//    {
//      mode++;
//      if(mode>=3)mode=0;
//      if(mode==0) LORA_SetParams();
//      else if(mode==1) GFSK_SetParams();
//      else if(mode==2) FLRC_SetParams();
//      
//      SX1280SetPacketType(modulationParams.PacketType);
//      SX1280SetModulationParams(&modulationParams);
//      SX1280SetPacketParams(&packetParams);
//
//      
//      Radio.SetDioIrqParams( TxIrqMask, TxIrqMask, IRQ_RADIO_NONE, IRQ_RADIO_NONE );
//      Radio.SendPayload( (uint8_t*)&pkt, sizeof(pkt), ( TickTime_t ){ RX_TIMEOUT_TICK_SIZE, TX_TIMEOUT_VALUE } );
//    }
  }


}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == DIO1_Pin)
  {
    SX1280OnDioIrq();
    
  }

  
}
void OnTxDone( void )
{
  TickCounter=0;
  AppState = APP_TX;
}

void OnRxDone( void )
{
    TickCounter=0;
    AppState = APP_RX;
}

void OnTxTimeout( void )
{
    TickCounter=0;
    AppState = APP_TX_TIMEOUT;

}

void OnRxTimeout( void )
{  
    TickCounter=0;
    AppState = APP_RX_TIMEOUT;
}

void OnRxError( IrqErrorCode_t errorCode )
{
    TickCounter=0;
    AppState = APP_RX_ERROR;
}
int fputc(int ch, FILE *f) {      
  HAL_UART_Transmit(&huart1,(uint8_t *)&ch,1,0xffff);
  return ch;
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
