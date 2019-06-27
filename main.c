
//Sleeping Barber Problem using semaphore
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include<limits.h>
#include<semaphore.h>
 
#define N 5 //no. of Chairs
 
int customerIDarray[N]; 

time_t end_time;
sem_t mutex,customers,barbers;
int count=0;    //The number of customers waiting for haircuts
 
void *cutHair(void *arg);
void *getHairCut(void *arg);
int smallest(int* values, int count);
int find_index(int a[], int num_elements, int value);
int main(int argc,char *argv[])
{
    pthread_t barber, *customerThreads;
    int status=0, i, ch;
    end_time=time(NULL)+20;/*Barber Shop Hours is 20s*/
 
    /*Semaphore initialization*/
    sem_init(&mutex,0,1);
    sem_init(&customers,0,0);
    sem_init(&barbers,0,1);

    for (i = 0; i < N; ++i) // Using for loop we are initializing
    {
      customerIDarray[i] = 9999; //Default IDs 9999 specifies no customer

    }
 
    printf("Enter the number of threads you want to create : ");
    scanf("%d",&ch);
    customerThreads=(pthread_t *)malloc(ch * sizeof(pthread_t ));
    for(i=1;i<=ch;i++)
    { 
      customerIDarray[i-1] = ((i*3)+rand())/1000000; //random ID for Customer to show FIFO
      pthread_create(&customerThreads[i],NULL,getHairCut,(void *) &customerIDarray[i-1]); //Default Attributes
    }
    pthread_create(&barber,NULL,cutHair,NULL);
    for(i=0;i<ch;i++)
    {
        pthread_join(customerThreads[i],NULL);
    }

    pthread_join(barber,NULL);
    free(customerThreads);
    exit(0);
}
 
void *cutHair(void *arg) //Barber's Function
{
  while(time(NULL)<end_time || count >0)
  {
    sem_wait(&customers);/*P(customers)*/            
    sem_wait(&mutex);/*P(mutex)*/
    count--;
    printf("Barber Cutting Hair : Customer count %i\n",count);
    sem_post(&mutex);/*V(mutex)*/
    sem_post(&barbers);/*V(barbers)*/
  }
  printf("Shop Closed\n");
}
 
void *getHairCut(void *arg) //Customer's Function
{
  int customerID = *((int *)arg);
  int indexOfcustomerID = find_index(customerIDarray,N,customerID); 
  printf("Customer with ID %i entering the Shop\n",customerID);
  if(count < N && indexOfcustomerID != -1)
  {
    count++;
    while(time(NULL)<end_time)
    {
      int smallestValue = smallest(customerIDarray,N);
      sem_wait(&mutex);/*P(mutex)*/
      if(smallestValue == customerID)
      {
        printf("CustomerID = %i : Ready to Get hair Cut : Customer count = %i\n",customerID,count);
        sem_post(&mutex);
        sem_post(&customers);
        customerIDarray[indexOfcustomerID] = 9999;
        sem_wait(&barbers);
        return NULL;
      } else {
        sem_post(&mutex);
      }
    }
  } else {
    printf("No seats Available - Customer with ID:%i left\n",customerID);
    sem_post(&mutex);
    return NULL;
  }
}


int smallest(int* values, int count) 
{
        int smallest_value = INT_MAX;
        int ii = 0;
        for (; ii < count; ++ii)
        {
                if (values[ii] < smallest_value)
                {
                        smallest_value = values[ii];
                }
        }
        return smallest_value;
}

int find_index(int a[], int num_elements, int value)
{
   int i;
   for (i=0; i<num_elements; i++)
   {
   if (a[i] == value)
   {
      return(i);  /* it was found */
   }
   }
   return(-1);  /* if it was not found */
}
}
