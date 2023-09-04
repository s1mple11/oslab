#include "thread.h"
#include <stdlib.h>
#include <sys/time.h>
#include <signal.h>
#include <stdio.h>

extern struct thread_struct *current;         
extern struct thread_struct *task[THR_TASKS]; 

//�������л���������switch.s��ʵ��
void switch_to_next(struct thread_struct *next);

// ȡʱ��
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

//pick��������������һ��Ҫ���е��̣߳�ʹ��RR�㷨
static struct thread_struct *pick()
{
    int i, next, c;

    // �Ƿ�������߽����ܹ�������
    for (i = 0; i < THR_TASKS; ++i)
    {
        if (task[i] && task[i]->status != THREAD_EXIT && gettime() > task[i]->wakeuptime)
        {
            task[i]->status = THREAD_RUNNING;
        }
    }

    // �������ȼ���ʱ��Ƭ��ת
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

        // ������� THREAD_RUNNING ����ʱ��Ƭ���� 0�����µ���ʱ��Ƭ��ֵ
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

    return task[next]; //������һ���̵߳Ľṹ��
}

//���Ⱥ���
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

//����
void thread_sleep(int seconds)
{ 

    //��Ƶ�ǰ�̵߳Ļ���ʱ��
    current->wakeuptime = gettime() + 1000 * seconds;

    //����ǰ�̱߳��Ϊ����״̬
    current->status = THREAD_SLEEP;

    //׼������
    schedule();
}

// ʱ���жϺ�����ÿһ��������Զ�����һ��
static void do_timer()
{ 
    // ��ǰ�̵߳�ʱ��Ƭ��ֵ��1. ��С�ڵ���0����е��ȡ�
    if (--current->timeslice_counter > 0)
        return;
    current->timeslice_counter = 0;
    schedule();
}

__attribute__((constructor)) //ʹ��setitimerÿ10ms����һ��sigalrm���� main ����ǰִ�С�
static void
init()
{
    struct itimerval value;
    value.it_value.tv_sec = 0;
    value.it_value.tv_usec = 1000;
    value.it_interval.tv_sec = 0;
    value.it_interval.tv_usec = 1000 * 10; // ʱ�ӵ������Ϊ10 ms
    if (setitimer(ITIMER_REAL, &value, NULL) < 0)
    {
        perror("setitimer");
    }
    signal(SIGALRM, do_timer);
}
