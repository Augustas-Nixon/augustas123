//producer
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
struct data
{
    int buf[5];
    sem_t full,empty,mutex;
}*d;
void main()
{
    int fd,k,j=0,t,i=0;
    fd=shm_open("/shared",O_CREAT|O_RDWR,0666);
    ftruncate(fd,sizeof(struct data));
    d=mmap(NULL,sizeof(struct data),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    sem_init(&(d->mutex),1,1);
    sem_init(&(d->empty),1,5);
    sem_init(&(d->full),1,0);
    while(1)
    {
      sem_getvalue(&(d->empty),&k);
      if(j==5)
      {
          j=0;
      }
       if(k==0)
       {
           printf("Buffer is full\n");
       }
       else
       {
           sem_wait(&(d->mutex));
           sem_wait(&(d->empty));
           (d->buf)[j]=i;
           sem_post(&(d->full));
           printf("%d produced\n",i);
           j++;
           i++;
           sem_getvalue(&(d->full),&t);
           sem_post(&(d->mutex));
           sem_getvalue(&(d->empty),&k);
       }
        sleep(1);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Consumer
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
struct data
{
    int buf[5];
    sem_t full,empty,mutex;
}*d;
void main()
{
    int fd,k=0,j=0,t,i=0,item;
    fd=shm_open("/shared",O_CREAT|O_RDWR,0666);
    ftruncate(fd,sizeof(struct data));
    d=mmap(NULL,sizeof(struct data),PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
    while(1)
    {
      sem_getvalue(&(d->full),&t);
      if(j==5)
      {
          j=0;
      }
       if(t==0)
       {
           printf("Buffer is empty\n");
       }
       else
       {
           sem_wait(&(d->mutex));
           sem_wait(&(d->full));
           sem_post(&(d->empty));
           item=(d->buf)[j];
           j++;
           printf("%d consumed\n",item);
           sem_post(&(d->mutex));
       }
        sleep(1);
    }
}