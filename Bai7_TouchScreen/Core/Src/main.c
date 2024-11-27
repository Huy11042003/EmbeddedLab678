/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
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
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "gpio.h"
#include "fsmc.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "software_timer.h"
#include "led_7seg.h"
#include "button.h"
#include "lcd.h"
#include "picture.h"
#include "ds3231.h"
#include "sensor.h"
#include "buzzer.h"
#include "touch.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define HEIGHT 170
#define WIDTH 170
int snakeTailX[4096], snakeTailY[4096];
int snakeTailLen = 5;
int gameover = 1;
int key, score;
int x, y, fruitx, fruity;
int navigatorIndex = 0;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
#define INIT 0
#define START 1
#define PLAY 2
#define GAMEOVER 3

int game_Status = INIT;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void system_init();
void test_LedDebug();
void touchProcess();
uint8_t isButtonClear();
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_TIM2_Init();
  MX_SPI1_Init();
  MX_FSMC_Init();
  MX_I2C1_Init();
  MX_TIM13_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  system_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
 touch_Adjust();
lcd_Clear(BLACK);
 while (1)
  {
	  //scan touch screen
	  touch_Scan();
	  //check if touch screen is touched
//	  if(touch_IsTouched() && draw_Status == DRAW){
//            //draw a point at the touch position
//		  lcd_DrawPoint(touch_GetX(), touch_GetY(), RED);
//	  }
	  // 50ms task
	  if(flag_timer2 == 1){
		  flag_timer2 = 0;
		  touchProcess();
		  if(!gameover){
			  logic();
	  //		  draw();
		  }
	  }


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
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void system_init(){
	  timer_init();
	  button_init();
	  lcd_init();
	  touch_init();
	  setTimer2(50);
}

uint8_t count_led_debug = 0;

void test_LedDebug(){
	count_led_debug = (count_led_debug + 1)%20;
	if(count_led_debug == 0){
		HAL_GPIO_TogglePin(DEBUG_LED_GPIO_Port, DEBUG_LED_Pin);
	}
}


uint8_t isButtonStart(){
	if(!touch_IsTouched()) return 0;
	return touch_GetX() > 10 && touch_GetX() < 130 && touch_GetY() > 250 && touch_GetY() < 300;
}

uint8_t isNavigatorTouch(){
	if(!touch_IsTouched()) return 0;
	if (touch_GetX() >= 100 && touch_GetX() <= 140 && touch_GetY() >= 180 && touch_GetY() <= 220) {
		navigatorIndex = 1;
	} else if (touch_GetX() >= 100 && touch_GetX() <= 140 && touch_GetY() >= 280 && touch_GetY() <= 320) {
		navigatorIndex = 2;
	} else if (touch_GetX() >= 50 && touch_GetX() <= 90 && touch_GetY() >= 230 && touch_GetY() <= 270) {
		navigatorIndex = 3;
	} else if (touch_GetX() >= 150 && touch_GetX() <= 190 && touch_GetY() >= 230 && touch_GetY() <= 270) {
		navigatorIndex = 4;
	}
	return 1;
}

void touchProcess(){
	switch (game_Status) {
		case INIT:
                // display blue button
			lcd_Fill(10, 250, 130, 300, GREEN);
			lcd_ShowStr(40, 260, "START", RED, BLACK, 24, 1);
			game_Status = START;
			break;
		case START:
			if(isButtonStart()){
				game_Status = PLAY;
				lcd_Clear(BLACK);
				lcd_DrawRectangle(10, 10, 170, 170, GBLUE);
				lcd_Fill(100, 180, 140, 220, GBLUE); // up arrow 1
				lcd_Fill(100, 280, 140, 320, GBLUE); // down arrow 2
				lcd_Fill(50, 230, 90, 270, GBLUE); // left arrow 3
				lcd_Fill(150, 230, 190, 270, GBLUE); // right arrow 4
				setup();
			}
			break;
		case PLAY:
			if (isNavigatorTouch() != 0) {
//				lcd_ShowIntNum(90, 80, navigatorIndex, 1, RED, BLACK, 24);
				switch(navigatorIndex){
				case 3:
					if(key!=2)
					key = 1;
					break;
				case 4:
					if(key!=1)
					key = 2;
					break;
				case 1:
					if(key!=4)
					key = 3;
					break;
				case 2:
					if(key!=3)
					key = 4;
					break;
	//				case 'x':
	//					gameover = 1;
	//					break;
				}
			}
			if(gameover){
				lcd_Clear(BLACK);
				game_Status = INIT;
				lcd_ShowStr(90, 20, "GAME OVER", RED, BLACK, 24, 1);
			}
			break;
		case GAMEOVER:
			break;
		default:
			break;
	}
}

void setup() {

    // Flag to signal the gameover
    gameover = 0;
    key = 3;
    // Initial coordinates of the snake
    x = WIDTH / 2;
    y = HEIGHT / 2;

    // Initial coordinates of the fruit
    fruitx = rand() % (WIDTH - 5);
    fruity = rand() % (HEIGHT - 5);
	lcd_Fill(fruitx, fruity, fruitx+4, fruity+4, RED);
    while (fruitx < 11)
    	fruitx = rand() % (WIDTH - 5);
    while (fruity < 11)
    	fruity = rand() % (HEIGHT - 5);

    // Score initialzed
//    score = 0;
}
void draw() {

    for (int j = 11; j < HEIGHT; j++) {
        for (int i = 11; i < WIDTH; i++) {

            // Creating snake's head
            if (j == y && i == x)
            	lcd_DrawPoint(i, j, RED);

            // Creating the sanke's food
            else if (j == fruity && i == fruitx)
            	lcd_Fill(i, j, i+4, j+4, RED);

            // Creating snake's body
            else {
                int prTail = 0;
                for (int k = 0; k < snakeTailLen; k++) {
                    if (snakeTailX[k] == i
                        && snakeTailY[k] == j) {
                    	lcd_DrawPoint(i, j, RED);
                        prTail = 1;
                    }
                }
                if (!prTail){
                	if(j >= fruity && i >= fruitx && j <= fruity+4 && i <= fruitx+4)
                		continue;
                	else lcd_DrawPoint(i, j, BLACK);
                }
            }
        }

    }
}

void logic() {

    // Updating the coordinates for continuous
    // movement of snake
    int prevX = snakeTailX[0];
    int prevY = snakeTailY[0];
    int prev2X, prev2Y;
    snakeTailX[0] = x;
    snakeTailY[0] = y;
    lcd_DrawPoint(x, y, RED);
    for (int i = 1; i < snakeTailLen; i++) {
        prev2X = snakeTailX[i];
        prev2Y = snakeTailY[i];
        snakeTailX[i] = prevX;
        snakeTailY[i] = prevY;
        prevX = prev2X;
        prevY = prev2Y;
    }
    lcd_DrawPoint(snakeTailX[snakeTailLen-1], snakeTailY[snakeTailLen-1], BLACK);
    // Changing the direction of movement of snake
    switch (key) {
    case 1:
        x--;
        break;
    case 2:
        x++;
        break;
    case 3:
        y--;
        break;
    case 4:
        y++;
        break;
    default:
        break;
    }

    // If the game is over
    if (x <= 10 || x >= WIDTH || y <= 10 || y >= HEIGHT)
        gameover = 1;

    // Checks for collision with the tail (o)
    for (int i = 0; i < snakeTailLen; i++) {
        if (snakeTailX[i] == x && snakeTailY[i] == y)
            gameover = 1;
    }

    // If snake reaches the fruit
    // then update the score
    if (y >= fruity && x >= fruitx && y <= fruity+4 && x <= fruitx+4) {
    	lcd_Fill(fruitx, fruity, fruitx+4, fruity+4, BLACK);
    	fruitx = rand() % (WIDTH - 5);
    	fruity = rand() % (HEIGHT - 5);
        while (fruitx < 11)
        	fruitx = rand() % (WIDTH - 5);

        // Generation of new fruit
        while (fruity < 11)
        	fruity = rand() % (HEIGHT - 5);
        lcd_Fill(fruitx, fruity, fruitx+4, fruity+4, RED);
//        score += 10;
         snakeTailLen += 5;
    }
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
