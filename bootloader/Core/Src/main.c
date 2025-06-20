/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "fatfs.h"
#include "sdio.h"
#include "usart.h"
#include "gpio.h"
#include "monocypher.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

typedef struct __attribute__((packed)) {
  uint16_t meta_magic;
  uint8_t signature1[64];
  uint8_t signature2[64];
  uint8_t signature3[64];
  uint8_t signature4[64];
  uint8_t signature5[64];
  uint8_t root_key1[32];
  uint8_t root_key2[32];
  uint8_t root_key3[32];
  uint8_t root_key4[32];
  uint8_t root_key5[32];
  uint8_t root_threshhold;
  uint8_t target_key1[32];
  uint8_t target_key2[32];
  uint8_t target_key3[32];
  uint8_t target_threshhold;
  uint8_t expire_major;
  uint8_t expire_minor;
  uint8_t expire_patch;
  uint8_t pad;
} root_metadata;

typedef struct __attribute__((packed)) {
  uint16_t image_magic;
  uint8_t signature1[64];
  uint8_t signature2[64];
  uint8_t signature3[64];
  uint16_t image_hdr_version;
  uint32_t data_size;
  uint8_t version_major;
  uint8_t version_minor;
  uint8_t version_patch;
  uint8_t pad;
} image_hdr_t;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void start_app(uint32_t pc, uint32_t sp) __attribute__((naked));
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

extern int __app_flash_start__;
extern int __app_flash_size__;

// static uint8_t tx_buff[] = "Hello, World!";
static FATFS fs;
static FRESULT fr;
static HAL_StatusTypeDef hr;
static FIL fil;
static UINT bytes_read;
static uint8_t read_buff[512];
static uint32_t page_error;
static uint32_t flash_read_buff[6];

