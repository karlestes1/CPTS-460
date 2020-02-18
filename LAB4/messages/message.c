//#include "type.h"
//#include "defines.h"

/******** message.c file ************/
#define NMBUF 10
struct semaphore nmbuf, mlock;
MBUF mbuf[NMBUF], *mbufList; // mbufs buffers and mbufList

//nmbuf = NMBUF; // number of free mbufs
//struct semaphore PROC.nmsg = 0; // for proc to wait for messages

int menqueue(MBUF **queue, MBUF *p)
{ // enter p into queue by priority
  MBUF *q  = *queue;
  if (q==0){
    *queue = p;
    p->next = 0;
    return;
  }
  if ((*queue)->priority < p->priority){
    p->next = *queue;
    *queue = p;
    return;
  }
  while (q->next && p->priority <= q->next->priority){
    q = q->next;
  }
  p->next = q->next;
  q->next = p;
}

MBUF *mdequeue(MBUF **queue)
{
  MBUF *p = *queue;
  if (p)
    *queue = p->next;
  return p;
}

int msg_init()
{
    int i;
    MBUF *mp;
    printf("mesg_init()\n");
    mbufList = 0;
    for (i = 0; i < NMBUF; i++)
        // initialize mbufList
        menqueue(&mbufList, &mbuf[i]); // all priority=0, so use menqueue()
    nmbuf.value = NMBUF;
    nmbuf.queue = 0; // counting semaphore
    mlock.value = 1;
    mlock.queue = 0; // lock semaphore
}

int P(struct semaphore *s) 
{

    int SR = int_off();
    s->value--;
    if (s->value < 0)
        block(s);
    int_on(SR);
}

int V(struct semaphore *s)
{
    int SR = int_off();
    s->value++;
    if (s->value <= 0)
        signal(s);
    int_on(SR);
}

int block(struct semaphore *s)
{
    running->status = BLOCK;
    enqueue(&s->queue, running);
    tswitch();
}

int signal(struct semaphore *s)
{
    PROC *p = dequeue(&s->queue);
    p->status = READY;
    enqueue(&readyQueue, p);
}

MBUF mbuf[NMBUF];
// NMBUF = number of mbufs


MBUF *get_mbuf()
// return a free mbuf pointer
{
    P(nmbuf);
    // wait for free mbuf
    P(&mlock);
    MBUF *mp = dequeue(mbufList);
    V(&mlock);
    return mp;
}
int put_mbuf(MBUF *mp)
// free a used mbuf to freembuflist
{
    P(&mlock);
    enqueue(mbufList, mp);
    V(&mlock);
    V(nmbuf);
}
int send(char *msg, int pid) // send msg to partet pid
{
    if (checkPid() < 0)
        // validate receiving pid
        return -1;
    PROC *p = &proc[pid];
    MBUF *mp = get_mbuf();
    mp->pid = running->pid;
    mp->priority = 1;
    strcpy(mp->contents, msg);
    P(&p->mQlock);
    menqueue(&p->mQueue, mp);
    V(&p->mQlock);
    V(&p->nmsg);
    return 0;
}
int recv(char *msg)
// recv msg from own msgqueue
{
    P(&running->nmsg);
    P(&running->mQlock);
    MBUF *mp = mdequeue(&running->mQueue);
    V(&running->mQlock);
    strcpy(msg, mp->contents);
    int sender = mp->pid;
    put_mbuf(mp);
    return sender;
}