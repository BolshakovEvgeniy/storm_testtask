#include <global.h>

int tcp_server()
{
    int 		rcv_socket, sock_d;
    int			socket_status, c;
    struct 		sockaddr_in server, client;
    char		reply_from_srv;	

    server.sin_family = AF_INET;
    server.sin_port = htons(1234);
    inet_aton("192.168.1.70", &server.sin_addr); 

    sock_d = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_d == -1)
    {
	TRACE(("[TCP_BIND] Couldn't create socket\n"));
	exit(0);
    }

    if(bind(sock_d, (struct sockaddr *)&server, sizeof(struct sockaddr)) < 0)
    {
	TRACE(("[TCP_BIND] Bind failed\n"));
    }

    listen(sock_d, 5);

    c = sizeof(struct sockaddr_in);
    rcv_socket = accept(sock_d, (struct sockaddr *)&client, (socklen_t*)&c);
    if(rcv_socket < 0)
    {
	TRACE(("[TCP_BIND] Accept() failed\n"));
	exit(0);
    }

    while(1)
    {
	while((socket_status = recv(rcv_socket, &reply_from_srv, 1, 0)) != -1)
	{
	    TRACE(("[TCP_BIND] Connection attempt to local TCP port\n"));
	}
    }
    close(rcv_socket);

 return 0;
}
