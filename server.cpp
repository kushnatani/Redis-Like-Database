#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <cstdlib>  // for exit()
#include <cstdio>   // for perror()
#include <unistd.h> // for read() write()
#include <errno.h>
#include <cstring>  // for strlen()



static void die(const char *msg) {
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

void msg(const char *msg){
    fprintf(stderr, "%s\n", msg);
}

// read and write
// static is used so that this function is accessed in this file only
static void do_something(int connfd){
   char rbuf[64] = {};
   ssize_t n = read(connfd , rbuf, sizeof(rbuf)-1);

   if(n<0){
      msg("read() error");
      return ;
   }

   printf("client says: %s\n", rbuf);

   char wbuf[] = "Yes!!!";
   write(connfd, wbuf, strlen(wbuf));
}


int main(){
    int fd = socket(AF_INET,SOCK_STREAM,0);
    int val = 1;
    setsockopt(fd,SOL_SOCKET,SO_REUSEADDR,&val,sizeof(val));
    
    // BIND
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;   // family we are using like here we are using IPv4
    addr.sin_port = htons(1234);  // port number of the server // htons converts to big endian // as networls are BE and CPUs are LE
    addr.sin_addr.s_addr = htonl(0); // which IP's are avalable to connect , here 0.0.0.0 means every interfaace is available and client will decide which interface it wants to connect 
    
    int rv = bind(fd,(const struct sockaddr *)&addr,sizeof(addr));// fd-socket,casting addr to sockaddr as it is generic type ont specific to ipv4 

    if(rv){ die("bind()");} // rv-:0(success) , rv-:-1(fail)

    // Listen
    rv = listen(fd,SOMAXCONN);  // 2nd arg is queue size for linux 4096 for mac 128
    if(rv){ die("listen()"); }

    // Accept
    while(true){
        struct sockaddr_in client_addr = {};
        socklen_t addrlen = sizeof(client_addr);
        
        // here const is not used as the addrress is modified , client address is accepted
        int connfd = accept(fd,(struct sockaddr *)&client_addr ,&addrlen);  // new connection for specific client // &addrlen is written as accept() ,wants to reada as well as write theaddrlen value
        
        if(connfd < 0){
            continue; //error
        }

        do_something(connfd);
        close(connfd);
    }



    
}