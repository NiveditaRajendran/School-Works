/* motor.c : dc motor functions 
 * submitted by Nivedita Rajendran
 * Submitted on 2018/11/08
 */


#include <stdio.h>
#include <stdint.h>

#include "common.h"

uint32_t timestamp;
uint32_t flagrun = 0;
uint32_t interval;
static uint32_t timerun = 0;

void motorrun(void) {
  if(flagrun == 1) {
    timerun *= 1000;
    interval = timestamp + timerun;
    if(HAL_GetTick() == interval) {
      flagrun = 0;
      timerun = 0;
      HAL_GPIO_WritePin(GPIOA,(GPIO_PIN_0|GPIO_PIN_1),0);
    }
    else {
      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_0,1);
      HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,0);
    }
  }  
}

void motorInit(void)
{
  int32_t val = 0;
  pwminit();
  
  /* Set up GPIO pins */
  
  __HAL_RCC_GPIOA_CLK_ENABLE();
  
  GPIO_InitTypeDef GPIO_InitStruct;
  
  GPIO_InitStruct.Pin = (GPIO_PIN_0|GPIO_PIN_1);
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = (GPIO_PIN_10|GPIO_PIN_11);
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 0;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

/*Disable motor for sure*/
  HAL_GPIO_WritePin(GPIOA,(GPIO_PIN_0|GPIO_PIN_1),val);
  HAL_GPIO_WritePin(GPIOB,(GPIO_PIN_10|GPIO_PIN_11),val);
}

void motor(uint32_t motor,int32_t speed) {
  GPIO_TypeDef *gpio;
  uint32_t pin1,pin2;
  uint32_t val1,val2;
  uint32_t pwmChannel;
  
  switch(motor){
  case 0: 
    gpio = GPIOA;
    pin1 = GPIO_PIN_0;
    pin2 = GPIO_PIN_1;
    pwmChannel = 1;
 break;
  case 1:
    gpio = GPIOB;
    pin1 = GPIO_PIN_10;
    pin2 = GPIO_PIN_11;
    pwmChannel = 2;
    break;
  default: printf("error ...invalid motor number \n");
    return;
  }
  if(speed > 0){
    val1 = 1;
    val2 = 0;
  }
  else if(speed<0){
    val1 = 0;
    val2 = 1;
  }
  else{
    val1 = 0;
    val2 = 0;
  }
  
  if(speed<0) speed = -speed;
  pwm(pwmChannel,speed);
  HAL_GPIO_WritePin(gpio,pin1,val1);
  HAL_GPIO_WritePin(gpio,pin2,val2);
}

ParserReturnVal_t Cmdmotorinit(int action) {
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("motorinit\n\n"
	   "This command initialise the DCmotor\n"
	   );
    
    return CmdReturnOk;
  }
  
  motorInit();
  
  return CmdReturnOk;
}

ADD_CMD("motorinit",Cmdmotorinit,"                Initialise the DC motor")


ParserReturnVal_t Cmdmotorrun(int action) {
  int rc;
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("motorrun\n\n"
	   "This command runs the dc motor for seconds provided\n"
	   );
    
    return CmdReturnOk;
  }
  
  rc = fetch_uint32_arg(&timerun);
  if(rc) {
    printf("Enter value for time in seconds\n");
    return CmdReturnBadParameter1;
  }
  
  timestamp = HAL_GetTick();
  pwm(1,50);
  flagrun = 1;
  
  return CmdReturnOk;
}

ADD_CMD("motorrun",Cmdmotorrun,"<time>         run the DC motor for given secs")

ParserReturnVal_t Cmdmotor(int action) {

  int rc;
  uint32_t channel;
  int32_t val;
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("motor\n\n"
	   "This command sets the motor\n"
	   );
    
    return CmdReturnOk;
  }

  rc = fetch_uint32_arg(&channel);
  if(rc) {
    printf("Enter value for channel \n");
    return CmdReturnBadParameter1;
  }
  rc = fetch_int32_arg(&val);
  if(rc) {
    printf("Enter value for duty cycle in percentage \n");
    return CmdReturnBadParameter2;
  }

  if (val > 100) {
    val = 100;
  }
  
  motor(channel,val);
  return CmdReturnOk;
}

ADD_CMD("motor",Cmdmotor,"<channel> <val>     control motor speed and direction")





