#include<pthread.h>
#include<time.h>
#include<iostream>

using namespace std;

#define AS 2000000000
#define NT 4

long long a[AS];
long long global_idx = 0;
long long th_sum = 0, sum = 0;
pthread_mutex_t mutex1;
int err;

void *do_work(void * tid)
{
    long long i, start, mytid, end;
    long long local;
    mytid = (long) tid;
    start = mytid * AS/NT;
    end = start + AS / NT;
    for(i=start; i<end;i++)
    {
        local += a[i];
    }

    pthread_mutex_lock(&mutex1);
    th_sum+=local;
    pthread_mutex_unlock(&mutex1);

    return NULL;
}

int main()
{
    clock_t th_start, th_finish, start, finish;
    double th_duration, duration;

    long long i;
    pthread_t thread[NT];
    pthread_attr_t attr;
    
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

    for(i=0;i<AS;i++)
    {
        a[i] = i+1;
    }

    th_start = clock();


    for(i=0;i<NT;i++)
    {
        err= pthread_create(&thread[i], &attr, do_work, (void*) i); //tid, 속성, function pointer, argument
    }

    for(i=0;i<NT;i++)
    {
        err = pthread_join(thread[i], NULL);
    }
    th_finish = clock();
    th_duration = (double)(th_finish - th_start) /CLOCKS_PER_SEC;
    cout << th_duration << "\n";
    cout << th_sum << "\n";


    start = clock();
    for(int i=0;i<AS;i++)
    {
        sum+=a[i];
    }

    finish = clock();
    duration = (double)(finish - start) /CLOCKS_PER_SEC;
    cout << duration << "\n";
    cout << sum << "\n";

}