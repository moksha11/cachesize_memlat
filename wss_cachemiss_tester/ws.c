#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>

// A shared mutex
pthread_mutex_t mutex;
#define MAX_THREAD 1000
/*#define SIZE (1024*1024*32)*/
/*#define ITERATION 1024*/
#define ITERATION 16
long SIZE=0;

int *global_array;
long global_array_size = 0;
long global_sum  = 0;
int num_threads = 1;
typedef struct {
	int id;
} parm;

unsigned long myrand2(unsigned long *a){
	unsigned long num = *a;
	 num ^= (num << 21);
	 num ^= (num >> 35);
	 num ^= (num << 4);
	 *a = num;
return num;
}

double myrand(double *seed) {
	const double a = 12054.67293087436;
	const double b = 0.3183287916948132;
	long i;

	*seed = a * (*seed) +  b;
	i = (long) *seed;
	*seed -= i;
	return *seed;

} 

//compute loop
inline long compute(int *array, unsigned long size, long count)
{
	/*double seed = (double)(rand()%100)/100;*/
	unsigned long seed  = rand();
	unsigned long i,j,index;
	double randnum;
	long answer = 0;
		for(j = 0; j < count;j++){
	for(i = 0; i < size;i++){
			/*randnum= myrand(&seed);*/
			/*index = randnum * SIZE;*/
		index = (myrand2(&seed)) % size;
		/*index = i;*/
			//printf("%lu %lu\n",index, size);
			answer += array[index];
			/*index++;*/
		}
	}
	return answer;
}

//worker thread
void *worker(void *arg)
{
	parm *p=(parm *)arg;
	long socket_id = p->id / 8; 
	long core_id = p->id % 8;
	long cpu_id = socket_id + core_id*4;
	srand(cpu_id);
	/*long cpu_id = p->id;*/
	unsigned long mask = (1 << cpu_id);
	/*printf("%d %d %x\n",p->id,cpu_id, mask);*/
	pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask);

	unsigned long i,j=0;
	unsigned long local_array_size = (SIZE);
	srand(time(NULL));
	int *local_array = (int*)malloc(sizeof(int)*local_array_size);
	/*for(i=0;i<local_array_size;i++)*/
	/*local_array[i] = i;*/
	/*printf("%d\n",local_array[i]);*/
	long local_sum;

	//parallel fraction
	local_sum += compute(local_array,local_array_size,ITERATION);

	return(NULL);
}

int main(int argc, char* argv[]) {
	long i,j;
	pthread_t *threads;
	pthread_attr_t pthread_custom_attr;
	parm *p;

	if (argc != 3)
	{
		printf ("Usage: %s n_t wss\n",argv[0]);
		exit(1);
	}

	//arguments
	num_threads=atoi(argv[1]);
	SIZE=atof(argv[2])*1024*1024;

	if(num_threads > MAX_THREAD)
	{
		printf ("Too many threads\n");
		exit(1);
	}

	//create threads
	threads=(pthread_t *)malloc(num_threads*sizeof(*threads));
	pthread_attr_init(&pthread_custom_attr);
	p=(parm *)malloc(sizeof(parm)*num_threads);

	/* Start up thread */
	for (i=0; i<num_threads; i++)
	{
		p[i].id=i;
		pthread_create(&threads[i], &pthread_custom_attr, worker, (void *)(p+i));
	}

	/* Synchronize the completion of each thread. */

	for (i=0; i<num_threads; i++)
		pthread_join(threads[i],NULL);

	free(p);

	/*printf("Done\n");*/
	return 0;
}

