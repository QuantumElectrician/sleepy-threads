//
//  main.c
//  sleepy threads
//
//  Created by Владислав Агафонов on 07.10.2017.
//  Copyright © 2017 Владислав Агафонов. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //sleep
#include <pthread.h>

#define TASKS_N 100
#define THREADS_N 10

typedef enum{
    NEW,
    PROCESSING,
    DONE
} statuses;
typedef struct{
    int duration;
    int id;
    statuses status;
    pthread_t worker;
} task_t;
typedef struct{
    pthread_t thread_id;
    int stat;
} thread_stat_t;

pthread_mutex_t lock;
task_t tasks[TASKS_N];
thread_stat_t statistics[THREADS_N];


void* my_thread(void* dummy)
{
    int index = 0;
    pthread_t worker = pthread_self();
    
    //индекс работающего треда
    for (int i = 0; i < THREADS_N; i++)
    {
        if (statistics[i].thread_id == worker)
        {
            index = i;
        }
    }
    
    for (int i = 0; i < TASKS_N; i++)
    {
        pthread_mutex_lock(&lock);
        if (tasks[i].status == NEW)
        {
            tasks[i].status = PROCESSING;
            tasks[i].worker = worker;
        }
        pthread_mutex_unlock(&lock);
        if ( (tasks[i].status == PROCESSING) && (tasks[i].worker == worker) )
        {
            //pthread_mutex_lock(&lock);
            //tasks[i].status = PROCESSING;
            //pthread_mutex_unlock(&lock);
            usleep(tasks[i].duration);
            tasks[i].status = DONE;
            //tasks[i].worker = worker;
            statistics[index].stat++;
        }
       // pthread_mutex_unlock(&lock);
    }
    
    return 0;
}

int main(int argc, const char * argv[])
{
    //statuses status = DONE;
    int result;
    
    srand ( (unsigned int)time(NULL) ); //инициализация рандомайзера
    
    pthread_mutex_init(&lock, NULL); //инициализация лока
    
    //задание задания
    for (int i = 0; i < TASKS_N; i++)
    {
        tasks[i].duration = rand() / 10000;
        tasks[i].id = i;
        tasks[i].status = NEW;
    }
    
    for (int i = 0; i < THREADS_N; i++)
    {
        statistics[i].stat = 0;
    }
    
    //создание тредов
    for (int i = 0; i < THREADS_N; i++) {
        result = pthread_create(&statistics[i].thread_id, NULL, my_thread, NULL);
        if (result) {
            printf("Can't create thread, returned value = %d\n", result);
            exit(-1);
        }
    }
    
    pthread_mutex_destroy(&lock);
    
    for (int i = 0; i < TASKS_N; i++)
    {
        pthread_join(statistics[i].thread_id, NULL);
        printf("TASK: %d STATUS: %d WORKER: %d\n", i, tasks[i].status, (int)tasks[i].worker);
    }
    
    printf("\nSTATISTICS\n\n");
    
    for (int i = 0; i < THREADS_N; i++)
    {
        printf("THREAD %d done %d tasks\n", (int)statistics[i].thread_id, statistics[i].stat);
    }
    
    int sigma = 0;
    for (int i = 0; i < THREADS_N; i++)
    {
        sigma = sigma + statistics[i].stat;
    }
    printf("\nSigma = %d\n", sigma);
    
    return 0;
}
