/* timer.c : To implement a delay using timers
 * By : Nivedita Rajendran
 */

#include <stdio.h>
#include <stdint.h>

#include "common.h"

TIM_HandleTypeDef tim17;

void timerInit()
{
  __HAL_RCC_TIM17_CLK_ENABLE();
  
  tim17.Instance = TIM17;
  tim17.Init.Prescaler =
    (HAL_RCC_GetPCLK2Freq() / 1000000 - 1);
  tim17.Init.CounterMode =
    TIM_COUNTERMODE_UP;
  tim17.Init.Period = 0xffff;
  tim17.Init.ClockDivision =
    TIM_CLOCKDIVISION_DIV1;
  tim17.Init.RepetitionCounter = 0;
  HAL_TIM_Base_Init(&tim17);
  
  printf("Timer 17 Initialisation Successful \n");
}

/* void TIM17_IRQHandler() */
/* { */
/*   HAL_TIM_IRQHandler(&tim17); */
/*   //HAL_GPIO_TogglePin(GPIOC,GPIO_PIN_0); */

/* } */
/* void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) */
/* { */
/*   HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,1); */
/* } */
/* void INTRInit() */
/* { */
/*   HAL_NVIC_SetPriority(TIM17_IRQn,0,1); */
/*   HAL_NVIC_EnableIRQ(TIM17_IRQn); */
/* } */

void GPIOInit()
{
  GPIO_InitTypeDef GPIO_InitStruct;
  
  GPIO_InitStruct.Pin = (GPIO_PIN_0 | GPIO_PIN_1
			 | GPIO_PIN_5);
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  printf("GPIO Initialisation Successful\n");
}

void timer(uint32_t val)
{
  HAL_TIM_Base_Start(&tim17); // Starts the timer 17
  
  TIM17->CNT = 0;
  /* Reset counter */
  
  while(TIM17->CNT < val)
    {
      asm volatile ("nop\n");
    }
  
  HAL_TIM_Base_Stop(&tim17); // Stops the timer 17
}
void timerDelay(int delay)
{
  uint32_t x = 0;
  uint32_t y = 0;
  
  x = delay / 65000;
  y = delay % 65000;
  
  for(uint32_t i = 0; i < x; i++)
    {
      timer(65000);
    }
  
  timer(y);
}

ParserReturnVal_t CmdTimerInit(int action)
{
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("timerInit\n\n"
	   "This command initialise the timer 17\n"
	   );
    
    return CmdReturnOk;
  }
  
  timerInit();
  
  return CmdReturnOk;
}

ADD_CMD("timerInit",CmdTimerInit,"                Initialise the timer 17")

/* ParserReturnVal_t CmdINTRInit(int action) */
/* { */

/*   if(action==CMD_SHORT_HELP) return CmdReturnOk; */
/*   if(action==CMD_LONG_HELP) { */
/*     printf("INTRInit\n\n" */
/* 	   "This command initialise the timer interrupt\n" */
/* 	   ); */

/*     return CmdReturnOk; */
/*   } */

/*   INTRInit(); */
  
/*   return CmdReturnOk; */
/* } */

/* ADD_CMD("INTRInit",CmdINTRInit,"                Initialise the timer interrupt") */

ParserReturnVal_t Cmdtimer(int action)
{
  uint32_t val;
  int rc;
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("timer\n\n"
	   "This command turns ON an LED for a time set by the \n"
	   "input value\n"
	   );
    
    return CmdReturnOk;
  }
  
  rc = fetch_uint32_arg(&val);
  if(rc) {
    printf("Must specify GPIO pin (0 - 15)\n");
    return CmdReturnBadParameter2;
  }
  
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,1);
  timerDelay(val);
  HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,0);
  
  return CmdReturnOk;
}

ADD_CMD("timer",Cmdtimer,"<val>           Turns ON the LED for a time specified")

ParserReturnVal_t CmdGPIOInit(int action)
{
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("GPIOInit\n\n"
	   "This command initialise the GPIO\n"
	   );
    
     return CmdReturnOk;
  }
  
  GPIOInit();
  
  return CmdReturnOk;
}

ADD_CMD("GPIOInit",CmdGPIOInit,"                Initialise the GPIO Pin")
