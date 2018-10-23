
/*
使用epool实现tcp服务器高并发
*/
class tcpServeEpool
{
private:
    #define MAXBUF 4096*4
    #define MAXEPOLLSIZE 100000

    struct epoll_event events[MAXEPOLLSIZE];
    int listen_socket;
    int listen_port;
    int listen_num;
    map<unsigned int,bool> fdmap;
    struct epoll_event ev;
    struct epoll_event wait_event;
    int kdpfd, curfds;
    socklen_t len;
    struct sockaddr_in my_addr, their_addr;
    
    int setnonblocking(int sockfd)
    {
        if(fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK) == -1)
        {
            return -1;
        }
        return 0;
    }
    int handle_recv(int new_fd)
    {
        char buf[MAXBUF + 1];
        int len;
        /* 开始处理每个新连接上的数据收发 */
        bzero(buf, MAXBUF + 1);
        /* 接收客户端的消息 */
        int count = 0;
        while(1)
        {
        	len = recv(new_fd, buf, MAXBUF, 0);
        	if(len > 0)
            {
            	printf("new_fd=%d buf=%s len=%d\n",new_fd, buf, len);
        		count+=len;
        	}
            else
            {
            	if(len < 0)
                {
                	//process data
                	break;
            	}
                else
                {
                	printf("recv close \n",new_fd);
                	count = -1;
                	break;
            	}
            	
        	}
    	}
        /* 处理每个新连接上的数据收发结束 */
        return count;
    }
    static void * thread_run(void *arg)
    {
        #if 0
    	while(1)
        {
    		usleep(2000000);
    		int i;
    		for(i = 0 ;i < MAXEPOLLSIZE ;i++)
            {
    			if(fds[i].fd>0)
                {
    				const char *str = "hello";
    				int f = send(fds[i].fd,str,5,0);
    				printf("send %d\n",f);
    			}
    		}
    	}
        #endif
    }
    void init_tcp_server()
    {
    	if((listen_socket = socket(PF_INET, SOCK_STREAM, 0)) == -1) 
        {
            perror("socket error \n");
            exit(1);
        }
        else
        {
            printf("socket okn");
        }
        
        /*设置socket属性，端口可以重用*/
        int opt=SO_REUSEADDR;
        setsockopt(listen_socket,SOL_SOCKET,SO_REUSEADDR,&opt,sizeof(opt));
        /*设置socket为非阻塞模式*/
        setnonblocking(listen_socket);
        bzero(&my_addr, sizeof(my_addr));
        my_addr.sin_family = PF_INET;
        my_addr.sin_port = htons(listen_port); 
        my_addr.sin_addr.s_addr = INADDR_ANY;
        if(bind(listen_socket, (struct sockaddr *) &my_addr, sizeof(struct sockaddr))== -1) 
        {
            perror("bind error \n");
            exit(1);
        }
        else
        {
            printf("bind ok \n");
        }
        
        if (listen(listen_socket, 10000) == -1) 
        {
            perror("listen error ");
            exit(1);
        }
        else
        {
        	printf("listen okn");
    	}
        
        /* 创建 epoll 句柄，把监听 socket 加入到 epoll 集合里 */        
        len = sizeof(struct sockaddr_in);
        ev.events = EPOLLIN;
        ev.data.fd = listen_socket;
        if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, listen_socket, &ev) < 0) 
        {
            fprintf(stderr, "epoll set insertion error: fd=%d\n", listen_socket);
           // return -1;
        }
        else
        {
        	printf("epoll_ctl listen_socket ok \n");
        }
    }
public:
    int whileMain(int port,int num)
    {
        int new_fd, nfds, n, ret;
        struct rlimit rt;
        listen_port = port;
        listen_num = num;
        /* 设置每个进程允许打开的最大文件数 */
        rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;
        if(setrlimit(RLIMIT_NOFILE, &rt) == -1) 
        {
            perror("setrlimit error ");
            exit(1);
        }
        else
        {
            printf("setrlimit ok \n");
        }
        
        /*create epoll*/
        kdpfd = epoll_create(MAXEPOLLSIZE);
        /* 开启 socket 监听 */
        init_tcp_server();
        curfds = 1;
        //pthread_t t;
        //pthread_create(&t,NULL,thread_run,NULL);
        while (1) 
        {
            /* 等待有事件发生 */
            nfds = epoll_wait(kdpfd, events, curfds, -1);
            if (nfds == -1)
            {
                perror("epoll_wait\n");
                break;
            }
            //std::cout<<nfds<<endl;
            /* 处理所有事件 */
            for(int i = 0 ; i < nfds ; i++)
            {
            	if(listen_socket == events[i].data.fd)
                {
            		new_fd = accept(listen_socket, (struct sockaddr *) &their_addr,&len);
    	           	if (new_fd < 0) 
                    {
    	                perror("accept\n");
    	                if(epoll_ctl(kdpfd,EPOLL_CTL_DEL,listen_socket,&ev)<0)
                        {
    	                	fprintf(stderr, "epoll_ctl EPOLL_CTL_DEL error\n",
    			                new_fd, strerror(errno));
    	                }
    	                close(listen_socket);
    	                init_tcp_server();
    	                continue;
    	            }
                    else
    	            {
    	              	printf("accept：%s:%d  socke=%d\n",inet_ntoa(their_addr.sin_addr), ntohs(their_addr.sin_port), new_fd);
    	            }
                    
                    std::map<unsigned int,bool>::iterator it;
                    it = fdmap.find(new_fd);
                    if (it == fdmap.end())
                    {
                        fdmap.insert(std::pair<unsigned int,bool>(new_fd,true));
			            setnonblocking(new_fd);
			            ev.events = EPOLLIN;
			            ev.data.fd = new_fd;
			            if (epoll_ctl(kdpfd, EPOLL_CTL_ADD, new_fd, &ev) < 0) 
                        {
			            	fprintf(stderr, "epoll_ctl EPOLL_CTL_ADD %s\n",
			                new_fd, strerror(errno));
			                return -1;
			            }
			            curfds++;
			            break;
	        		}
            	}
                else
            	{
                    unsigned int fd = events[i].data.fd;
            	 	if(EPOLLIN == events[i].events & (EPOLLIN|EPOLLERR))
                    {
            	 		ret = handle_recv(fd);
    	                if (ret < 0) 
                        {
    	                    if(epoll_ctl(kdpfd, EPOLL_CTL_DEL,fd, &ev) < 0)
    	                    {
    	                        fprintf(stderr, "epoll_ctl EPOLL_CTL_DEL %s\n",
    	                        fd, strerror(errno));
    	                    }
    	                    curfds--;
    	                    close(fd);
                            fdmap.erase (fd);
    	                }
            	 	}
            	}
            }

        }
        printf("close\n");
        close(listen_socket);
        return 0;
    }    
};
