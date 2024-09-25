#include<pthread.h>
#include<time.h>
#include<iostream>

#define SIZE 10
#define NT 100

using namespace std;


struct Item{
    int x;
};

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t nonempty, nonfull=PTHREAD_COND_INITIALIZER;
Item buffer[SIZE];
int put = 0;
int get = 0;



void insert(Item x)
{
    pthread_mutex_lock(&lock);
    while((put > get && (put-get) == SIZE - 1) ||
          (put < get && (get-put) == 1))
    {
        pthread_cond_wait(&nonfull, &lock);
    }

    buffer[put] = x;
    put = (put+1) %SIZE;
    pthread_cond_signal(&nonempty);
    pthread_mutex_unlock(&lock);
}

Item remove()
{
    Item x;
    pthread_mutex_lock(&lock);
    while(put == get)
    {
        pthread_cond_wait(&nonempty, & lock);
    }

    x = buffer[get];
    get = (get + 1) % SIZE;
    pthread_cond_signal(&nonfull);
    pthread_mutex_unlock(&lock);
    return x;
}


void *produce(void* item)
{
    insert(*((Item * )item) );
    
    return NULL;
}

void *consume(void* arg)
{
    Item* item  = (Item*) malloc(sizeof(Item));
    *item = remove();
    return item;
}

int main()
{
    pthread_t thread[NT * 2];
    Item x[NT];
    for(int i=0;i<NT;i++)
    {
        x[i].x = i;
    }

    for(int i=0;i<NT;i++)
    {   
        pthread_create(&thread[i], NULL, produce, (void*) &x[i]);
    }



    for(int i=NT;i< NT*2;i++)
    {
        pthread_create(&thread[i], NULL, consume, (void*) &x[i]);
    }
    for(int i=0;i<NT;i++)
    {
        pthread_join(thread[i], NULL);
    }

    Item result[NT] = {0,};
    for(int i=NT; i<NT * 2; i++) {
        Item* ret;
        pthread_join(thread[i], (void**) &ret);
        result[i - NT] = *ret;
        free(ret);  // malloc으로 할당된 메모리 해제
    }


    for(int i=0;i<NT;i++)
    {
        cout<< result[i].x << " ";
    }

    return 0;
}

