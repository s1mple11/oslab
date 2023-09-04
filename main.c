#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "thread.h"

extern void schedule();
extern void thread_sleep(int seconds);

void function1()
{
    int i = 10;
    while (i--)
    {
        printf("function1 is running\n");
        thread_sleep(3);
    }
}

void function2()
{
    int i = 9;
    while (i--)
    {
        printf("function2 is running\n");
        thread_sleep(3);
    }
}

void function3()
{
    int i = 8;
    while (i--)
    {
        printf("function3 is running\n");
        thread_sleep(3);
    }
}

void function4()
{
    int i = 7;
    while (i--) {
        printf("function4 is running\n");
        thread_sleep(3);
    }
}

void function5()
{
    int i = 6;
    while (i--) {
        printf("function5 is running\n");
        thread_sleep(3);
    }
}

int main()
{

    int thread_id1, thread_id2, thread_id3, thread_id4, thread_id5;
    thread_create(&thread_id1, function1);
    printf("Thread %d has been created.\n", thread_id1);
    thread_create(&thread_id2, function2);
    printf("Thread %d has been created.\n", thread_id2);
    thread_create(&thread_id3, function3);
    printf("Thread %d has been created.\n", thread_id3);
    thread_create(&thread_id4, function4);
    printf("Thread %d has been created.\n", thread_id4);
    thread_create(&thread_id5, function5);
    printf("Thread %d has been created.\n", thread_id5);


    printf("main is running\n");
    int i = 2;
    while (i--) 
    {
        thread_sleep(3);
    }
    thread_join(thread_id1);
    thread_join(thread_id2);
    thread_join(thread_id3);
    thread_join(thread_id4);
    thread_join(thread_id5);
    return 0; 
}
