int whileTcpListen(int listenPort)
{
	//循环监听tcp，等待客户端请求连接
	
	//socket
	int fd_socket;
    fd_socket= socket(AF_INET, SOCK_STREAM, 0);
    if(fd_socket < 0)
    {
            perror("socket ");
            return -1;
    }

	//setsockopt
    int iOption_value = 1;
    int iLength = sizeof(int);
    if(setsockopt(fd_socket,SOL_SOCKET,SO_REUSEADDR,&iOption_value, iLength)<0)
    {
            perror("setsockopt \n");
            return -1;
    }
	
	//bind
	struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(listenPort);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(fd_socket, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)) < 0)
    {
            perror("bind ");
            return -2;
    }

	//listen
    if(listen(fd_socket, 4) < 0)
    {
        printf("Listen failed!\n");
    }
	
    while(1)
    {
    	//accept
    	printf("start to accept!\n");
    	struct sockaddr_in client_addr;
    	socklen_t cli_len;
    	int fd_connect;
        fd_connect = accept(fd_socket, (struct sockaddr *)&client_addr, &cli_len);        
		if(fd_connect>0)
		{
			printf("the ServerA ip is %s, port is %d\n",inet_ntoa(client_addr.sin_addr),htons(client_addr.sin_port));
			//客户端连接成功，此处可以创建线程处理socketfd_connect
		}
		else
		{
			perror("accetp ");
		}
    }
    printf("over\n");
    return 0;
}

int main(int argc, char **argv)
{
	std::cout<<"SOFTVER=0.0.0.1"<<endl;	
	//whileTcpListen(5000);
	tcpConnectToServer("192.168.1.197",6000,0);
	while(1)
	{
		pause();
		sleep(10);
		printf("pause end\n");		
	}
	printf("main end\n");
	return 0;
}
