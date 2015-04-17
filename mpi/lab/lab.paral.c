/**
 * @author GitHib: kyza1994
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <mpi.h>

/**
 * Variables
 */
int l, p = 1, a = 1;
double h, tau, max_t;


/**
 * Init functions
 */
double f( double, double );
double g( double );
double b( double );


/**
 * Main function
 * @param  argc count of arguments
 * @param  argv array of arguments
 * @return      finish code of program
 */
int main( int argc, char** argv ) {
	FILE *OUTP = fopen( "data.txt", "w+" );
	if ( argc != 5 ) {
		printf( "Wrong number of parameters.\n" );
		return -1;
	}

	h = atof( argv[ 1 ] );
	l = atof( argv[ 3 ] );
	tau = atof( argv[ 2 ] );
	max_t = atof( argv[ 4 ] );

	int k = round( max_t / tau ), i = 0;
	int m = round( l / h ), j = 0;
	int rank, size, l;
	double t[ k + 1 ], x[ m + 1 ], u1[ m + 1 ], u2[ m + 1 ], t1, t2;

	MPI_Init( &argc, &argv );
	MPI_Comm_rank( MPI_COMM_WORLD, &rank );
	MPI_Comm_size( MPI_COMM_WORLD, &size );
	MPI_Status status;
	t1 = MPI_Wtime();

	t[ 0 ] = 0;
	for ( i = 1; i <= k; i++ ) {
		t[ i ] = t[ i - 1 ] + tau;
	}
	x[ 0 ] = 0;
	for ( j = 1; j <= m; j++ ) {
		x[ j ] = x[ j - 1 ] + h;
	}

	i = rank + 1;
	if ( rank == 0 ) {
		fprintf( OUTP, "%d %d\n", k, m );
		printf( "k = %d m = %d\n", k, m );
		for ( j = 0; j <= m; j++ ) {
			u1[ j ] = g( x[ j ] );
			fprintf( OUTP,"%lf ", u1[ j ] );
		}
		fprintf( OUTP, "\n" );
		for ( j = 0; j <= m; j++ ) {
			if ( j == 0 ) {
				u2[ j ] = b( t[ i ] );
			}
			else if ( j < m ) {
				u2[ j ] = f( x[ j ], t[ i - 1 ] ) * tau - 
					( u1[ j + 1 ] - u1[ j - 1 ] ) * tau / ( 2 * h ) + 
					0.5 * ( u1[ j + 1 ] + u1[ j - 1 ] );
				MPI_Send( &u2[ j ], 1, MPI_DOUBLE, ( rank + 1 ) % size, j, MPI_COMM_WORLD);
			} else {
				u2[ j ] = f( x[ j ], t[ i - 1 ] ) * tau - 
					(u1[ j ] - u1[ j - 1 ] ) * tau / h + u1[ j ];
				for ( l = 0; l <= m; l++ ) {
					fprintf( OUTP, "%lf ", u2[ l ] );
				}
				fprintf( OUTP, "\n" );
				MPI_Send( &u2[ j ], 1, MPI_DOUBLE, ( rank + 1 ) % size, j, MPI_COMM_WORLD);
			}
		}
	}

	if ( rank == 0 ) {
		i = size + 1;
	} else {
		i = rank + 1;
	}
	for ( i; i < k + 1; i += size ) {
		for ( j = 0; j <= m; j++ ) {
			if ( j == 0 ) {
				u1[ j ] = b( t[ i ] );
				u2[ j ] = b( t[ i ] );
				MPI_Recv( &u1[ 1 ], 1, MPI_DOUBLE, ( rank - 1 + size ) % size, 
					j + 1, MPI_COMM_WORLD, &status );
			} else if ( j < m ) {
				MPI_Recv( &u1[ j + 1 ], 1, MPI_DOUBLE, ( rank - 1 + size) % size, 
					j + 1, MPI_COMM_WORLD, &status );
				u2[ j ] = f( x[ j ], t[ i - 1 ] ) * tau - 
					( u1[ j + 1 ] - u1[ j - 1 ] ) * tau / ( 2 * h ) + 
					0.5 * ( u1[ j + 1 ] + u1[ j - 1 ] );
				if ( i != k ) {
					MPI_Send( &u2[ j ], 1, MPI_DOUBLE, ( rank + 1 ) % size, j, MPI_COMM_WORLD );
				}
			} else {
				u2[ j ] = f( x[ j ], t[ i - 1 ] ) * tau - 
					( u1[ j ] - u1[ j - 1 ] ) * tau / h + u1[ j ];
				MPI_Send( &u2[ j ], 1, MPI_DOUBLE, ( rank + 1) % size, j, MPI_COMM_WORLD );
				MPI_Send( u2, m + 1, MPI_DOUBLE, 0, k + i, MPI_COMM_WORLD );
			}
		}
	}
	if ( rank == 0 ) {
		for ( j = 2; j <= k; j++ ) {
			MPI_Recv( u2, m + 1, MPI_DOUBLE, MPI_ANY_SOURCE, k + j, MPI_COMM_WORLD, &status );
			for ( l = 0; l <= m; l++ ) {
				fprintf( OUTP, "%lf ", u2[ l ] );
			}
			fprintf( OUTP, "\n" );
		}
	}
	MPI_Barrier( MPI_COMM_WORLD );
	if ( rank == 0 ) {
		fprintf( OUTP, "\n" );
		for ( i = 0; i <= k; i++ ) {
			fprintf( OUTP, "%lf ", t[ i ] );
		}
		fprintf( OUTP, "\n" );
		for ( j = 0; j <= m; j++ ) {
			fprintf( OUTP, "%lf ", x[j] );
		}
		fprintf( OUTP, "\n" );
		t2 = MPI_Wtime();
		printf( "Time: %lf\n", t2 - t1 );
	}

	fclose( OUTP );
	MPI_Finalize();
	return 0;
}


/**
 * Right side of equation
 * @param  x X
 * @param  t T
 * @return   function of x and t
 */
double f( double x, double t ) {
	return x / l * ( 1 - x / l ) * exp( - a * t );
}


/**
 * Border condition for X
 * @param  x X
 * @return   function of x
 */
double g( double x ) {
	return pow( fabs( cos( M_PI * pow( x / l, 4 ) ) ), 3 );
}


/**
 * Border condition for T
 * @param  t T
 * @return   function of t
 */
double b( double t  ) {
	return exp( -p * t );
}
