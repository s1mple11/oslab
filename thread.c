#include "thread.h"
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>

static struct thread_struct init_task = {0, NULL, THREAD_RUNNING, 0, 0, 15, 15, {0}};

struct thread_struct *current = &init_task; //��ʼ���߳�

struct thread_struct *task[THR_TASKS] = {&init_task,}; //��ʼ���߳��б�

void schedule();  

//�߳���������
void start(struct thread_struct *tsk)
{
    tsk->thread_func();   
    tsk->status = THREAD_EXIT;  //�˳��ϸ��߳�
    printf("Thread %d excited\n",tsk->id);
    //task[tsk->id] = NULL; 
    schedule();           
}

int thread_create(int *thread_id, void (*start_routine)()) // start_routine ���̹߳��̺���
{
    int id = -1; //��ʼ���߳�id
    //���̷߳���һ���ṹ��
    struct thread_struct *tsk = (struct thread_struct *)malloc(sizeof(struct thread_struct));
    //�����������һ����λ��
    while (++id < THR_TASKS && task[id]);
    //δ�ҵ�����-1
    if (id == THR_TASKS)return -1;
    //���߳̽ṹ��ָ��ŵ��������
    task[id] = tsk; 

    if (thread_id)  //������۵������ŵ����߳� id �ţ����ص� thread_id
        *thread_id = id;

    //��ʼ���߳̽ṹ��
    tsk->id = id;                              
    tsk->thread_func = start_routine;         
    int *stack = tsk->stack;                  
    tsk->esp = (int)(stack + STACK_SIZE - 11); 
    tsk->status = THREAD_RUNNING;       
    tsk->wakeuptime = 0; //����ʱ��                     
    tsk->timeslice_counter = 15; //ʱ��Ƭ                   
    tsk->priority = 15; //���ȼ�                   

    // ��ʼ switch_to_next ����ջ֡
    // �͵�ַ
    stack[STACK_SIZE - 11] = 7;         // eflags
    stack[STACK_SIZE - 10] = 6;         // eax
    stack[STACK_SIZE - 9] = 5;          // edx
    stack[STACK_SIZE - 8] = 4;          // ecx
    stack[STACK_SIZE - 7] = 3;          // ebx
    stack[STACK_SIZE - 6] = 2;          // esi
    stack[STACK_SIZE - 5] = 1;          // edi
    stack[STACK_SIZE - 4] = 0;          // old ebp
    stack[STACK_SIZE - 3] = (int)start; // ret to start
    //start ����ջ֡���ս��� start ����
    stack[STACK_SIZE - 2] = 100;        // ret to unknown����� start ִ�н����������߳̽���
    stack[STACK_SIZE - 1] = (int)tsk;   // start �Ĳ���
    // �ߵ�ַ
    return 0;
}

int thread_join(int thread_id)
{
    //����߳�û�˳����͵���
    while(task[thread_id]->status != THREAD_EXIT)
    {
        schedule();
    }
    free(task[thread_id]);   //�ͷ��ڴ�
    task[thread_id] = NULL;  
}
