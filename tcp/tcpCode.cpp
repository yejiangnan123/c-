//循环监听tcp，等待客户端请求连接
bool whileTcpListen(int listenPort)
{
    //socket
    int fd_socket;
    fd_socket= socket(AF_INET, SOCK_STREAM, 0);
    if(fd_socket < 0)
    {
            perror("socket ");
            return false;
    }

    //setsockopt
    int iOption_value = 1;
    int iLength = sizeof(int);
    if(setsockopt(fd_socket,SOL_SOCKET,SO_REUSEADDR,&iOption_value, iLength)<0)
    {
            perror("setsockopt \n");
            return false;
    }
    
    //bind
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(listenPort);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if(bind(fd_socket, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in)) < 0)
    {
            perror("bind ");
            return false;
    }

    //listen
    if(listen(fd_socket, 4) < 0)
    {
        printf("Listen failed!\n");
        return false
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
            return false;
        }
    }
    printf("over\n");
    return true;
}
//tcp连接服务器
bool tcpConnectToServer(string ip,int port,int mePort)
{
    //socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    //setsockopt
    int iOption_value = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR|SO_REUSEPORT, (const void *)&iOption_value, sizeof(iOption_value));

    //bind
    if(mePort>0)
    {
        struct sockaddr_in mine;
        bzero(&mine,sizeof(mine));
        mine.sin_family = AF_INET;
        mine.sin_port = htons(mePort);
        mine.sin_addr.s_addr = INADDR_ANY;
        int b = bind(sockfd,(struct sockaddr*)&mine,sizeof(mine));
        if(b<0)perror("bind error");
        printf("the mine ip is %s, port is %d\n",inet_ntoa(mine.sin_addr),htons(mine.sin_port));
    }
    
    //connect
    struct sockaddr_in serveraddr;
    memset(&serveraddr,0,sizeof(serveraddr));
    serveraddr.sin_family=AF_INET;
    serveraddr.sin_port=htons(port);
    string serverIp = ip;
    inet_pton(AF_INET,serverIp.c_str(),&serveraddr.sin_addr.s_addr);
    if(connect(sockfd,(struct sockaddr*)&serveraddr,sizeof(serveraddr))<0)
    {
        perror("connect error");
        return false;
    }
    else
    {
        //此处可以创建线程处理socket
        printf("the serveraddr ip is %s, port is %d\n",inet_ntoa(serveraddr.sin_addr),htons(serveraddr.sin_port));
        return true;
    }
}
