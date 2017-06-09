#include <global.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

void tcp_connect(int socket, const struct sockaddr *addr, int len)
{
    TRACE(("[TCP] Connecting...\n"));
    while(!comm_str.tcp_conn_flag)
    {
	if(connect(socket, addr, len) == 0)
	    comm_str.tcp_conn_flag = TRUE;
	else
	    sleep(TCP_RECONNECT_TIMEOUT);
    }
}

int parse_packet(unsigned char* buffer, int data_size, char *srv_addr, char *srv_port)
{
    unsigned short 	iphdrlen;
    unsigned long 	srv_addr_l = 0;
    unsigned short 	srv_port_s = 0;

    int			payload_len = 0;
    
    struct sockaddr_in source,dest, local_remote;
    inet_aton(srv_addr, (struct in_addr*)srv_addr_l);
    srv_port_s = htons(atoi(srv_port));

    struct iphdr *iph = (struct iphdr *)buffer;
    iphdrlen =iph->ihl*4;
     
    memset(&source, 0, sizeof(source));
    source.sin_addr.s_addr = iph->saddr;
     
    memset(&dest, 0, sizeof(dest));
    dest.sin_addr.s_addr = iph->daddr;

    memset(&local_remote, 0, sizeof(local_remote));
    inet_aton(srv_addr, &local_remote.sin_addr);


    struct tcphdr *tcph=(struct tcphdr*)(buffer + iphdrlen);
   
    if(iph->protocol == 6 && \
	    source.sin_addr.s_addr == local_remote.sin_addr.s_addr && \
	    tcph->source == srv_port_s)
    {
	if(tcph->rst == TRUE || tcph->fin == TRUE)
	{
	    TRACE(("[TCP] Connection with %s:%d lost... \n", inet_ntoa(source.sin_addr), ntohs(tcph->source)));
	    return -1;
	}
	else
	{
	    payload_len = data_size - tcph->doff*4-iph->ihl*4;
	    TRACE(("[TCP] Msg RX from %s:%d (%d bytes) received: \n", inet_ntoa(source.sin_addr), ntohs(tcph->source), payload_len));

	    if(payload_len == 0)
	    {
	    	TRACE(("   |-Acknowledgement Flag : %d\n",(unsigned int)tcph->ack));
		TRACE(("   |-Reset Flag           : %d\n",(unsigned int)tcph->rst));
		TRACE(("   |-Synchronise Flag     : %d\n",(unsigned int)tcph->syn));
		TRACE(("   |-Finish Flag          : %d\n",(unsigned int)tcph->fin));

	    }
	    else
	    {
		print_payload(buffer + iphdrlen + tcph->doff*4, payload_len );
	    }

	}
    }


    return 0;
}
int tcp_client(char *srv_addr, char *srv_port)
{
    int 	send_socket, raw_socket;
    struct 	sockaddr_in send_addr;
    struct	sockaddr raw_addr;
    int		saddr_size;
    int 	socket_status = 0, data_size = 0;

    unsigned char *buffer = (unsigned char *)malloc(65536); //Its Big!
    // Send socket init
    send_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(send_socket == -1)
	printf("[TCP] Could not create socket");

    bzero(&send_addr, sizeof(send_addr));

    send_addr.sin_family = AF_INET;
    send_addr.sin_port = htons(atoi(srv_port));
    
    if(inet_aton(srv_addr, &send_addr.sin_addr) == 0)
    {
	printf("[TCP} Invalid IP - %s\n", srv_addr);
	exit(0);
    }

    // Rvc raw socket init
    raw_socket = socket(AF_INET , SOCK_RAW , IPPROTO_TCP);
    if(raw_socket < 0)
    {
        printf("Socket Error\n");
        return 1;
    }

    // connecting to remote server
    tcp_connect(send_socket, (struct sockaddr*)&send_addr, sizeof(send_addr));
    TRACE(("[TCP] Connected to %s:%d\n", inet_ntoa(send_addr.sin_addr), atoi(srv_port)));

    while(1)
    {
	if(comm_str.package_ready_flag == TRUE)
	{
	    if(send(send_socket, comm_str.payload, comm_str.len, 0))
	    {
		TRACE(("[TCP] Message sent to %s:%s (%d bytes)\n", inet_ntoa(send_addr.sin_addr), srv_port, comm_str.len));
		comm_str.package_ready_flag = FALSE;
	    }
	}
	data_size = recvfrom(raw_socket , buffer , 65536 , 0 , &raw_addr , &saddr_size);
	if(data_size < 0)
	{
	    TRACE(("[TCP] Recv error , failed to get packets\n"));
	    return 1;
	}
	socket_status = parse_packet(buffer, data_size, srv_addr, srv_port);
	if(socket_status < 0)
	{
	    comm_str.tcp_conn_flag = FALSE;
	    close(send_socket);
	    send_socket = socket(AF_INET, SOCK_STREAM, 0);
	    tcp_connect(send_socket, (struct sockaddr*)&send_addr, sizeof(send_addr));
	}
    }

    close(send_socket);
    close(raw_socket);

 return 0;
}
