#include <sys/select.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include "fsm.h"
//#define GPIO_BUTTON	2
//#define GPIO_LED	3
//#define GPIO_CUP	4
//#define GPIO_COFFEE	5
//#define GPIO_MILK	6
//#define CUP_TIME	250
//#define COFFEE_TIME	3000
//#define MILK_TIME	3000

#define CINCUENTA	50
//#define DEV_TIME	250

enum cofm_state {
	ESPERANDO
};

static int button = 0;//A 1 para devolver. Como un booleano.
static int detecta_moneda=1;//El sensor para detectar moneda

int dinero=0;//Lo que acumula el cajetin
static int moneda=50;//El valor de cada moneda introducida

static int devolucion=0;//Lo que devuelve la maquina si el boton es presionado.

//static int timer = 0;


//static void timer_isr (union sigval arg) {
//	timer = 1;
//}

//static void timer_start (int ms){
//	timer_t timerid;
//	struct itimerspec value;
//	struct sigevent se;
//	se.sigev_notify = SIGEV_THREAD;
//	se.sigev_value.sival_ptr = &timerid;
//	se.sigev_notify_function = timer_isr;
//	se.sigev_notify_attributes = NULL;
//	value.it_value.tv_sec = ms / 1000;
//	value.it_value.tv_nsec = (ms % 1000) * 1000000;
//	value.it_interval.tv_sec = 0;
//	value.it_interval.tv_nsec = 0;
//	timer_create(CLOCK_REALTIME, &se, &timerid);
//	timer_settime(timerid, 0, &value, NULL);
//}

static int button_devolver (fsm_t* this){
	int ret = button;
	button = 0;
	if(ret==1)
		printf("Quiero mi dinero\n");
	return ret;
}
static int dinero_cafe (fsm_t* this){
	int ret = detecta_moneda;
	detecta_moneda = 0;
	//usleep(1*1000);
	if(ret==1)
		printf("El sensor detecto la moneda\n");
	return ret;
}

static void acumula(fsm_t* this){
	dinero+=moneda;
	//usleep(500*1000);
	if(dinero>=CINCUENTA){
		//tengo50=1;
		printf("Hay dinero para el cafe\n");
	}else{
		//tengo50=0;
		printf("Falta dinero para el cafe\n");
	}
	moneda=0;
}

static void devolver(fsm_t* this){
	//timer_start (DEV_TIME);
	devolucion=dinero;//En cuanto se comparta las variables entre ambas maquinas habria que ver si la devolucion es porque nosotros queremos o por si ha terminado ela maquina de cafe y se nos da el cambio => devolucion=dinero-50. O a lo mejor se cobra en la otra maquina y devolvemos de lo que nos llega por la variable compartida.
	printf("Dinero devuelto: %d\n",devolucion);
	dinero=0;
}

// Explicit FSM description
static fsm_trans_t cofm[] = {
	{ ESPERANDO, button_devolver, ESPERANDO, devolver},
	{ ESPERANDO, dinero_cafe, ESPERANDO, acumula },
	{-1, NULL, -1, NULL },
};
// Utility functions, should be elsewhere
// res = a - b
void timeval_sub (struct timeval *res, struct timeval *a, struct timeval *b){
	res->tv_sec = a->tv_sec - b->tv_sec;
	res->tv_usec = a->tv_usec - b->tv_usec;
	//printf("Tiempo a: %d\n",(int)a->tv_usec);
	//printf("Tiempo b: %d\n",(int)b->tv_usec);
	//printf("Tiempo: %d\n",250000-(int)res->tv_usec);
	if (res->tv_usec < 0) {
		--res->tv_sec;
		res->tv_usec += 1000000;
	}
}
// res = a + b
void timeval_add (struct timeval *res, struct timeval *a, struct timeval *b){
	res->tv_sec = a->tv_sec + b->tv_sec
	+ a->tv_usec / 1000000 + b->tv_usec / 1000000;
	res->tv_usec = a->tv_usec % 1000000 + b->tv_usec % 1000000;
}
// wait until next_activation (absolute time)
void delay_until (struct timeval* next_activation){
	struct timeval now, timeout;
	gettimeofday (&now, NULL);
	timeval_sub (&timeout, next_activation, &now);
	select (0, NULL, NULL, NULL, &timeout);
}

// Utility functions, should be elsewhere
// res = a - b
void timeval_sub2 (struct timespec *res, struct timespec *a, struct timespec *b){
	res->tv_sec = a->tv_sec - b->tv_sec;
	res->tv_nsec = a->tv_nsec - b->tv_nsec;
	//printf("Tiempo a: %d\n",(int)a->tv_usec);
	//printf("Tiempo b: %d\n",(int)b->tv_usec);
	printf("Tiempo: %d\n",(int)res->tv_nsec);
	if (res->tv_nsec < 0) {
		--res->tv_sec;
		res->tv_nsec += 1000000;
	}
}
// res = a + b
void timeval_add2 (struct timespec *res, struct timespec *a, struct timespec *b){
	res->tv_sec = a->tv_sec + b->tv_sec
	+ a->tv_nsec / 1000000000 + b->tv_nsec / 1000000000;
	res->tv_nsec = a->tv_nsec % 1000000000 + b->tv_nsec % 1000000000;
}
// wait until next_activation (absolute time)
void delay_until2 (struct timespec* next_activation){
	struct timespec now, timeout;
	clock_gettime(CLOCK_MONOTONIC,&now);
	timeval_sub2 (&timeout, next_activation, &now);
	clock_nanosleep(CLOCK_MONOTONIC,0,&timeout,NULL);
}

int main (){
	struct timeval clk_period = { 0, 250 * 1000 };
	struct timeval next_activation;
	
	struct timespec tiempo;
	struct timespec clk = { 0, 250 * 1000000 };
	
	fsm_t* cofm_fsm = fsm_new(cofm);
	//wiringPiSetup();
	//pinMode (GPIO_BUTTON, INPUT);
	//wiringPiISR (GPIO_BUTTON, INT_EDGE_FALLING, button_isr);
	//pinMode (GPIO_CUP, OUTPUT);
	//pinMode (GPIO_COFFEE, OUTPUT);
	//pinMode (GPIO_MILK, OUTPUT);
	//pinMode (GPIO_LED, OUTPUT);
	//digitalWrite (GPIO_LED, HIGH);
	//gettimeofday (&next_activation, NULL);
	
	clock_gettime(CLOCK_MONOTONIC,&tiempo);
	while (1) {
		fsm_fire(cofm_fsm);
		//timeval_add (&next_activation, &next_activation, &clk_period);
		//delay_until (&next_activation);
		timeval_add2 (&tiempo, &tiempo, &clk);
		delay_until2 (&tiempo);
	}
}
