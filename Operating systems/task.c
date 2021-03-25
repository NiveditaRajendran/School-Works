/*
 * task.c       : Demonstrate a task excecutive capable of adding
 *                killing and running different tasks assigned in a task list
 * Submitted by : Nivedita Rajendran
 * Data         : 2018/10/21
 */

#include <stdio.h>
#include <stdlib.h>

#include "common.h"

#define MAX_TASKS 5
int32_t TaskAdd(void (*f)(void *data), void *data);
void TaskKill(int32_t id);
static int32_t TaskNext(void);
int32_t TaskSwitcher(void);

typedef struct Task_s {
  void (*f)(void *data); /* Task function */
  void *data; /* Private data pointer for this task */
} Task_t;

/* Process list */
Task_t tasks[MAX_TASKS];
int32_t currentTask = -1;

int32_t TaskAdd(void (*f)(void *data), void *data) {
  /* Try to find an empty slot */
  int32_t i;
  for(i=0; i<MAX_TASKS; i++) {
    if(tasks[i].f == NULL) {
      tasks[i].f = f;
      tasks[i].data = data;
      return i;
    }
  }
  /* No slots available, return -1 */
  return -1;
}

void TaskKill(int32_t id) {
  tasks[id].f = NULL;
  tasks[id].data = NULL;
  
}

/* Find the next task to run */
static int32_t TaskNext(void) {
  int32_t i;
  uint32_t count=0;

  i = currentTask;
  
  do {
    i = (i + 1) % MAX_TASKS;
    count++;
  } while((tasks[i].f == NULL)
	  && (count <= MAX_TASKS));
  return (count <= MAX_TASKS) ? i : -1;
}

int32_t TaskSwitcher(void) {
  currentTask = TaskNext();

  if(currentTask < 0) {
    return -1;
  }
  tasks[currentTask].f(tasks[currentTask].data);

  return 1;
}

void task1(void* data) {
  printf("Task 1 is  running \n");
}

void task2(void* data) {
  printf("Task 2 is running\n");
}

void task3(void* data) {
  printf("Task 3 is running\n");
}

void task4(void* data) {
  printf("Task 4 is running\n");
}

void task5(void* data) {
  printf("Task 5 is running\n");
}


ParserReturnVal_t CmdAddTask(int action) {
  int32_t id;
  int rc;
  int32_t ret;
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("Adding task\n\n"
	   "This command adds a task\n"
	   );
    
    return CmdReturnOk;
  }
  rc = fetch_int32_arg(&id);
  if(rc) {
    printf("Enter id \n");
    return CmdReturnBadParameter 1;
  }

  switch(id) {
  case 1:
   ret =  TaskAdd(task1, NULL);
    break;
  case 2:
    ret = TaskAdd(task2, NULL);
    break;
  case 3:
    ret = TaskAdd(task3, NULL);
    break;
  case 4:
    ret = TaskAdd(task4, NULL);
    break;
  case 5:
    ret = TaskAdd(task5, NULL);
    break;
  default:
    printf("Entered task id does not exist.\n");
    break;
  }

  if(ret == -1) {
    printf("Not enough space for task\n");
  }
  
  return CmdReturnOk;
}

ADD_CMD("addtask",CmdAddTask,"<id>                used to add task")

ParserReturnVal_t CmdKillTask(int action) {
  uint32_t id;
  int rc;
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("kill task\n\n"
	   "This command kills a task\n"
	   );
    
    return CmdReturnOk;
  }

  rc = fetch_uint32_arg(&id);
  if(rc) {
    printf("Enter ID \n");
    return CmdReturnBadParameter1;
  }

  TaskKill(id-1);
  
  return CmdReturnOk;
}

ADD_CMD("killtask",CmdKillTask,"<id>                used to kill task")

ParserReturnVal_t CmdTaskSwitch(int action) {
  int32_t i;
  
  if(action==CMD_SHORT_HELP) return CmdReturnOk;
  if(action==CMD_LONG_HELP) {
    printf("task switch\n\n"
	   "This command switches a task\n"
	   );
    
    return CmdReturnOk;
  }
  for (i = 0; i<10; i++) {
     TaskSwitcher();
  }
  
  return CmdReturnOk;
}

ADD_CMD("taskswitch",CmdTaskSwitch,"                used to switch task")
