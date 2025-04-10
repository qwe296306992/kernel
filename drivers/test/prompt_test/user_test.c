#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>

void *hi_prio_t1(void *p)
{
  long startAt, stopAt;

  startAt = time(NULL);
    printf("T1 start at %ld\n",startAt);

    sleep(3);
  stopAt = time(NULL);

    printf("T1 stop at %ld. elapse: %ld seconds\n", stopAt, stopAt-startAt);
    return NULL;
}

void *low_prio_t2(void *p)
{
	char buf[20];
  	char errmsg[256];
  	int fd, len;
  	long startAt, stopAt;
	
  	startAt = time(NULL);
    printf("T2 start at %ld\n",startAt);

    sleep(1);

    if((fd=open("/dev/demo", O_RDONLY))<0) {
    	strerror_r(errno, errmsg, 256);
    	printf("T2 open /dev/demo failed. error:%s\n", errmsg);
    	return NULL;
  	}

    if((len=read(fd, buf, 19))<0) {
    strerror_r(errno, errmsg, 256);
    printf("T2 read failed. error:%s\n", errmsg);
    return NULL;
  }
  buf[len]='\0';

  stopAt = time(NULL);
    printf("T2 stop at %ld. elapse: %ld seconds. buf=[%d:\"%s\"]\n", stopAt, stopAt-startAt, len, buf);
    return NULL;
}

int main()
{
    pthread_t t1, t2;
    pthread_attr_t attr;
    struct sched_param param;
      
    pthread_attr_init(&attr);
    pthread_attr_setinheritsched (&attr, PTHREAD_EXPLICIT_SCHED);
    pthread_attr_setschedpolicy(&attr, SCHED_RR);
    
    param.sched_priority = 50;
    pthread_attr_setschedparam(&attr, &param);
    pthread_create(&t1, &attr, hi_prio_t1, NULL);
    
    param.sched_priority = 30;
    pthread_attr_setschedparam(&attr, &param);
    pthread_create(&t2, &attr, low_prio_t2, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}