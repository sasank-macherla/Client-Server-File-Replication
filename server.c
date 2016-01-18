#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include<dirent.h>

#define PORT "3490"
#define MAXCLIENTS 10
#define MAXDATASIZE 65535

void sigchld_handler(int s)
{
    while(waitpid(-1, NULL, WNOHANG)>0);
}
void *get_in_addr(struct sockaddr *sa)
{
    if(sa->sa_family == AF_INET)
    {
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}


int main(int argc, char* argv[ ])
{
FILE *fp= NULL;
pid_t process_id = 0;
pid_t sid = 0;
// Create child process
process_id = fork( );
// Indication of fork( ) failure
if (process_id < 0)
{
printf("fork failed!\n");
// Return failure in exit status
exit(1);
}
// PARENT PROCESS. Need to kill it.
if (process_id > 0)
{
printf("process_id of child process %d \n", process_id);
// return success in exit status
exit(0);
}
//unmask the file mode
umask(0);
//set new session
sid = setsid( );
    printf("sid=%d\n",sid);
if(sid < 0)
{
// Return failure
exit(1);
}
// Change the current working directory to root.
chdir("/");
    
// Close stdin. stdout and stderr
close(STDIN_FILENO);
close(STDOUT_FILENO);
close(STDERR_FILENO);
// Open a log file in write mode.
fp = fopen ("Log1.txt", "w+");
  
    int sockfd, newfd, status, yes=1, numbytes;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage clients_addr;
    socklen_t sin_size;
    struct sigaction sa;
    char s[INET6_ADDRSTRLEN];
    char buf[MAXDATASIZE];
    
    memset(&hints,0, sizeof hints);
    hints.ai_family=AF_UNSPEC;
    hints.ai_socktype=SOCK_STREAM;
    hints.ai_flags=AI_PASSIVE;
    fprintf(fp, "what the hell\n");
    fflush(fp);
    if((status=getaddrinfo(NULL, PORT, &hints, &servinfo))!=0)
       {
           fprintf(fp,"error at getaddrinfo\n");
           return 1;
       }
    fprintf(fp, "address loading success\n");
    fprintf(fp, "flags=%d\n", servinfo->ai_flags);
    fprintf(fp, "family=%d\n", servinfo->ai_family);
    fprintf(fp, "socket=%d\n", servinfo->ai_socktype);
    fprintf(fp, "protocol=%d\n", servinfo->ai_protocol);
    fprintf(fp, "addrlen=%d\n", servinfo->ai_addrlen);
    
    for(p=servinfo;p != NULL; p=p->ai_next)
    {
        if((sockfd=socket(p->ai_family, p->ai_socktype, p->ai_protocol))==-1)
        {
            fprintf(fp,"error:address already in use\n");
            continue;
        }
        fprintf(fp, "sockfd=%d\n", sockfd);
        
        if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int))==-1)
        {
            fprintf(fp,"error:setsockopt\n");
            exit(1);
        }
        
       if(bind(sockfd, p->ai_addr, p->ai_addrlen) == -1)
        {
            close(sockfd);
            fprintf(fp,"error:binding\n ");
            continue;
        }
        break;
    }
    fprintf(fp,"binding succesful\n");
    
   if(p==NULL)
    {
        fprintf(fp, "p is NULL..better to give ip\n ");
        return 2;
    }
    
    freeaddrinfo(servinfo);
    fprintf(fp, "entering into listening phase\n");
    fflush(fp);
    
    if(listen(sockfd, MAXCLIENTS)==-1)
    {
        fprintf(fp, "error:listen ");
        exit(1);
    }
    
    sa.sa_handler=sigchld_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags=SA_RESTART;
    if(sigaction(SIGCHLD, &sa, NULL)==-1)
    {
        fprintf(fp,"error:sigaction ");
        exit(1);
    }
    
    fprintf(fp, "server:waiting for connections..\n ");
    fflush(fp);
    
    while(1)
    {
        sleep(1);
        fprintf(fp, "server daemon starting again..\n");
        fflush(fp);
        sin_size=sizeof clients_addr;
        if((newfd=accept(sockfd, (struct sockaddr *)&clients_addr, &sin_size))==-1)
        {
            fprintf(fp, "error:accept ");
            fflush(fp);
            continue;
        }
        
        inet_ntop(clients_addr.ss_family, get_in_addr((struct sockaddr *)&clients_addr), s, sizeof s);
        fprintf(fp, "server:rxd connection from %s\n", s);
        fflush(fp);
        if (!fork( ))
        { // this is the child process
            close(sockfd); // child doesn't need the listener
            if (send(newfd, "Hello, excuse me!", 13, 0) == -1)
                fprintf(fp, "error: sending data\n");
            
            numbytes = recv(newfd, buf, MAXDATASIZE, 0);
            if (numbytes == -1) {
                fprintf(fp, "error: receiving data");
                exit(1);
            }
            buf[numbytes] = '\0';
            
            fprintf(fp, "server: received '%d' bytes and the data is:\n'%s'\n", numbytes, buf);
            /*fprintf(fp, "buffer0=%sbuffer1=%s",buf[0], buf[1]);*/
            fflush(fp);
            
            char *dirname = "/users/duggina/version3";
            struct    dirent *ep;    /* struct dirent to store all files*/
            struct stat filestat;
            DIR *dp;    /*DIR Pointer to open Dir*/
            
            if((dp = opendir(dirname))==NULL)
            {
                printf("unable to open dirname\n");
                exit(0);
            }
            chdir(dirname);
            char sbuf1[MAXDATASIZE];
            char nofiles[5], fnamesize[3], fname[100], fsize[5];
            int nfiles, fnamelen, flen, off=0, i,j=0,k;
            FILE *fp1;
            
            for(i=off; i<=off+2; i++)
            {
                nofiles[j]=buf[i];
                j++;
            }
            j=0;
            fprintf(fp, "nofiles=%s", nofiles);
            
            nfiles=atoi(nofiles);
            fprintf(fp, "nfiles=%d", nfiles);
            for(k=0;k<=nfiles;k++)
            {
            /* trying to get length of file name */
            for(i=off+3; i<=off+4; i++)
            {
                fnamesize[j]=buf[i];
                j++;
            }
            j=0;
            fprintf(fp, "sz=%s", fnamesize);       
            fnamelen=atoi(fnamesize);
             fprintf(fp, "len=%d", fnamelen);
            if(fnamelen==0){fprintf(fp, "fnamelen is not a valid nmbr\n");}
            /* trying to get file name */
            for(i=off+5; i<=off+fnamelen+4; i++)
            {
                fname[j]=buf[i];
                j++;
            }
            j=0;
                fprintf(fp, "nm=%s", fname);
            fname[i]='\0';
            /* comparing filename with filenames in the directory */
            /* if a match occurs, copy the content of sbuf1 into the file */
          
                for(i=off+fnamelen+5; i<=off+fnamelen+8; i++)
                {
                    fsize[j]=buf[i];
                    j++;
                }
                j=0;
                fprintf(fp, "fsz=%s", fsize);
                flen=atoi(fsize);
                fprintf(fp, "flen=%d", flen);
                fflush(fp);
                fp1=fopen(fname, "w");
                for(i=off+fnamelen+9;i<=off+fnamelen+flen+7;i++)
                {
                    sbuf1[j]=buf[i];
                    j++;
                }
                j=0;
                fprintf(fp, "sbuf=%s\n\n\n", sbuf1);
                fflush(fp);
                int write_sz = fwrite(sbuf1, sizeof(char), flen, fp1);
                bzero(sbuf1, MAXDATASIZE);
                off=off+fnamelen+flen+8;
                fprintf(fp, "off=%d\n", off);
            }
            
            close(newfd);
            exit(0);
        }
        close(newfd);
 }
    
fclose(fp);
return (0);
}
