/*
    ChibiOS - Copyright (C) 2006..2018 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/
/* Submitted by Nivedita Rajendran
 * 2018/12/09
*/
#include "ch.h"
#include "hal.h"
#include "rt_test_root.h"
#include "oslib_test_root.h"

semaphore_t Sem; // data structure for semaphore

/*
 * Green LED blinker thread, times are in milliseconds.
 */
static THD_WORKING_AREA(waThread1, 128);
static THD_FUNCTION(Thread1, arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (true) {
    palClearPad(GPIOA, GPIOA_LED_GREEN);
chSemWait(&Sem); // semaphore thread pauses until another task signals the semaphore
    palSetPad(GPIOA, GPIOA_LED_GREEN);
chSemWait(&Sem);
  }
}

void print(const char *msgp) {

  while (*msgp)
    streamPut(&SD2, *msgp++);
}



/*
 * Application entry point.
 */
int main(void) {
  SerialConfig sdConfig;
  /*
   * System initializations.
   * - HAL initialization, this also initializes the configured device drivers
   *   and performs the board-specific initializations.
   * - Kernel initialization, the main() function becomes a thread and the
   *   RTOS is active.
   */
  halInit();
  chSysInit();
 

  /*
   * Activates the serial driver 2 using the driver default configuration.
   */
  sdConfig.speed = 115200;
  sdStart(&SD2, &sdConfig);

chSemObjectInit(&Sem, 0); // initialise the semaphore
  /*
   * Creates the blinker thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);

  /*
   * Normal main() thread activity, in this demo it does nothing except
   * sleeping in a loop and check the button state.
   */
  while (true) {
    if (!palReadPad(GPIOC, GPIOC_BUTTON)) {
      
chSemSignal(&Sem); // when button is pressed, semaphore is called
    }
    print("Hello world!\r\n");
    chThdSleepMilliseconds(500); 
  }
}
