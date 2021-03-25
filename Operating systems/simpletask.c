/* simpletask.c: To demonstrate a simple task. 
 * Group 2
 * Members: Nivedita Rajendran
 * 
 */

#include<stdio.h>
#include<stdint.h>

#include "common.h"

static uint32_t Counter;

/* Print a message is the counter is non-zero. Decerment
 * it and return. */
void TaskCounter(void)
{
  if(Counter) {
    printf("The Counter is %u\n", (unsigned)Counter);
    Counter--;
  }
}

/* Monitor command to set the counter variable */
ParserReturnVal_t CmdCount(int mode)
{
  uint32_t count;

  if(mode != CMD_INTERACTIVE) return CmdReturnOk;

  count = 0;
  fetch_uint32_arg(&count);

  Counter += count;

  return CmdReturnOk;
}

ADD_CMD("count", CmdCount,
	"<n>       Add <n> to the message print count");
