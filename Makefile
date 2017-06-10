CC=gcc
CFLAGS=-c -g3 -Wall
INC_DIR=./include

LOG_DIR=./log
UDP_DIR=./udp_server
TCP_DIR=./tcp_client

all: storm_testtask

storm_testtask: main.o log.o udp_server.o tcp_client.o
	$(CC) -pthread -o storm_testtask main.o log.o udp_server.o tcp_client.o

main.o: main.c
	$(CC) $(CFLAGS) -I$(INC_DIR) main.c

log.o: $(LOG_DIR)/log.c
	$(CC) $(CFLAGS) -I$(INC_DIR) $(LOG_DIR)/log.c

udp_server.o: $(UDP_DIR)/udp_server.c
	$(CC) $(CFLAGS) -I$(INC_DIR) $(UDP_DIR)/udp_server.c

tcp_client.o: $(TCP_DIR)/tcp_client.c
	$(CC) $(CFLAGS) -I$(INC_DIR) $(TCP_DIR)/tcp_client.c

clean:
	rm *.o storm_testtask


