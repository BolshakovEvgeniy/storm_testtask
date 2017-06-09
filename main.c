#include <pthread.h>
#include <global.h>

interthread 	comm_str;
char		*filename;

void init(void)
{
    memset(comm_str.payload, 0, sizeof(comm_str.payload));
    comm_str.len = 0;
    comm_str.package_ready_flag = FALSE;
    comm_str.tcp_conn_flag = FALSE;
    comm_str.debug_stdout = FALSE;
}

void *udp_thread(void *arg)
{
    ip_params	*param = (ip_params*)arg;

    TRACE(("[UDP] Starting thread\n"));
    udp_server(param->ip, param->port);
    return 0; 
}

void *tcp_thread(void *arg)
{
    ip_params	*param = (ip_params*)arg;

    TRACE(("[TCP] Starting thread\n"));
    tcp_client(param->ip, param->port);
    return 0; 
}

void *tcp_bind_thread(void *arg)
{
    TRACE(("[TCP_BIND] Binding local TCP port to prevent receiving RST\n"));
    tcp_server();
    return 0;
}

void open_log(char *fname, int fsize)
{
	FILE *fp;
	if((fp = fopen(fname, "a+")) == NULL)
	{
	    printf("[MAIN] Failed to create log file\n");
	    exit(0);
	}
	filename = malloc(sizeof(char)*sizeof(fname));
	memcpy(filename, fname, fsize);
	fclose(fp);
}

int main(int argc, char **argv)
{
    pthread_t 		udp_thread_id, tcp_thread_id, tcp_bind_thread_id;
    pthread_attr_t 	common_attr;
    static ip_params	serv_udp, clnt_tcp;
    char		*ptr;


    if(argc != 5 && argc != 6)
    {
	printf("Usage: %s <UDP_CLIENT_IP:UDP_CLIENT_PORT> <TCP_SERV_IP:TCP_SERV_PORT> [d] <log name>  <TCP_PAYLOAD_PREFIX>\n", argv[0]);
	exit(0);
    }
    init();


    ptr = strtok(argv[1],":");
    memcpy(&serv_udp.ip, ptr, strlen(ptr));
    ptr = strtok(NULL,":");
    memcpy(&serv_udp.port, ptr, strlen(ptr));

    ptr = strtok(argv[2],":");
    memcpy(&clnt_tcp.ip, ptr, strlen(ptr));
    ptr = strtok(NULL,":");
    memcpy(&clnt_tcp.port, ptr, strlen(ptr));

    if(argc == 6)
    {
	if(argv[3][0] != 'd' && strlen(argv[3]) > 1)
	{
	    printf("[MAIN] Wrong argument %s\n", argv[4]);
	    exit(0);
	}
	else
	{
	    comm_str.debug_stdout = TRUE;
	}


	open_log(argv[4], sizeof(argv[4]));
    }
    else
	open_log(argv[3], sizeof(argv[3]));


    if(strtol(argv[argc-1], NULL, 16) != 0 || memcmp(argv[argc-1], "0000", 4) == 0)
	memcpy(comm_str.payload, argv[argc-1], 4);
    else
    {
	printf("[MAIN] Last argument isn't HEX value");
	exit(0);
    }

    pthread_attr_init(&common_attr);
    pthread_attr_setdetachstate(&common_attr, PTHREAD_CREATE_JOINABLE);

    //pthread_create(&tcp_bind_thread_id, &common_attr, tcp_bind_thread, NULL);
    pthread_create(&udp_thread_id, &common_attr, udp_thread, (void*)&serv_udp);
    pthread_create(&tcp_thread_id, &common_attr, tcp_thread, (void*)&clnt_tcp);

    //pthread_join(tcp_bind_thread_id, NULL);
    pthread_join(udp_thread_id, NULL);
    pthread_join(tcp_thread_id, NULL);

    pthread_exit(NULL);

    free(filename);
 return 0;
}
