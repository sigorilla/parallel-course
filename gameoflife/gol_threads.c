#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

/**
 * ---------------------- Global Defines ----------------------
 */

/**
 * Maximum dimensions of the board
 */
#define MAX_HEIGHT 50
#define MAX_WIDTH 160

/**
 * Default dimensions of the board
 */
#define DHEIGHT 25
#define DWIDTH 80

/**
 * Define renew time in us
 */
#define RTIME 500000

/**
 * ------------------- End of Global Defines ------------------
 */


/**
 * --------------------- Global Variables ---------------------
 */

/**
 * Boards dimension values
 */
unsigned int height;
unsigned int width;

/**
 * Number of threads
 */
unsigned int nthreads;

/**
 * Array for thread ids
 */
int *t_ids;

/**
 * Barrier variable
 */
pthread_barrier_t barr;

/**
 * Program has two arrays in order to save each state
 * The needed memory is allocated in main due to dimension values
 */
int **array1;
int **array2;

/**
 * Pointers to handle arrays 
 * (current and next states, and temp for swap).
 */
int **curptr, **nextptr, **temp;

/**
 * Flag to specify the step mode
 */
unsigned int step_flag;

/**
 * Count of steps
 */
unsigned int max_step;

/**
 * Flag to specify default dimension mode
 */
unsigned int dflag;

/**
 * Input filename
 */
char *filename;

/**
 * ------------------ End of Global Variables -----------------
 */


/**
 * ---------------------- Init Functions ----------------------
 */

int arg_check( int, char ** );
void initialize_board( int ** );
void read_file( int **, char * );
void *entry_function( void * );
void play( int **, int **, int, int );
int adjacent_to( int **, int, int );
void print( int ** );
void print_help( void );
void free_memory( void );

/**
 * ------------------- End of Init Functions ------------------
 */


/**
 * ----------------------- Main Program -----------------------
 */

/**
 * main function
 * @param  argc count of arguments
 * @param  argv array of arguments
 * @return      code of program finish
 */
int main( int argc, char **argv ) {
	int i;

	/**
	 * Init flags and variables
	 */
	dflag = 1;
	step_flag = 0;
	max_step = 100;
	height = 0;
	width = 0;
	filename = NULL;

	/**
	 * Default value for number of threads is 1
	 */
	nthreads = 1;

	/**
	 * Call argument checking
	 */
	if ( arg_check( argc, argv ) != 0 ) {
		return -1;
	}

	/**
	 * Allocate the two arrays due to the given dimensions
	 */

	array1 = ( int ** ) malloc( height * sizeof( int * ) );
	if ( array1 == NULL ) {
		printf( "Error of allocating memory\n" );
		return -1;
	}
	for ( i = 0; i < height; i++ ) {
		array1[ i ] = ( int * ) malloc( width * sizeof( int ) );
		if ( array1[ i ] == NULL ) {
			printf( "Error of allocating memory\n" );
			return -1;
		}
	}

	array2 = ( int ** ) malloc( height * sizeof( int * ) );
	if ( array2 == NULL ) {
		printf( "Error of allocating memory\n" );
		return -1;
	}
	for ( i = 0; i < height; i++ ) {
		array2[ i ] = ( int * ) malloc( width * sizeof( int ) );
		if ( array2[ i ] == NULL ) {
			printf( "Problem in memory allocation\n" );
			return -1;
		}
	}

	/**
	 * Init pointers to show to the relevant arrays
	 */
	curptr = array1;
	nextptr = array2;
	initialize_board( curptr );

	/**
	 * If we have default dimensions read input state from file
	 */
	if ( filename != NULL ) {
		read_file( curptr, filename );
	} else {
		/**
		 * In case of play mode with user input dimensions 
		 * check if they are too big to print in console
		 */
		if ( ( height > MAX_HEIGHT ) || ( width > MAX_WIDTH ) ) {
			printf( "WARNING!!!! Too large board to print in screen.\n" );
			printf( "Maximum allowed dimensions is %d x %d.\n", 
				MAX_HEIGHT, MAX_WIDTH );
			printf( "Bigger values are allowed only in bench mode.\n" );
			return -1;
		}
	}

	/**
	 * Create an array with threads given the input number
	 */
	pthread_t thr[ nthreads ];

	/**
	 * Allocate memory for the thread ids
	 */
	t_ids = malloc( nthreads * sizeof( int ) );
	if ( t_ids == NULL ) {
		printf( "Error of allocating memory\n" );
		return -1;
	}

	/**
	 * Barrier initialization
	 */
	if ( pthread_barrier_init( &barr, NULL, nthreads ) ) {
		printf( "Could not create a barrier\n" );
		return -1;
	}

	/**
	 * Create the threads
	 */
	for ( i = 0; i < nthreads; i++ ) {
		t_ids[ i ] = i;
		if ( pthread_create( &thr[ i ], NULL, &entry_function, ( void * ) &t_ids[ i ] ) ) {
			printf( "Could not create thread %d\n", i );
			return -1;
		}
	}

	/**
	 * Proccess's master thread waits for the execution of all threads
	 */
	for ( i = 0; i < nthreads; i++ ) {
		if ( pthread_join( thr[ i ], NULL ) ) {
			printf( "Could not join thread %d\n", i );
			return -1;
		}
	}

	printf( "-------------------------------" );
	printf( " Board's Last (%d) State ", max_step );
	printf( "--------------------------------\n" );
	print( curptr );

	free_memory();


	return 0;
}

