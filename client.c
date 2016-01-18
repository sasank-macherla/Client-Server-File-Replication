#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include<dirent.h>
#include<sys/stat.h>
#include <arpa/inet.h>

#define PORT "3490" 
#define MAXDATASIZE 100 
#define LENGTH 65535

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int main(int argc, char *argv[])
{
    int sockfd, numbytes;
    char buf[MAXDATASIZE];
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char s[INET6_ADDRSTRLEN];
    printf("%d\n", argc);
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    
    if ((rv = getaddrinfo(argv[1], PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }
    
    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                             p->ai_protocol)) == -1) {
            perror("client: socket");
            continue;
        }
        if (connect(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("client: connect");
            continue;
        }
        
        break;
    }
    
    if (p == NULL) {
        fprintf(stderr, "client: failed to connect\n");
        return 2;
    }
    
    inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
              s, sizeof s);
    printf("client: connecting to %s\n", s);
    
    freeaddrinfo(servinfo); // all done with this structure
    
    int i, s1, s2,nofiles=0;
    printf("dirname planning to send to server:%s\n", argv[2]);
    char *srcdirname = argv[2];
    struct    dirent *ep1;    /* struct dirent to store all files*/
    struct stat filestat;
    DIR *dp1;
    
    if((dp1 = opendir(srcdirname))==NULL)
    {
        printf("unable to open srcdirname\n");
        exit(0);
    }
    while ((ep1 = readdir(dp1)) != NULL)
    {
        nofiles++;
    }
    rewinddir(dp1);
    printf("numberoffiles=%d\n", nofiles);
    
    
    while ((ep1 = readdir(dp1)) != NULL)
    {
        chdir(srcdirname);
        printf("\n reading files from %s directory\n",srcdirname);
        
        if(stat(ep1->d_name, &filestat) < 0)
        {
            printf("stat error\n");
            exit(0);
        }
        FILE *fpr, *fpw, *fpq;
        char sdbuf[LENGTH], buf[LENGTH], path[LENGTH];
        if((fpr = fopen(ep1->d_name,"r"))!=NULL)
        {
            printf("Now opened '%s' source file for sending\n", ep1->d_name);
            int lenoffilename=strlen(ep1->d_name);
            /*lenoffilename.ToString("000");*/
            /*string.Format("{0:00}", lenoffilename);*/
            printf("sizeoffilename=%d bytes\nfile size:%d bytes\n", lenoffilename, filestat.st_size);
            if(filestat.st_size==0)
            {
                printf("oops!file is empty\n");
            }
            bzero(sdbuf, LENGTH);
            bzero(buf, LENGTH);
            int fs_block_sz;
            
            while((fs_block_sz = fread(sdbuf, sizeof(char), LENGTH, fpr)) > 0)
            {
                sprintf(buf, "%03d%02d%s%04d%s", nofiles, lenoffilename, ep1->d_name, fs_block_sz, sdbuf);
                
                printf("file size Sending to server = %d bytes\nbuf=%s",fs_block_sz, buf);
                
                if((s2=send(sockfd, buf, lenoffilename+fs_block_sz+8, 0)) < 0)
                {
                    fprintf(stderr, "ERROR: Failed to send file %s. (errno = %d)\n", ep1->d_name, errno);
                    exit(1);
                }
                bzero(buf, LENGTH);
                printf("s2=%d bytes\n", s2);
            }
            
            }
    }
    if ((numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1)
    {
        perror("recv");
        exit(1);
    }
    buf[numbytes] = '\0';
    printf("client: received '%s'\n",buf);   
    close(sockfd);
    return 0;
}
