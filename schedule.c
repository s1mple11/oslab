#include "thread.h"
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <stdio.h>

extern struct thread_struct *current;         
extern struct thread_struct *task[THR_TASKS]; 

//上下文切换函数，在switch.s中实现
void switch_to_next(struct thread_struct *next);

// 取时间
static unsigned int gettime() 
{
    struct timeval tv;
    if (gettimeofday(&tv, NULL) < 0)
    {
        perror("gettimeofday");
        exit(-1);
    }
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

//pick函数用来调度下一个要运行的线程，使用RR算法
static struct thread_struct *pick()
{
    int i, next, c;

    // 是否存在休眠进程能够被唤醒
    for (i = 0; i < THR_TASKS; ++i)
    {
        if (task[i] && task[i]->status != THREAD_EXIT && gettime() > task[i]->wakeuptime)
        {
            task[i]->status = THREAD_RUNNING;
        }
    }

    // 基于优先级的时间片轮转
    while (1)
    {
        c = -1;
        next = 0;
        for (i = 0; i < THR_TASKS; ++i)
        {
            if (!task[i])
                continue;
            if (task[i]->status == THREAD_RUNNING && task[i]->timeslice_counter > c)
            {
                c = task[i]->timeslice_counter;
                next = i;
            }
        }
        if (c)
            break;

        // 如果所有 THREAD_RUNNING 任务时间片都是 0，重新调整时间片的值
        if (c == 0)
        {
            for (i = 0; i < THR_TASKS; ++i)
            {
                if (task[i])
                {
                    // timeslice_counter = timeslice_counter/2 + priority
                    task[i]->timeslice_counter = task[i]->priority + (task[i]->timeslice_counter >> 1);
                }
            }
        }
    }

    return task[next]; //返回下一个线程的结构体
}

//调度函数
void schedule()
{
    struct thread_struct *next = pick();
    if (next)
    {
        switch_to_next(next);
        //printf("switch_to_next thread:\n");
        //printf("Thread id= %d \n", next->id);
    }
}

//休眠
void thread_sleep(int seconds)
{ 

    //设计当前线程的唤醒时间
    current->wakeuptime = gettime() + 1000 * seconds;

    //将当前线程标记为休眠状态
    current->status = THREAD_SLEEP;

    //准备调度
    schedule();
}

// 时钟中断函数，每一个嘀嗒数自动运行一次
static void do_timer()
{ 
    // 当前线程的时间片的值减1. 当小于等于0则进行调度。
    if (--current->timeslice_counter > 0)
        return;
    current->timeslice_counter = 0;
    schedule();
}

__attribute__((constructor)) //使用setitimer每10ms发送一次sigalrm，在 main 函数前执行。
static void
init()
{
    struct itimerval value;
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = 1000;
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = 1000 * 10; // 时钟的嘀嗒数为10 ms
    if (setitimer(ITIMER_REAL, &value, NULL) < 0)
    {
        perror("setitimer");
    }
    signal(SIGALRM, do_timer);
}
