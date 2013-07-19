#include <pthread.h>
#include <stdio.h>

/* This is our thread function.  It is like main(), but for a thread*/
void *threadFunc(void *arg)
{
    int* str = (int *)arg;
    int local = *str;
    int i = 0;

    str=(int *)arg;

    while(i < 110000 )
    {
        usleep(1);
        printf("threadFunc says: %d\n",local);
        ++i;
    }

    return NULL;
}

int main(void)
{
    pthread_t pth;  // this is our thread identifier
    int i = 0;

    int j;
    for (j = 1; j < 4; j++) {
        pthread_create(&pth,NULL,threadFunc, &j);
    }
    
    while(i < 110000)
    {
        usleep(1);
        printf("main is running...\n");
        ++i;
    }

    printf("main waiting for thread to terminate...\n");
    pthread_join(pth,NULL);

    return 0;
}