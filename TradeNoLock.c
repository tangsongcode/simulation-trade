#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

static unsigned int iFlag=0;
static unsigned long iSed=0;
/* the structure of account */
typedef struct stCustomer
{
    int iNo;
    long lAmt;
} CUSTOMER;
CUSTOMER arrPerson[100];


/* get the random number*/
int GetRandom(int iBeg, int iEnd)
{
    iSed++;
    iSed = iSed%1000000;
    srand(time(NULL)*1000+iSed);
    return (rand()%(iEnd-iBeg))+iBeg;
}

/*initialize 100 accounts*/
int InitAll()
{
    int iSum = sizeof(arrPerson)/sizeof(arrPerson[0]);
    int iIndex = 0;
    for (iIndex=0; iIndex<iSum; iIndex++)
    {
        memset(&arrPerson[iIndex],0x00,sizeof(arrPerson[iIndex]));
        arrPerson[iIndex].iNo = iIndex;
        arrPerson[iIndex].lAmt = 1000;
    }
    return 0;
}


long TotalMoney()
{
    int i = 0;
    long lSum = 0;
    for(i=0;i<100;i++)
    {
        lSum+=arrPerson[i].lAmt;
    }
    return lSum;
}


void* thread_trade()
{
    int iAcct1, iAcct2;
    long lTradeAmt=0;
    long lTemp1 = 0;
    long lTemp2 = 0;

    /* circular trade */
    while(iFlag)
    {
        /*printf("线程[%u]start trade\n",(unsigned int)pthread_self());*/
        iAcct1=GetRandom(0,100);
            lTradeAmt = GetRandom(1,1000);
            if(arrPerson[iAcct1].lAmt<lTradeAmt)
            {
                printf("%u account[%d] current[%ld] sub amt[%ld] is has not enough money,and loop again\n",(unsigned int)pthread_self(),iAcct1,arrPerson[iAcct1].lAmt,lTradeAmt);
                continue;
            }
            lTemp1 = arrPerson[iAcct1].lAmt; 
            arrPerson[iAcct1].lAmt -= lTradeAmt;
            sleep(GetRandom(1,5));
            if((iAcct2=GetRandom(0,100)) && iAcct2!=iAcct1)
            {
                lTemp2 = arrPerson[iAcct2].lAmt; 
                arrPerson[iAcct2].lAmt += lTradeAmt;
                printf("account[%d]current[%ld]sub amt[%ld]left[%ld]\naccount[%d]current[%ld]add amt[%ld] left[%ld]\n",iAcct1,lTemp1,lTradeAmt,arrPerson[iAcct1].lAmt,iAcct2,lTemp2,lTradeAmt,arrPerson[iAcct2].lAmt);
               
                sleep(GetRandom(1,2));
                printf("bank have [%ld]\n",TotalMoney());
            }
            else
            {
                continue;
            }
        }
    printf("thread [%u] exit!",(unsigned int)pthread_self());
    pthread_exit(NULL);
}

void Stop(int iSigno)
{
    printf("Receive Ctrl+c!\n");
    iFlag = 0;
}

int main()
{
    int iCnt = 0;
    pthread_t td[100];
    InitAll();
    /* creat 100 threads */
    iFlag =1;
    signal(SIGINT,Stop);
    for (iCnt=0; iCnt<100; iCnt++)
    {
        pthread_create(&td[iCnt],NULL,thread_trade,NULL);
    }
    for (iCnt=0; iCnt<100; iCnt++)
    {
        pthread_join(td[iCnt], NULL);
    }
    return 0;
}
