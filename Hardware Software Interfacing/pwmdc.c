/* pwmdc.c : to implement a pwm using TIM1
 * By      : Nivedita Rajendran
 * Date    : 2018/10/31
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>

#include "common.h"

#define SQUARE(X) X*X

TIM_HandleTypeDef tim1;
uint32_t flag1 = 0;
uint32_t flag2 = 0;

/* converts one range of values to another range
 */
uint32_t mapFn(uint32_t x, uint32_t range1Low,
	       uint32_t range1High,uint32_t range2Low,uint32_t range2High)
{
  return (x - range1Low) * (range2High - range2Low) /
    (range1High - range1Low) + range2Low;
}

/* linear breathing for led at channel 1 */
void breathLinear(void) {
  static uint32_t value = 0;
  uint32_t duty;
  if(flag1 == 1) {
    duty = abs(((value++) % 2000) - 1000);
    /* https://stackoverflow.com/questions/1073606/
     *is-there-a-one-line-function-that-generates-a-triangle-wave/
     *1073628 
     */
    TIM1->CCR1 = duty;
    HAL_Delay(1);
  }
  if(value > 2000) {
    value = 0;
  }
}

/* parabolic breath pattern at channel 3 */
void breathParabola(void) {
  static uint32_t value = 0;
  uint32_t para = 0;
  if(flag2 == 1) {
    para = value++ % 1000;
    /* https://stackoverflow.com/questions/1073606/
     * is-there-a-one-line-function-that-generates-a-triangle-wave/
     * 1073628 
     */
    para = SQUARE(para);
    TIM1->CCR3 = para;
    HAL_Delay(10);
    if(para > 1000) {
      value = 0;
    }
  }
}

/* Initialise the timer 1 channels in PWM */ 
void pwminit(void) {
  
  __HAL_RCC_GPIOA_CLK_ENABLE();
  
  GPIO_InitTypeDef GPIO_InitStruct;
  
  GPIO_InitStruct.Pin = (GPIO_PIN_9 | GPIO_PIN_8
			 | GPIO_PIN_10);
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 6;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  /* Initialize PWM */
  __HAL_RCC_TIM1_CLK_ENABLE();
  
  tim1.Instance = TIM1;
  tim1.Init.Prescaler =
    HAL_RCC_GetPCLK2Freq() / 1000000 - 1;
  tim1.Init.CounterMode
    = TIM_COUNTERMODE_UP;
  tim1.Init.Period
    = 1000;
  tim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tim1.Init.RepetitionCounter = 0;
  HAL_TIM_Base_Init(&tim1);
  
  /* Configure Channels */
  TIM_OC_InitTypeDef sConfig;
  
  sConfig.OCMode
    = TIM_OCMODE_PWM1;
  sConfig.Pulse
    = 500;
  sConfig.OCPolarity
    = TIM_OCPOLARITY_HIGH;
  sConfig.OCNPolarity = TIM_OCNPOLARITY_LOW;
  sConfig.OCFastMode
    = TIM_OCFAST_DISABLE;
  sConfig.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfig.OCNIdleState =TIM_OCNIDLESTATE_RESET;
  HAL_TIM_PWM_ConfigChannel(&tim1,&sConfig,
			    TIM_CHANNEL_1);
  HAL_TIM_PWM_ConfigChannel(&tim1,&sConfig,
			    TIM_CHANNEL_2);
  HAL_TIM_PWM_ConfigChannel(&tim1,&sConfig,
			    TIM_CHANNEL_3);
  
  HAL_TIM_PWM_Start(&tim1,TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&tim1,TIM_CHANNEL_2);
  HAL_TIM_PWM_Start(&tim1,TIM_CHANNEL_3);
}

void pwm(uint32_t channel, uint32_t value) {
  
  if(value == 1000) {
    value = 1000;
  } else if(value < 0) {
    value = 0;
  }
  
  if(channel == 1) {
    TIM1->CCR1 = value;
  }
  
  if(channel == 2) {
    TIM1->CCR2 = value;
  }
  
  if(channel == 3) {
    TIM1->CCR3 = value;
  }
}

ParserReturnVal_t CmdPWMinit(int action) {
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("pwminit\n\n"
	   "This command initialise the PWM in timer1\n"
	   );
    
    return CmdReturnOk;
  }
  
  pwminit();
  
  return CmdReturnOk;
}

ADD_CMD("pwminit",CmdPWMinit,"                Initialise the"
	"PWM in timer 1")

ParserReturnVal_t CmdPWM(int action) {
  
  int rc;
  uint32_t channel;
  uint32_t val;
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("pwm\n\n"
	   "This command sets the PWm at timer 1 \n"
	   );
    
    return CmdReturnOk;
  }
  
  rc = fetch_uint32_arg(&channel);
  if(rc) {
    printf("Enter value for channel \n");
    return CmdReturnBadParameter1;
  }
  rc = fetch_uint32_arg(&val);
  if(rc) {
    printf("Enter value for duty cycle in percentage \n");
    return CmdReturnBadParameter2;
  }
  if(val > 100 || val < 0) {
    printf("Please enter a percentage between 0 and 100 \n");
    return CmdReturnBadParameter2;
  }
  
  val = mapFn(val,0,100,0,1000);
  pwm(channel,val);
  
  return CmdReturnOk;
}

ADD_CMD("pwm",CmdPWM,"<channel> <val>     sets the timer in pwm")

ParserReturnVal_t CmdBreathLinear(int action) {  
  uint32_t flag;
  int rc;

  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("breathLinear\n\n"
	   "This command makes the led in channel 1 'breath' linear \n"
	   );
    
    return CmdReturnOk;
  }
  
  rc = fetch_uint32_arg(&flag);
  if(rc) {
    printf("Enter 0|1 to turn OFF or ON \n");
    return CmdReturnBadParameter1;
  }
  
  if(flag == 1) {
    flag1 = 1;
  }
  else {
    flag1 = 0;
  }
  
  return CmdReturnOk;
}

ADD_CMD("breathlinear",CmdBreathLinear,"0 | 1     stop or start LED 'breathing'")

ParserReturnVal_t CmdBreathPara(int action) {  
  uint32_t flag;
  int rc;

  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("breathpara\n\n"
	   "This command makes the led in channel 2 'breath' Parabolic \n"
	   );
    
    return CmdReturnOk;
  }

  rc = fetch_uint32_arg(&flag);
  if(rc) {
    printf("Enter 0|1 to turn OFF or ON \n");
    return CmdReturnBadParameter1;
  }

  if(flag == 1) {
    flag2 = 1;
  }
  else {
    flag2 = 0;
  }
  
  return CmdReturnOk;
}

ADD_CMD("breathpara",CmdBreathPara,"0 | 1     stop or start LED 'breathing'")
