#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <errno.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int i, n, last = -1;
int *ids;
int token;

void *Number( void * );

int main( int argc, char **argv ) {

    if ( argc != 2 ) {
        printf( "Wrong numbers of parameters. Add n.\n" );
        return -1;
    }

    n = atoi( argv[ 1 ] );

    int i = 0;

    if ( !( ids = ( int * ) malloc( sizeof( int ) * n ) ) ) {
        printf( "Error of allocating memory\n" );
        return -1;
    }

    pthread_t tids[ n ];

    for ( i = 0; i < n; i++ ) {
        ids[ i ] = i;
        if ( pthread_create( &tids[ i ], NULL, Number, ( void * ) &ids[ i ] ) ) {
            printf( "Error of creating thread: %s\n", strerror( errno ) );
            return -1;
        }
    }

    for ( i = 0; i < n; i++ ) {
        if ( pthread_join( tids[ i ], NULL ) ) {
            printf( "Error of joining thread: %s\n", strerror( errno ) );
            exit( -1 );
        }
    }

    return 0;
}


void *Number( void *arg ) {
    int id = *( int * ) arg;

    if ( id == 0 ) {
        pthread_mutex_lock( &mutex );
        last = id;
        token = 123;
        pthread_mutex_unlock( &mutex );
    } else {
        while (last < id - 1) {
            continue;
        }
        printf("Thread %d received token %d from thread %d\n",
            id, token, id - 1);
        pthread_mutex_lock( &mutex );
        last = id;
        token += 1;
        pthread_mutex_unlock( &mutex );
    }

    if ( id == 0 ) {
        while ( last < n - 1 ) {
            continue;
        }
        printf("Thread %d received token %d from thread %d\n",
            id, token, n - 1 );
    }

    pthread_exit( NULL );
}
