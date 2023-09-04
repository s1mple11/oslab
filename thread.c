#include "thread.h"
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>

static struct thread_struct init_task = {0, NULL, THREAD_RUNNING, 0, 0, 15, 15, {0}};

struct thread_struct *current = &init_task; //初始化线程

struct thread_struct *task[THR_TASKS] = {&init_task,}; //初始化线程列表

void schedule();  

//线程启动函数
void start(struct thread_struct *tsk)
{
    tsk->thread_func();   
    tsk->status = THREAD_EXIT;  //退出上个线程
    printf("Thread %d excited\n",tsk->id);
    //task[tsk->id] = NULL; 
    schedule();           
}

int thread_create(int *thread_id, void (*start_routine)()) // start_routine 是线程过程函数
{
    int id = -1; //初始化线程id
    //给线程分配一个结构体
    struct thread_struct *tsk = (struct thread_struct *)malloc(sizeof(struct thread_struct));
    //在任务槽中找一个空位置
    while (++id < THR_TASKS && task[id]);
    //未找到返回-1
    if (id == THR_TASKS)return -1;
    //将线程结构体指针放到空任务槽
    task[id] = tsk; 

    if (thread_id)  //将任务槽的索引号当作线程 id 号，传回到 thread_id
        *thread_id = id;

    //初始化线程结构体
    tsk->id = id;                              
    tsk->thread_func = start_routine;         
    int *stack = tsk->stack;                  
    tsk->esp = (int)(stack + STACK_SIZE - 11); 
    tsk->status = THREAD_RUNNING;       
    tsk->wakeuptime = 0; //唤醒时间                     
    tsk->timeslice_counter = 15; //时间片                   
    tsk->priority = 15; //优先级                   

    // 初始 switch_to_next 函数栈帧
    // 低地址
    stack[STACK_SIZE - 11] = 7;         // eflags
    stack[STACK_SIZE - 10] = 6;         // eax
    stack[STACK_SIZE - 9] = 5;          // edx
    stack[STACK_SIZE - 8] = 4;          // ecx
    stack[STACK_SIZE - 7] = 3;          // ebx
    stack[STACK_SIZE - 6] = 2;          // esi
    stack[STACK_SIZE - 5] = 1;          // edi
    stack[STACK_SIZE - 4] = 0;          // old ebp
    stack[STACK_SIZE - 3] = (int)start; // ret to start
    //start 函数栈帧，刚进入 start 函数
    stack[STACK_SIZE - 2] = 100;        // ret to unknown，如果 start 执行结束，表明线程结束
    stack[STACK_SIZE - 1] = (int)tsk;   // start 的参数
    // 高地址
    return 0;
}

int thread_join(int thread_id)
{
    //如果线程没退出，就调度
    while(task[thread_id]->status != THREAD_EXIT)
    {
        schedule();
    }
    free(task[thread_id]);   //释放内存
    task[thread_id] = NULL;  
}