/**
 * -------------------- End of Main Program -------------------
 */


/**
 * ---------------------- Functions Code ----------------------
 */

/**
 * In order to specify the behaviour of the program we check the passed arguments. 
 * We make the required checks and print relevant messages.
 * The basic idea is that we have two modes. Play and step modes. 
 * For more details read the relevant paragraph in the full report file.
 * @param  argc Count of arguments
 * @param  argv Array of arguments
 * @return      Code of function finish
 */
int arg_check( int argc, char **argv ) {
	int opt_char;
	if ( argc == 1 ) {
		print_help();
		return -1;
	}
	while ( ( opt_char = getopt( argc, argv, "n:h:w:f:m:s" ) ) != -1 ) {
		switch ( opt_char ) {
			case 'n':
				nthreads = atoi( optarg );
				break;
			case 'h':
				height = atoi( optarg );
				break;
			case 'w':
				width = atoi( optarg );
				break;
			case 'f':
				filename = optarg;
				break;
			case 'm':
				max_step = atoi( optarg );
				break;
			case 's':
				step_flag = 1;
				break;
			default:
				print_help();
				break;
		}
	}
	/**
	 * In case of user given dimensions, check if both of them have been given
	 */
	if ( height == 0 && width == 0) {
		dflag = 1;
	} else if ( height != 0 && width == 0 ) {
		printf( "Give width dimension too!\n" );
		printf( "Or leave default.\n" );
		return -1;
	} else if ( width != 0 && height == 0 ) {
		printf( "Give length dimension too!\n" );
		printf( "Or leave default.\n" );
		return -1;
	} else if ( width != 0 && height != 0 ) {
		/**
		 * If all correct given disable default flag
		 */
		dflag = 0;
		if ( filename != NULL ) {
			printf( "\nWARNING!\n" );
			printf( "Dimensions arguments have overscale default input file\n" );
			printf( "Hit ENTER to continue.\n" );
			getchar();
		}
	}
	/**
	 * If default flag has not been disabled, assign default dimension to variables
	 */
	if ( dflag ) {
		height = DHEIGHT;
		width = DWIDTH;

		/**
		 * In default mode we need an input file for the initial state. 
		 * So check if it has been given, otherwise exit with message
		 */
		if ( filename == NULL ) {
			printf( "Program create random aria, cause you don't put filename.\n" );
		}
	}
	return 0;
}


/**
 * Init board with zeroes if default mode, or with random 0, 1
 * @param curptr Current state
 */
void initialize_board( int **curptr ) {
	int i, j;

	for ( i = 0; i < height; i++ ) {
		for ( j = 0; j < width; j++ ) {
			curptr[ i ][ j ] = ( dflag && filename == NULL ) ? ( rand() % 2 ) : 0;
		}
	}
}


/**
 * Read the initial state of the board from a file in default mode
 * @param curptr Current state
 * @param name   Filename
 */
void read_file( int **curptr, char *name ) {
	FILE *f;
	int i, j;
	char s[ MAX_WIDTH ];

	f = fopen( name, "r" );
	for ( i = 0; i < height; i++ ) {
		fgets( s, MAX_WIDTH, f );
		for ( j = 0; j < width; j++ ) {
			curptr[ i ][ j ] = ( s[ j ] == 'x' );
		}
	}

	fclose( f );
}


/**
 * Threads' entry function (calculate raw bounds and play game).
 * @param  t_id Thread ID
 * @return      Code of function finish
 */