static FLASH_EraseInitTypeDef erase_conf = {
  .TypeErase = FLASH_TYPEERASE_PAGES,
  .NbPages = 14
};

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
  MX_SDIO_SD_Init();
  MX_FATFS_Init();
  MX_USART1_Init();
  /* USER CODE BEGIN 2 */

  erase_conf.PageAddress = __app_flash_start__;

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  uint32_t *app_code = (uint32_t *)__app_flash_start__;
  uint32_t app_sp = app_code[0];
  uint32_t app_start = app_code[1];

  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    HAL_Delay(1000);

    for (int i = 0; i < 3; i++) {
      fr = f_mount(&fs, SDPath, 1);
      if (!fr) {
        break;
      }
      HAL_USART_Transmit(&husart1, (uint8_t*) "Error mounting SD\n", sizeof("Error mounting SD\n"), 1000);
      HAL_USART_Transmit(&husart1, (uint8_t*) "Entering infinite loop\n", sizeof("Entering infinite loop\n"), 1000);
      HAL_Delay(1000);
      if (i == 2) {
        while (1) {}
      }
    }

    fr = f_open(&fil, "firmware.bin", FA_READ);
    if (fr) {
      HAL_USART_Transmit(&husart1, (uint8_t*) "Error opening firmware.bin\n", sizeof("Error opening firmware.bin\n"), 1000);
      HAL_USART_Transmit(&husart1, (uint8_t*) "Entering infinite loop\n", sizeof("Entering infinite loop\n"), 1000);
      while (1) {}
    }

    hr = HAL_FLASH_Unlock();
    if (hr) {
      HAL_USART_Transmit(&husart1, (uint8_t*) "Failed to unlock flash\n", sizeof("Failed to unlock flash\n"), 1000);
      HAL_USART_Transmit(&husart1, (uint8_t*) "Entering infinite loop\n", sizeof("Entering infinite loop\n"), 1000);
      while (1) {}
    }

    hr = HAL_FLASHEx_Erase(&erase_conf, &page_error);
    if (hr) {
      HAL_USART_Transmit(&husart1, (uint8_t*) "Failed to erase flash\n", sizeof("Failed to erase flash\n"), 1000);
      HAL_USART_Transmit(&husart1, (uint8_t*) "Entering infinite loop\n", sizeof("Entering infinite loop\n"), 1000);
      while (1) {}
    }

    uint8_t still_reading = 1;
    uint8_t buffers_read = 0;
    while (still_reading) {
      HAL_USART_Transmit(&husart1, (uint8_t*) "Reading next buffer from file\n", sizeof("Reading next buffer from file\n"), 1000);
      fr = f_read(&fil, read_buff, sizeof(read_buff), &bytes_read);
      if (fr) {
        HAL_USART_Transmit(&husart1, (uint8_t*) "Error reading firmware.bin\n", sizeof("Error reading firmware.bin\n"), 1000);
        HAL_USART_Transmit(&husart1, (uint8_t*) "Entering infinite loop\n", sizeof("Entering infinite loop\n"), 1000);
        while (1) {}
      }
      if (bytes_read < sizeof(read_buff)) {
        still_reading = 0;
      }
      HAL_USART_Transmit(&husart1, (uint8_t*) "Writing next buffer to flash\n", sizeof("Writing next buffer to flash\n"), 1000);
      for (int i = 0; i < bytes_read / 4; i++) {
        hr = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, __app_flash_start__ + buffers_read * 128 + 1 * i, ((uint32_t*)read_buff)[i]);
        if (hr) {
          HAL_USART_Transmit(&husart1, (uint8_t*) "Failed writing to flash\n", sizeof("Failed writing to flash\n"), 1000);
          HAL_USART_Transmit(&husart1, (uint8_t*) "Entering infinite loop\n", sizeof("Entering infinite loop\n"), 1000);
          while (1) {}
        }
      }
      if (bytes_read % 4 > 0) {
        for (int i = 0; i < bytes_read % 4; i++) {
          read_buff[bytes_read + i] = 0xFF;
        }
        hr = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, __app_flash_start__ + buffers_read * 128 + 1 * (bytes_read / 4), ((uint32_t*)read_buff)[bytes_read / 4]);
        if (hr) {
          HAL_USART_Transmit(&husart1, (uint8_t*) "Failed final step writing to flash\n", sizeof("Failed final step writing to flash\n"), 1000);
          HAL_USART_Transmit(&husart1, (uint8_t*) "Entering infinite loop\n", sizeof("Entering infinite loop\n"), 1000);
          while (1) {}
        }
      }
      buffers_read++;
    }

    hr = HAL_FLASH_Lock();
    if (hr) {
      HAL_USART_Transmit(&husart1, (uint8_t*) "Failed to locking flash\n", sizeof("Failed to locking flash\n"), 1000);
      HAL_USART_Transmit(&husart1, (uint8_t*) "Entering infinite loop\n", sizeof("Entering infinite loop\n"), 1000);
      while (1) {}
    }

    fr = f_close(&fil);
    if (fr) {
      HAL_USART_Transmit(&husart1, (uint8_t*) "Failed to close file\n", sizeof("Failed to close file\n"), 1000);
      HAL_USART_Transmit(&husart1, (uint8_t*) "Entering infinite loop\n", sizeof("Entering infinite loop\n"), 1000);
      while (1) {}
    }

    fr = f_mount(0, SDPath, 0);
    if (fr) {
      HAL_USART_Transmit(&husart1, (uint8_t*) "Failed to unmount SD\n", sizeof("Failed to unmount SD\n"), 1000);
      HAL_USART_Transmit(&husart1, (uint8_t*) "Entering infinite loop\n", sizeof("Entering infinite loop\n"), 1000);
      while (1) {}
    }

    HAL_USART_Transmit(&husart1, (uint8_t*) "Everything seems to have worked...\n", sizeof("Everything seems to have worked...\n"), 1000);
    HAL_USART_Transmit(&husart1, (uint8_t*) "Jumping to new app...\n", sizeof("Jumping to new app...\n"), 1000);
    // while (1) {}
    SCB->VTOR = __app_flash_start__;
    start_app(app_start, app_sp);

    // fr = f_open(&fil, "test.txt", FA_READ);
    // if (fr) {
    //   HAL_USART_Transmit(&husart1, (uint8_t*) "error opening\n", sizeof("error opening\n"), 1000);
    // }
    // else {
    //   fr = f_read(&fil, read_buff, sizeof(read_buff), &bytes_read);
    //   if (fr) {
    //     HAL_USART_Transmit(&husart1, (uint8_t*) "error reading\n", sizeof("error reading\n"), 1000);
    //   }
    //   else {
    //     HAL_Delay(1000);
    //     HAL_USART_Transmit(&husart1, (uint8_t*) "Before writing to flash: ", sizeof("Before write to flash: "), 1000);
    //     for (int i = 0; i < 6; i++) {
    //       flash_read_buff[i] = ((uint32_t*) 0x8007000)[i];
    //     }
    //     HAL_USART_Transmit(&husart1, (uint8_t*) flash_read_buff, 24, 1000);
    //      HAL_USART_Transmit(&husart1, (uint8_t*) "Writing to flash: ", sizeof("Writing to flash: "), 1000);
    //      HAL_USART_Transmit(&husart1, read_buff, bytes_read, 1000);
    //     hr = HAL_FLASH_Unlock();
    //      if (hr) {
    //        HAL_USART_Transmit(&husart1, (uint8_t*) "unlock flash error\n", sizeof("unlock flash error\n"), 1000);
    //      }
    //     hr = HAL_FLASHEx_Erase(&erase_conf, &page_error);
    //      if (hr) {
    //        HAL_USART_Transmit(&husart1, (uint8_t*) "erase flash error\n", sizeof("erase flash error\n"), 1000);
    //      }
    //     for (int i = 0; i < 6; i++) {
    //       hr = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, 0x8007000 + 4 * i, ((uint32_t*)read_buff)[i]);
    //       if (hr) {
    //         HAL_USART_Transmit(&husart1, (uint8_t*) "program flash error\n", sizeof("program flash error\n"), 1000);
    //       }
    //     }
    //     hr = HAL_FLASH_Lock();
    //      if (hr) {
    //        HAL_USART_Transmit(&husart1, (uint8_t*) "lock flash error\n", sizeof("lock flash error\n"), 1000);
    //      }
    //      HAL_Delay(1000);
    //      HAL_USART_Transmit(&husart1, (uint8_t*) "After writing to flash: ", sizeof("After writing to flash: "), 1000);
    //     HAL_USART_Transmit(&husart1, (uint8_t*) __app_flash_start__, 24, 1000);
    //     for (int i = 0; i < 6; i++) {
    //       flash_read_buff[i] = ((uint32_t*) 0x8007000)[i];
    //     }
    //     //HAL_USART_Transmit(&husart1, (uint8_t*) flash_read_buff, 24, 1000);
    //   }
    //   hr = f_close(&fil);
    //   if (hr) {
    //     //HAL_USART_Transmit(&husart1, (uint8_t*) "error closing\n", sizeof("error closing\n"), 1000);
    //   }
    //   else {
    //     //HAL_USART_Transmit(&husart1, (uint8_t*) "file closed\n", sizeof("file closed\n"), 1000);
    //   }
    // }

    // HAL_Delay(1000);

    // // unmount
    // fr = f_mount(0, SDPath, 0);
    // if (fr) {
    //   //HAL_USART_Transmit(&husart1, (uint8_t*) "error unmounting\n", sizeof("error unmounting\n"), 1000);
    // }
    // else {
    //   //HAL_USART_Transmit(&husart1, (uint8_t*) "unmounted fs\n", sizeof("unmounted fs\n"), 1000);
    // }

    // HAL_Delay(1000);

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

/* USER CODE BEGIN 4 */

static void start_app(uint32_t pc, uint32_t sp) {
    __asm("           \n\
          msr msp, r1 /* load r1 into MSP */\n\
          bx r0       /* branch to the address at r0 */\n\
    ");
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
