CC=gcc
CFLAGS=-Wall -ggdb

all: HarmSeries SendNum

HarmSeries: harm_series.c 
	$(CC) $(CFLAGS) -o sum harm_series.c -lpthread

SendNum: send_num.c 
	$(CC) $(CFLAGS) -o num send_num.c -lpthread

