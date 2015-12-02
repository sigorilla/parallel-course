/**
 * @author GitHub: sigorilla
 */
#include <stdio.h>
#include <stdlib.h>

int RESULT = 1;

#define ERROR { printf( "Error on line %d\n", __LINE__ ); RESULT = 0; goto out; }

/**
 * Variables
 */
int i;
int T, X, K, M;
double dx, dt, del;
double **u;

/**
 * Init functions
 */
double f( int, int );
double phi( int );
double psi( int );
void save_data();
void free_memory();

/**
 * Main function
 * @param  argc count of argumnets
 * @param  argv array of arguments
 * @return      finish code of program
 */
int main( int argc, char **argv ) {

    T = ( argc >= 2 ) ? atoi( argv[ 1 ] ) : 1;
    X = ( argc >= 3 ) ? atoi( argv[ 2 ] ) : 2;
    K = ( argc >= 4 ) ? atoi( argv[ 3 ] ) : 80;
    M = ( argc >= 5 ) ? atoi( argv[ 4 ] ) : 20;

    if ( K == 0 ) ERROR;
    if ( M == 0 ) ERROR;

    dx = ( double ) X / M;
    dt = ( double ) T / K;
    del = dt / dx;

    if ( del > 1.0 ) ERROR;

    u = ( double ** ) malloc( sizeof( double * ) * K );
    if ( u == NULL ) ERROR;

    for ( i = 0; i < K; i++ ) {
        u[ i ] = ( double * ) malloc( sizeof( double ) * M );
        if ( u[ i ] == NULL ) ERROR;
    }

    int k, m;
    for ( m = 0; m < M; m++ ) {
        u[ 0 ][ m ] = phi( m );
    }
    for ( k = 0; k < (K - 1); k++ ) {
        u[ k + 1 ][ 0 ] = psi( k );
        for ( m = 1; m < M; m++ ) {
            u[ k + 1 ][ m ] = u[ k ][ m ] -
                del * ( u[ k ][ m ] - u[ k ][ m - 1 ] ) + dt * f( k, m );
        }
    }

    out:
        if ( RESULT != 0 ) {
            save_data();
        }
        free_memory();
        return 0;
}


/**
 * Right side of equation
 * @param  x X
 * @param  t T
 * @return   function of x and t
 */
double f( int k, int m ) {
    double t = dt * k;
    double x = dx * m;
    return x * t;
}


/**
 * Border condition for X
 * @param  x X
 * @return   function of x
 */
double phi( int m ) {
    double x = dx * m;
    return x * x;
}


/**
 * Border condition for T
 * @param  t T
 * @return   function of t
 */
double psi( int k ) {
    double t = dt * k;
    return t + 30;
}


/**
 * Saving data to file
 */
void save_data() {
    FILE *data = fopen( "data.dat", "wb" );

    fprintf( data, "K = %d\t M = %d\t dt = %.3lf\t dx = %.3lf\n", K, M, dt, dx );
    int k, m;
    for ( k = 0; k < K; k++ ) {
        fprintf( data, "%.3lf\t\t", k * dt );
        for ( m = 0; m < M; m++ ) {
            fprintf( data, "%.4lf\t\t", u[ k ][ m ] );
        }
        fprintf( data, "\n" );
    }

    fclose( data );
}


/**
 * Clean memory
 */
void free_memory() {
    for ( i = 0; i < K; i++ ) {
        free( u[ i ] );
    }

    free( u );
}
