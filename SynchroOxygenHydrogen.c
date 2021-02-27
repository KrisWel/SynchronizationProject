#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define PRODTLENU 5
#define PRODWODORU 5

pthread_mutex_t buf_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t buf_tlen = PTHREAD_COND_INITIALIZER;
pthread_cond_t buf_wodor = PTHREAD_COND_INITIALIZER;
pthread_t tlen[PRODTLENU];
pthread_t wodor[PRODWODORU];          
int lt = 0;
int lw = 0;
char *buf;
int ilosc_wody = 1;
int wolne_tlen = 1;
int wolne_wodor = 1;

void* prod_tlen (void* arg)
{
    while(1)
    {
		if ((pthread_mutex_lock (&buf_mutex)) != 0) {
            perror("Blad zablokowania mutex");
        }
		
		while (!wolne_tlen) pthread_cond_wait (&buf_tlen, &buf_mutex);
		
		wolne_tlen--;
		lt++;
		printf("Wyprodukowano tlen\n");
		if(lw >= 2 && lt >= 1) {
			printf("Wyprodukowano wode nr: %d\n", ilosc_wody);
			lw -= 2;
			lt--;
			ilosc_wody++;
			sleep(1);
		}
		wolne_wodor++;
		
		pthread_cond_signal (&buf_wodor);

        if ((pthread_mutex_unlock (&buf_mutex)) != 0) {
            perror("Blad odblokowania mutex");
        }
    }
}

void* prod_wodor (void* arg) 
{
    while(1)
    {
        if ((pthread_mutex_lock (&buf_mutex)) != 0) {
            perror("Blad zablokowania mutex");
        }
		
		while (!wolne_wodor) pthread_cond_wait (&buf_wodor, &buf_mutex);
		
		wolne_wodor--;
		lw++;
		printf("Wyprodukowano wodor\n");
		if(lw >= 2 && lt >= 1) {
			printf("Wyprodukowano wode nr: %d\n", ilosc_wody);
			lw -= 2;
			lt--;
			ilosc_wody++;
			sleep(1);
		}
		wolne_tlen++;
		
		pthread_cond_signal (&buf_tlen);

        if ((pthread_mutex_unlock (&buf_mutex)) != 0) {
            perror("Blad odblokowania mutex");
        }
    }
}

int main () 
{
    int *num;
    for (int i = 0; i < PRODTLENU; i++) 
    {
        num = (int*)malloc(sizeof(int));
        *num = i;
        pthread_create(&tlen[i], NULL, &prod_tlen, num);
    }
    for (int i = 0; i < PRODWODORU; i++) 
    {
        num = (int*)malloc(sizeof(int));
        *num = i;
        pthread_create(&wodor[i], NULL, &prod_wodor, num);
    }
    for (int i = 0; i < PRODTLENU; i++) pthread_join(tlen[i], NULL);
    for (int i = 0; i < PRODWODORU; i++) pthread_join(wodor[i], NULL);
}