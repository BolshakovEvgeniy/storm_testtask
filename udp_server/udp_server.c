#include <global.h>

int udp_server(char *srv_addr, char *srv_port)
{
    int 	rcv_socket;
    struct 	sockaddr_in addr;
    int bytes, addr_len = sizeof(addr);
    char 	rcv_buf[UDP_MAX_BUF_SIZE+1];

    memset(rcv_buf, 0, sizeof(rcv_buf));

    rcv_socket = socket(PF_INET, SOCK_DGRAM, 0);
    bzero(&addr, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(srv_port));
    if(inet_aton(srv_addr, &addr.sin_addr) == 0)
    {
	TRACE(("[UDP] Invalid IP - %s\n", srv_addr));
	exit(0);
    }

    if(bind(rcv_socket, (struct sockaddr*)&addr, sizeof(addr)) != 0)
    {
	TRACE(("[UDP] Error binding socket\n"));
	exit(0);
    }

    TRACE(("[UDP] Socket opened at %s:%d\n", inet_ntoa(addr.sin_addr), atoi(srv_port)));

    while(1)
    {
	bytes = recvfrom(rcv_socket, rcv_buf, sizeof(rcv_buf), 0, (struct sockaddr*)&addr, &addr_len);
	TRACE(("[UDP] Msg RX from %s:%d (%d bytes) received\n", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port), bytes, rcv_buf));
	print_payload(rcv_buf, bytes);
	
	if(comm_str.tcp_conn_flag)
	{
	    if(bytes >= UDP_MIN_BUF_SIZE && bytes <= UDP_MAX_BUF_SIZE)
	    {
		comm_str.package_ready_flag = TRUE;
		comm_str.len = bytes+PREFIX_SIZE;
		bzero(comm_str.payload+PREFIX_SIZE, UDP_MAX_BUF_SIZE); 
		memcpy(comm_str.payload+PREFIX_SIZE, rcv_buf, bytes);
	    }
	    else 
		TRACE(("[UDP] Payload is not in specified range\n"));
	}
	else
	{
	    TRACE(("[UDP] Package received, but TCP connection isn't established\n"));
	}
    }

    close(rcv_socket);


 return 0;
   
}
