#include <global.h>
#include <netinet/tcp.h>
#include <netinet/ip.h>

void tcp_connect(int socket, const struct sockaddr *addr, int len)
{
    while(!comm_str.tcp_conn_flag)
    {
	TRACE(("[TCP] Connecting...\n"));
	if(connect(socket, addr, len) == 0)
	    comm_str.tcp_conn_flag = TRUE;
	else
	    sleep(TCP_RECONNECT_TIMEOUT);
    }
}

int tcp_client(char *srv_addr, char *srv_port)
{
    int 	send_socket;
    struct 	sockaddr_in addr;
    int 	socket_status, socket_status_read;
    char 	reply_from_srv[1024];

    send_socket = socket(AF_INET, SOCK_STREAM, 0);

    if(send_socket == -1)
	printf("[TCP] Could not create socket");

    bzero(&addr, sizeof(addr));

    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(srv_port));
    
    if(inet_aton(srv_addr, &addr.sin_addr) == 0)
    {
	printf("[TCP} Invalid IP - %s\n", srv_addr);
	exit(0);
    }

    tcp_connect(send_socket, (struct sockaddr*)&addr, sizeof(addr));
    TRACE(("[TCP] Connected to %s:%d\n", inet_ntoa(addr.sin_addr), atoi(srv_port)));

    while(1)
    {
//	TRACE(("[TCP] read() status - %d\n", socket_status_read));
//	sleep(1);

	switch(socket_status_read)
	{
	    case 0:
		comm_str.tcp_conn_flag = FALSE;
		TRACE(("[TCP] Connection lost...\n"));
		close(send_socket);
		send_socket = socket(AF_INET, SOCK_STREAM, 0);
		tcp_connect(send_socket, (struct sockaddr*)&addr, sizeof(addr));
		TRACE(("[TCP] Connected to %s:%d\n", inet_ntoa(addr.sin_addr), atoi(srv_port)));
		break;
	    case -1: break;
	    default: TRACE(("[TCP] RX from %s:%d\n", inet_ntoa(addr.sin_addr), atoi(srv_port)));
		     print_payload(reply_from_srv, socket_status_read);
		     bzero(reply_from_srv, 1024);
		     socket_status_read = recv(send_socket, reply_from_srv, 1024, MSG_DONTWAIT);
		     break;

	};

	if(comm_str.package_ready_flag == TRUE)
	{
	    socket_status = send(send_socket, comm_str.payload, comm_str.len, 0);
	    TRACE(("[TCP] send() status - %d\n", socket_status));
	    TRACE(("[TCP] Message sent to %s:%s (%d bytes)\n", inet_ntoa(addr.sin_addr), srv_port, comm_str.len));
	    comm_str.package_ready_flag = FALSE;
	}
    }

    close(send_socket);

 return 0;
}
