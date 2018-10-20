class cThread
{
    typedef void *(*pthreadFun)(void *);
public:
    static void createThread(pthreadFun fun)
    {
        pthread_t thr;
        if(pthread_create(&thr,NULL,fun,NULL)!=0)
        {
            perror("create createThread failed!\n");
            return;
        }
    }
};
template <typename T>
class oneFifo
{
public:
    static inline unsigned int mymin(unsigned int x, unsigned int y){return x<y?x:y;}
    T *mybuffer;                          /* the buffer holding the data */
    volatile unsigned int mysize;         /* the size of the allocated buffer */
    volatile unsigned int myin;           /* data is added at offset (in % size) */
    volatile unsigned int myout;          /* data is extracted from off. (out % size) */
    static inline int fls(int x)
    {
    	int position;
    	int i;
    	if(0 != x)
    	{
    		for (i = (x >> 1), position = 0; i != 0; ++position)
    			i >>= 1;
    	}
    	else
    	{
    		position = -1;
    	} 
    	return position+1;
    }

    static inline unsigned int roundup_pow_of_two(unsigned int x)
    {
        return 1UL << fls(x - 1);
    }
    
    bool kfifo_alloc(unsigned int size)
    {
        T *buffer;  
        T * ret;
        if (size & (size - 1)) {
            size = roundup_pow_of_two(size);
        }
    
        buffer = new T[size];;
        if (!buffer)
        {
            perror("kfifo_alloc ");
            return false;
        }
        mybuffer = buffer;
        mysize = size;
        myin = myout = 0;
    
        return true;
    }
    void kfifo_free()
    {
        delete(mybuffer);
    }
    void copy(T *dst,T *src,const unsigned int len)
    {
        for(unsigned int i=0;i<len;i++)
        {
            dst[i] = src[i];
        }
    }
    unsigned int __kfifo_put(T *buffer, unsigned int len)
    {
        unsigned int l;
        len = mymin(len, mysize - myin + myout);     
        l = mymin(len, mysize - (myin & (mysize - 1)));
        copy(mybuffer + (myin & (mysize - 1)),buffer,l);
        copy(mybuffer,buffer + l,len - l);
        myin += len;        
        return len;
    }
    unsigned int __kfifo_get(T *buffer, unsigned int len)
    {
        unsigned int l;
        len = mymin(len, myin - myout);
        l = mymin(len, mysize - (myout & (mysize - 1)));
        copy(buffer, mybuffer + (myout & (mysize - 1)), l);
        copy(buffer + l, mybuffer, len - l);
        myout += len;
        return len;
    }
};
oneFifo<int *> *gqueue;
// 测试无锁队列
void *testFreeLockQueuePut(void *arg)
{
    int buf[10240];
    int *p[10240];
    for(int j=0;j<10240;j++)
    {
        p[j] = &buf[j];
    }
    int **pp;
    unsigned int i=1;
    pp = &p[0];
    while(1)
    {
        if(gqueue->__kfifo_put(pp,1)>0)
        {
            i++;
            buf[i%10240] = i;
            pp = &p[i%10240]; 
            //if((**pp)%10000==0)
              //std::cout<<**pp<<endl;
        }
    }
    return NULL;
}
void *testFreeLockQueueGet(void *arg)
{
    int *p;
    int **pp;
    pp = &p;
    sleep(2);
    while(1)
    {
        if(gqueue->__kfifo_get(pp,1)>0)
        {
            if(((**pp)%(9999999+1))==9999999)
              std::cout<<**pp<<endl;
        }
    }
    return NULL;
}

int main(int argc, char **argv)
{
    gqueue = new oneFifo<int *>;
    gqueue->kfifo_alloc(256);
    cThread::createThread(testFreeLockQueuePut);
    cThread::createThread(testFreeLockQueueGet);
    while(1)
    {
        pause();
    }
    return 0;
}
