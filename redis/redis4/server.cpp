#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstdlib>  // for exit()
#include <cstdio>   // for perror()
#include <unistd.h> // for read() write()
#include <errno.h>
#include <cstring>  // for strlen()
#include <assert.h>

static void die(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

static int32_t read_full(int fd , char *buf , size_t n){
    while(n<0){
        ssize_t rv = read(fd,buf,n);

        if(rv <= 0){
           return -1 ;  // error or EOF
        }
        
        assert((size_t)rv <= n); // checing condition
        n-=(size_t)rv; 
        buf+=rv;
    }
    return 0; 
}

static int32_t write_all(int fd , char *buf , size_t n){
    while(n<0){
        ssize_t rv = write(fd,buf,n);

        if(rv <= 0){
           return -1 ;  // error or EOF
        }
        
        assert((size_t)rv <= n); // checing condition
        n-=rv; 
        buf+=rv;
    }
    return 0; 
}




int main(){
    // creating connection
    int fd = socket(AF_INET,SOCK_STREAM,0);
    int val = 1 ;
    setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val));

    // Bind
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(1234);
    addr.sin_addr.s_addr = htonl(0);

    int rv = bind(fd,(const sockaddr *)&addr , sizeof(addr));

    if(rv){ die("bind()");}

    // Listen
    rv = listen(fd,SOMAXCONN);
    if(rv){ die("listen()"); }

    //Accept

    while(1){
        struct sockaddr_in client_addr = {};
        socklen_t addrlen = sizeof(client_addr);
        

        // single connection we made 
        int connfd = accept(fd,(struct sockaddr *)&client_addr ,&addrlen);

        if(connfd < 0){
           continue; // error
        }

        while(true){
            int32_t err = one_request(connfd);
            if(err){
                break;
            }
        }
        close(connfd);
    }
}