#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <resolv.h>
#include <stdbool.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>

#define TCP_BUF_SIZE		124
#define UDP_MIN_BUF_SIZE	10
#define UDP_MAX_BUF_SIZE	120
#define PREFIX_SIZE		4

#define TCP_RECONNECT_TIMEOUT	5

#define TCP_LOCAL_IP		"192.168.1.70"
#define TCP_LOCAL_PORT		1234

#define TCP_FIN			0b00000001
#define TCP_SYN			0b00000010
#define TCP_RST			0b00000100
#define TCP_ACK			0b00010000

#define FALSE			0
#define TRUE			1

#define TRACE(x) do { log_write x; } while (0)

typedef struct interthread_str 
{
    char 	payload[TCP_BUF_SIZE];
    char 	tcp_prefix[PREFIX_SIZE];
    int		len;
    bool	package_ready_flag;
    bool	tcp_conn_flag;
    bool	debug_stdout;
} interthread;

typedef struct ip_params_str 
{
    char	ip[15];
    char	port[5];
} ip_params;

extern interthread comm_str; 

int udp_server(char*, char*);
int tcp_client(char*, char*);
int tcp_server();
void log_write(const char *fmt, ...);
void print_payload(char* data , int len);
