#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int i, n, N, n_start, n_master, n_slave;
int *ids;
double sum;

void *Summator( void * );

int main( int argc, char **argv ) {

	if ( argc != 3 ) {
		printf( "Wrong numbers of parameters. Add n and N.\n" );
		return -1;
	}

	n = atoi( argv[ 1 ] );
	N = atoi( argv[ 2 ] );

	n_slave = (int) (N / n);
	n_master = n_slave + N % n;

	int i = 1;

	if ( !( ids = ( int * ) malloc( sizeof( int ) * n ) ) ) {
		printf( "Error of allocating memory\n" );
		exit( -1 );
	}

	pthread_t tids[ n ];

	for ( i = 0; i < n; i++ ) {
		ids[ i ] = i;
		if ( pthread_create( &tids[ i ], NULL, Summator, ( void * ) &ids[ i ] ) ) {
			printf( "Error of creating thread: %s\n", strerror( errno ) );
			return -1;
		}
	}

	for ( i = 0; i < n; i++ ) {
		if ( pthread_join( tids[ i ], NULL ) ) {
			printf( "Error of joining thread: %s\n", strerror( errno ) );
			return -1;
		}
	}

	printf("Sum of harmonical series 1/n = %.4lf for N = %d\n", sum, N);

	return 0;
}


void *Summator( void *arg ) {
	int id = *( int * ) arg, start = 1;

	if ( id != 0 ) {
		start = n_master + 1 + (id - 1) * n_slave;
	}

	int finish = n_master + 1;
	if ( id != 0 ) {
		finish = start + n_slave;
	}

	while ( start < finish ) {
		pthread_mutex_lock( &mutex );

		sum += 1.0 / start;

		pthread_mutex_unlock( &mutex );
		start++;
	}

	pthread_exit( NULL );
}

