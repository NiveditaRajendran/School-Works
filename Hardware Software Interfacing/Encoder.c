/* encode.c : implementing Quadrature encoder */

#include <stdio.h>
#include <stdint.h>

#include "common.h"

TIM_HandleTypeDef tim3;
TIM_HandleTypeDef tim4;

void encoderinit() {

  HAL_StatusTypeDef rc;
  
  /* Set up GPIO pins */
  
  __HAL_RCC_GPIOA_CLK_ENABLE();
  
  GPIO_InitTypeDef GPIO_InitStruct;
  
  GPIO_InitStruct.Pin = (GPIO_PIN_6|GPIO_PIN_7);
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 2;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
  GPIO_InitStruct.Pin = (GPIO_PIN_11|GPIO_PIN_12);
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = 10;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  
  __HAL_RCC_TIM3_CLK_ENABLE();
  __HAL_RCC_TIM4_CLK_ENABLE();

  /*initialise time base */
  tim3.Instance = TIM3;
  tim3.Init.Prescaler = 0;
  tim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim3.Init.Period = 0xffff;
  tim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tim3.Init.RepetitionCounter = 0;
  rc = HAL_TIM_Base_Init(&tim3);
  if(rc != HAL_OK) {
    printf("Failed to initialize Timer 3 Base, "
	   "rc=%u\n",rc);
    return;  
  }

  
  tim4.Instance = TIM4;
  tim4.Init.Prescaler = 0;
  tim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  tim4.Init.Period = 0xffff;
  tim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  tim4.Init.RepetitionCounter = 0;
  rc = HAL_TIM_Base_Init(&tim4);
  if(rc != HAL_OK) {
    printf("Failed to initialize Timer 4 Base, "
	   "rc=%u\n",rc);
    return;  
  }

  /*Initialize encoder data structures */

  TIM_Encoder_InitTypeDef encoderConfig;
  encoderConfig.EncoderMode = TIM_ENCODERMODE_TI12;
  encoderConfig.IC1Polarity = 0;
  encoderConfig.IC1Selection = TIM_ICSELECTION_DIRECTTI;
  encoderConfig.IC1Prescaler = 0;
  encoderConfig.IC1Filter = 3;
  encoderConfig.IC2Polarity = 0;
  encoderConfig.IC2Selection = TIM_ICSELECTION_DIRECTTI;
  encoderConfig.IC2Prescaler = 0;
  encoderConfig.IC2Filter = 3;
  
  rc = HAL_TIM_Encoder_Init(&tim3,&encoderConfig);
  if(rc != HAL_OK) {
    printf("Failed to initialize Timer 3 Encoder, "
	   "rc=%u\n",rc);
    return;
  }
  
  rc = HAL_TIM_Encoder_Init(&tim4,&encoderConfig);
  if(rc != HAL_OK) {
    printf("Failed to initialize Timer 3 Encoder, "
	   "rc=%u\n",rc);
    return;
  }
  
  
  rc = HAL_TIM_Encoder_Start(&tim3, TIM_CHANNEL_1);
  if(rc != HAL_OK) {
    printf("Failed to start Timer 3 Encoder, "
	   "rc=%u\n",rc);
    return;
  }
  rc = HAL_TIM_Encoder_Start(&tim3, TIM_CHANNEL_2);
  if(rc != HAL_OK) {
    printf("Failed to start Timer 3 Encoder, "
	   "rc=%u\n",rc);
    return;
  }
  
  rc = HAL_TIM_Encoder_Start(&tim4, TIM_CHANNEL_1);
  if(rc != HAL_OK) {
    printf("Failed to start Timer 4 Encoder, "
	   "rc=%u\n",rc);
    return;
  }
  rc = HAL_TIM_Encoder_Start(&tim4, TIM_CHANNEL_2);
  if(rc != HAL_OK) {
    printf("Failed to start Timer 4 Encoder, "
	   "rc=%u\n",rc);
    return;
  }
}


uint32_t encode(int32_t channel) {

  switch(channel) {
  case 0:
    return TIM3->CNT;
    break;
  case 1:
    return TIM4->CNT;
    break;
  default:
    printf("Enter channel number 0 or 1 \n");
    return 0;
  }
}


ParserReturnVal_t Cmdencoderinit(int action) {
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("encodeinit\n\n"
	   "This command initialises the encoder\n"
	   );
    
    return CmdReturnOk;
  }
  
  encoderinit();
  
  return CmdReturnOk;
}

ADD_CMD("encodeinit",Cmdencoderinit,"                Initialises the encoder")

ParserReturnVal_t Cmdencode(int action) {

  int rc;
  uint32_t channel;
  uint32_t count;
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("encode\n\n"
	   "This command gets the encoder pulse count\n"
	   );
    
    return CmdReturnOk;
  }

  rc = fetch_uint32_arg(&channel);
  if(rc) {
    printf("Enter the channel value \n");
    return CmdReturnBadParameter1;
  }

  count = encode(channel);
  printf("Position : %lu \n", count);
  
  return CmdReturnOk;
}


ADD_CMD("encode",Cmdencode,"<channel>             gets the encoder position")
