/**********************************************************\
*                                                         *\
*  program name:tcpscan.c                                 *\
*  Author:lightninig-zgc(http://lightning-zgc.com)        *\
*  Date: 09-21-2016                                       *\
*                                                         *\
\**********************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <time.h>
#include <math.h>
#include <fcntl.h>
#include <errno.h>
#define READ_TERMINAL 0
#define WRITE_TERMINAL 1
// #define CLOCKS_PER_SEC 1000

struct servenet
{
        char * s_name;
        char** s_aliases;
        int s_port;
        char*  s_proto;
};

void print_usage(char * cmd)
{
        fprintf(stderr," %s usage:\n",cmd);
        fprintf(stderr,"%s IP_Addr [port_start] [port_end]\n",cmd);

}


int main(int argc,char** argv)
{
        struct timeval tv;
        struct timezone tz;
        gettimeofday(&tv, &tz);
        int start = tv.tv_sec * 1000000 + tv.tv_usec, finish;
        double duration = 0.0;
        struct sockaddr_in server;
        int file_descriptors[2];
        pid_t pid;
        char PipeBuf[64]={'\0'};
        int read_ret=0;

        int ret;
        int len;
        int ports,porte;
        int sockfd;
        int sendnum;
        int recvnum;
        int status;
        int temp_time;
        char temp[500] = {'\0'};
        char send_buf[2048];
        char recv_buf[2048];
        char print_buf[20480] = {'\0'};



        pipe(file_descriptors);

        if (argc == 2)
        {
                ports = 0;
                porte = 65535;
        }else if (argc == 3) {
                ports = atoi(argv[2]);
                porte = 65535;
        }else if (argc == 4) {
                ports = atoi(argv[2]);
                porte = atoi(argv[3]);
        }else {
                print_usage(argv[0]);
                exit(1);
        }
        int port=ports;
        int pidcount = 0;
        for (; port < porte; port++)
        {
                pid = fork();
                if(0>pid)
                {
                        perror("fork error\n");
                        exit(1);

                }
                else if(0==pid)
                {
                        // if(0>fcntl(sockfd,F_SETFL,fcntl(sockfd,F_GETFL,0)|/*O_NDELAY*/O_NONBLOCK))
                        // {
                        //         printf("fcntl failed/n");
                        //         _exit(0);
                        // }
                        if (-1==(sockfd=socket(AF_INET,SOCK_STREAM,0)))
                        {
                                perror("can not create socket\n");
                                _exit(0);
                        }

                        memset(&server,0,sizeof(struct sockaddr_in));
                        server.sin_family = AF_INET;
                        server.sin_addr.s_addr = inet_addr(argv[1]);
                        server.sin_port = htons(port);
                        temp_time = connect(sockfd,(struct sockaddr*)&server,sizeof(struct sockaddr));
                        // printf("%d\n",temp_time);
                        if (temp_time)
                        {
                                perror("connect error");
                                //
                                // sprintf(temp,"port: %d\n",port);
                                // close(file_descriptors[READ_TERMINAL]);
                                // write(file_descriptors[WRITE_TERMINAL],temp,sizeof(temp));
                                close(sockfd);
                                _exit(0);
                        }
                        sprintf(temp,"open port: %d | %s\n",port,getservbyport(htons(port),"tcp")->s_name);
                        close(file_descriptors[READ_TERMINAL]);
                        write(file_descriptors[WRITE_TERMINAL],temp,sizeof(temp));
                        printf("%s", temp);

                        close(sockfd);
                        _exit(0);
                }
                else
                {
                        close(sockfd);
                        int flag=fcntl(file_descriptors[READ_TERMINAL],F_GETFL,0);
                        flag |= O_NONBLOCK;
                        if(fcntl(file_descriptors[READ_TERMINAL],F_SETFL,flag) < 0)
                        {
                                perror("fcntl");
                                exit(1);
                        }
                        // close(file_descriptors[WRITE_TERMINAL]);
                        while(read_ret=read(file_descriptors[READ_TERMINAL],PipeBuf,sizeof(PipeBuf)) > 0) //读pipe
                                strcat(print_buf,PipeBuf);

                        if(pidcount++ <= 500) {
                                waitpid(pid, &status, 0 );
                                pidcount = 0;
                        }
                        system("clear");
                        // printf("%c[2J", 0x1b);
                        printf("扫描进度:%f%%(port_now:%d)\n",((float)(port - ports) / (porte - ports)) * 100,port);
                        printf("%s",print_buf);
                }
        }
        waitpid(pid, &status, 0 );
        system("clear");
        // printf("%c[2J", 0x1b);
        printf("扫描进度:%f%%(port_now:%d)\n",((float)(port - ports) / (porte - ports)) * 100,port);
        printf("%s",print_buf);
        printf("scan %d ports\n",porte - ports);
        gettimeofday(&tv, &tz);
        finish = tv.tv_sec * 1000000 + tv.tv_usec;
        duration = (double)(finish - start);


        printf("%f seconds\n", duration / 1000000);
}