void *entry_function( void *t_id ) {
	int *thread_id = ( int * ) t_id;

	/**
	 * Calculate the array bounds that each thread will process
	 */
	int bound = height / nthreads;
	int start = *thread_id * bound;
	int finish = start + bound;

	int i, bn;

	/**
	 * Play the game for N rounds
	 */
	for ( i = 0; i < max_step; i++ ) {
		play( curptr, nextptr, start, finish );

		/**
		 * Synchronization point with barries
		 * 
		 * The pthread_barrier_wait subroutine synchronizes participating 
		 * threads at the barrier referenced by barrier. 
		 * 
		 * The calling thread blocks until the required number of threads 
		 * have called pthread_barrier_wait specifying the barrier. 
		 * 
		 * When the required number of threads have called 
		 * pthread_barrier_wait specifying the barrier, 
		 * the constant PTHREAD_BARRIER_SERIAL_THREAD is returned to one 
		 * unspecified thread and 0 is returned to the remaining threads. 
		 * 
		 * At this point, the barrier resets to the state it had as a result of 
		 * the most recent pthread_barrier_init function that referenced it.
		 */
		bn = pthread_barrier_wait(&barr);
		if ( bn != 0 && bn != PTHREAD_BARRIER_SERIAL_THREAD ) {
			printf( "Could not wait on barrier\n" );
			exit( -1 );
		}

		/**
		 * The thread with ID = 0 is responsible to swap the pointers and
		 * print the board's status in each round.
		 */
		if ( bn == PTHREAD_BARRIER_SERIAL_THREAD ) {
			if ( i == 0 ) {
				if ( !step_flag ) {
					system( "clear" );
				}
				printf( "-----------------------------" );
				printf( " Board's Initial State " );
				printf( "--------------------------------\n" );
				print( curptr );
				if ( !step_flag ) {
					printf( "\n\t\tRenew every %d us  ", RTIME );
					printf( "Hit ENTER to start play!\n" );
					getchar();
				}
			} else {
				printf( "----------------------------" );
				printf( " Board's state in round: %*d ", 3, i );
				printf( "---------------------------\n" );

				/**
				 * Swap pointers
				 */
				temp = curptr;
				curptr = nextptr;
				nextptr = temp;
				print( curptr );
			}

			if ( step_flag ) {
				printf( "----------------------------" );
				printf( "----------------------------" );
				printf( "----------------------------\n" );
				printf( "Hit ENTER to next step!\n" );
				getchar();
			} else {
				usleep( RTIME );
				system( "clear" );
			}
		}

		/**
		 * One more barrier is needed in order to ensure 
		 * that the pointers have been swapped before go to next round
		 */
		bn = pthread_barrier_wait( &barr );
		if ( bn != 0 && bn != PTHREAD_BARRIER_SERIAL_THREAD ) {
			printf( "Could not wait on barrier\n" );
			exit( -1 );
		}
	}

	return 0;
}


/**
 * Main play function for the game of life. 
 * Takes as input two pointers for the arrays
 * and the segment in which to apply the game.
 * @param curptr  Current state
 * @param nextptr Next state
 * @param start   Start horizontal coordinate
 * @param finish  End horizontal coordinate
 */
void play( int **curptr, int **nextptr, int start, int finish ) {
	int i, j, alive;

	/**
	 * Apply for each cell the game's rules
	 */
	for ( i = start; i < finish; i++ ) {
		for ( j = 0; j < width; j++ ) {
			alive = adjacent_to( curptr, i, j );
			if ( alive == 2 ) {
				nextptr[ i ][ j ] = curptr[ i ][ j ];
			}
			if ( alive == 3 ) {
				nextptr[ i ][ j ] = 1;
			}
			if ( alive < 2 ) {
				nextptr[ i ][ j ] = 0;
			}
			if ( alive > 3 ) {
				nextptr[ i ][ j ] = 0;
			}
		}
	}
}


/**
 * Function to return the number of alive neighbors of the ( i, j ) cell.
 * @param  curptr Current state
 * @param  i      Horizontal coordinate
 * @param  j      Vertical coordinate
 * @return        Count of alive neighbors
 */
int adjacent_to( int **curptr, int i, int j) {
	int row, col;
	int count = 0;

	for ( row = -1; row <= 1; row++ ) {
		for ( col = -1; col <= 1; col++ ) {
			/**
			 * Exclude current cell from count
			 */
			if ( row || col ) {
				/**
				 * Exclude extreme cells
				 */
				if ( ( i + row ) > 0 && ( i + row ) < height &&
					 ( j + col ) > 0 && ( j + col ) < width ) {
					if ( curptr[ i + row ][ j + col ] ) {
						count++;
					}
				}
			}
			/**
			 * It doesn't need to keep counting 
			 * if the number is over than 3 (no change in behaviour)
			 */
			if ( count > 3 ) {
				return count;
			}
		}
	}

	return count;
}


/**
 * Print the state of the board to standard output
 * @param curptr Current state
 */
void print( int **curptr ) {
	int i, j;

	for ( i = 0; i < height; i++ ) {
		for ( j = 0; j < width; j++ ) {
			printf( "%c", curptr[ i ][ j ] ? 'x' : ' ' );
		}
		printf( "|\n" );
	}
}


/**
 * Print argument usage
 */
void print_help() {
	printf( "\nUsage:\t ./GOL OPTIONS\n" );
	printf( "\tOPTIONS:\n" );
	printf( "\t\t-n  Number of Threads (mandatory option)\n" );
	printf( "\t\t-h  Set Board Height (Raws)\n" );
	printf( "\t\t-w  Set Board Width (Columns)\n" );
	printf( "\t\t-f  Input File\n" );
	printf( "\t\t-s  Step Mode\n\n" );
	printf( "\t\t-m  Count of Steps (default: 100)\n\n" );
}


/**
 * Free memory
 */
void free_memory() {
	int i;

	for ( i = 0; i < height; i++ ) {
		free( array1[ i ] );
	}
	free( array1 );

	for ( i = 0; i < height; i++ ) {
		free( array2[ i ] );
	}
	free( array2 );

	free( t_ids );
}

/**
 * ------------------- End of functions Code ------------------
 */
