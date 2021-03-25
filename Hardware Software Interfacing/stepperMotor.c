/* step.c : to implement a stepper motor that runs 
 *          taking the direction and speed
 * By     : Nivedita Rajendran
 * Date   : 2018/10/11
 */

#include <stdio.h>
#include <stdint.h>

#include "common.h"

TIM_HandleTypeDef tim15;
void DACInit(void);
void timerInit(void);
void timerDelay(int delay);
void DACVal(uint32_t val);

void stepperinit() {
  DACInit();
  timerInit();

  GPIO_InitTypeDef GPIO_InitStruct;
  
  GPIO_InitStruct.Pin = (GPIO_PIN_3 | GPIO_PIN_8
			 | GPIO_PIN_4 | GPIO_PIN_5);
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  
  GPIO_InitStruct.Pin = (GPIO_PIN_14);
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Set all the pins to initial value */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3,  0); /* RST  */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,  1); /* PS   */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4,  0); /* OE   */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0); /* STEP */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,  0); /* FR   */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3,  1); /* RST  */

  /* enable dac to provide VREF  */
  DACVal(4095);
}
void stepperinittimer() {
  DACInit();
  timerInit();

  GPIO_InitTypeDef GPIO_InitStruct;
  
  GPIO_InitStruct.Pin = (GPIO_PIN_3 | GPIO_PIN_8
			 | GPIO_PIN_4 | GPIO_PIN_5);
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  
  GPIO_InitStruct.Pin = (GPIO_PIN_14);
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 1;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* Set all the pins to initial value */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3,  0); /* RST  */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_8,  1); /* PS   */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4,  0); /* OE   */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,  0); /* FR   */
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3,  1); /* RST  */

  /* enable dac to provide VREF  */
  DACVal(4095);

  /* Initialise the timer in PWM mode */
  __HAL_RCC_TIM15_CLK_ENABLE();
  tim15.Instance = TIM15;
  tim15.Init.Prescaler =
    HAL_RCC_GetPCLK2Freq() / 1000000 - 1;
  tim15.Init.CounterMode
    = TIM_COUNTERMODE_UP;
  tim15.Init.Period
    = 1000;
  tim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tim15.Init.RepetitionCounter = 0;
  HAL_TIM_Base_Init(&tim15);

  /* Configure the PWM */
  TIM_OC_InitTypeDef sConfig;
  sConfig.OCMode
    = TIM_OCMODE_PWM1;
  sConfig.Pulse
    = 0;
  sConfig.OCPolarity
    = TIM_OCPOLARITY_HIGH;
  sConfig.OCNPolarity = TIM_OCNPOLARITY_LOW;
  sConfig.OCFastMode
    = TIM_OCFAST_DISABLE;
  sConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfig.OCNIdleState =TIM_OCNIDLESTATE_RESET;
  HAL_TIM_PWM_ConfigChannel(&tim15,&sConfig,
			    TIM_CHANNEL_1);
  /* start PWM */
  HAL_TIM_PWM_Start(&tim15,TIM_CHANNEL_1);
}

void stepperenable(uint32_t val) {
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, val ? 0 : 1); /* OE   */
}

void step( int32_t steps, uint32_t delay) {  
  uint32_t i;
  
  if(steps < 0) {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,  1); /* FR   */
    steps = -steps;
  } else {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,  0); /* FR   */
  }
  
  for(i = 0; i < steps ; i++) {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 0); /* STEP */
  timerDelay(delay / 2);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, 1); /* STEP */
  timerDelay(delay / 2);
  }
}

void stepspeed( int32_t dir, uint32_t delay) {  
  
  if(dir < 0) {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,  1); /* FR   */
  } else {
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5,  0); /* FR   */
  }
  TIM15->ARR  = delay;
  TIM15->CCR1 = delay / 2;
}

ParserReturnVal_t Cmdstepperinit(int action) {
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("stepperinit\n\n"
	   "This command initialise the stepper\n"
	   );
    
    return CmdReturnOk;
  }
  
  stepperinit();
  
  return CmdReturnOk;
}

ADD_CMD("stepperinit",Cmdstepperinit,"                Initialise the stepper motor")

ParserReturnVal_t Cmdstepperinittimer(int action) {
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("stepperinittimer\n\n"
	   "This command initialise the stepper and timer\n"
	   );
    
    return CmdReturnOk;
  }
  
  stepperinittimer();
  
  return CmdReturnOk;
}

ADD_CMD("stepperinittimer",Cmdstepperinittimer,"                Initialise the"
	"stepper motor and timer")

ParserReturnVal_t Cmdstepperenable(int action) {
  int rc;
  uint32_t val;
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("stepperenable\n\n"
	   "This command enables the stepper motor\n"
	   );
    
    return CmdReturnOk;
  }
  rc = fetch_uint32_arg(&val);
  if(rc) {
    printf("Enter 0 or 1 \n");
    return CmdReturnBadParameter2;
  }
  
  stepperenable(val);
  
  return CmdReturnOk;
}

ADD_CMD("stepperenable",Cmdstepperenable,"<1 | 0 >         Enables the stepper motor")

ParserReturnVal_t Cmdstep(int action) {
  int32_t val;
  uint32_t delay;
  int rc;
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("timerInit\n\n"
	   "This command initialise the timer 17\n"
	   );
    
    return CmdReturnOk;
  }
  
  rc = fetch_int32_arg(&val);
  if(rc) {
    printf("Enter a value \n");
    return CmdReturnBadParameter1;
  }

  rc = fetch_uint32_arg(&delay);
  if(rc) {
    printf("Enter a delay \n");
    return CmdReturnBadParameter2;
  }

  step(val, delay);
  
  return CmdReturnOk;
}

ADD_CMD("step",Cmdstep,"<step> <delay>                runs the stepper")

ParserReturnVal_t Cmdstepspeed(int action) {
  int32_t dir;
  uint32_t delay;
  int rc;
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("stepspeed\n\n"
	   "This command turns the stepper according"
	   "to speed and direction\n"
	   );
    
    return CmdReturnOk;
  }
  
  rc = fetch_int32_arg(&dir);
  if(rc) {
    printf("Enter a value \n");
    return CmdReturnBadParameter1;
  }
  
  rc = fetch_uint32_arg(&delay);
  if(rc) {
    printf("Enter a delay \n");
    return CmdReturnBadParameter2;
  }
  
  stepspeed(dir, delay);
  
  return CmdReturnOk;
}

ADD_CMD("stepspeed",Cmdstepspeed,"<dir> <delay>                runs the stepper")
