#include <errno.h>       
#include <fcntl.h>       
#include <stdio.h>       
#include <stdlib.h>      
#include <string.h>      
#include <sys/event.h>   
#include <unistd.h>      
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PATH_MAX 1000

int main(int argc, char* argv[ ])
{
    if (argc != 2) {
        printf("enter some directory name in argv[1]\n");
        exit(0);
    }
    FILE *fp= NULL;
    pid_t process_id = 0;
    pid_t sid = 0;
    process_id = fork( );
    if (process_id < 0)
    {
        printf("fork failed!\n");
        exit(1);
    }
    if (process_id > 0)
    {
        printf("process_id of child process %d \n", process_id);
        exit(0);
    }
    umask(0);
    sid = setsid( );
    printf("sid=%d\n",sid);
    if(sid < 0)
    {
        exit(1);
    }
    chdir("/");
    printf("changed to directory 'ROOT'\n");
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    fp = fopen ("log2.txt", "w+");
    fprintf(fp, "entering into directory monitoring mode..\n");
    fflush(fp);
    
    int kq = kqueue ( );
    int dirfd = open (argv[1], O_RDONLY);
    
    struct kevent direvent;
    EV_SET (&direvent, dirfd, EVFILT_VNODE, EV_ADD | EV_CLEAR | EV_ENABLE,
            NOTE_WRITE, 0, (void *)argv[1]);
    kevent(kq, &direvent, 1, NULL, 0, NULL);
    struct kevent sigevent;
    EV_SET (&sigevent, SIGINT, EVFILT_SIGNAL, EV_ADD | EV_ENABLE, 0, 0, NULL);
    signal (SIGINT, SIG_IGN);
    kevent(kq, &sigevent, 1, NULL, 0, NULL);
    fprintf(fp, "entering while loop\n");
    fflush(fp);
    system("ditto /users/duggina/version1 /users/duggina/version2");
    while (1) {
        struct kevent change;
        if (kevent(kq, NULL, 0, &change, 1, NULL) == -1) { exit(1); }
        if (change.udata == NULL) {
            break;
        }
        else
        {
            fprintf (fp, "changes made to ' %s ' directory\n", (char*)change.udata);
            fflush(fp);
            FILE *fp1, *fp2, *fp3;
            char path[PATH_MAX];
            if((fp1 = popen("diff /users/duggina/version1 /users/duggina/version2","r"))!=NULL)
            {
                while (fgets(path, PATH_MAX, fp1) != NULL)
                    fprintf(fp, "diff:%s", path);
                fflush(fp);
            }
            pclose(fp1);
            
            if((fp2 = popen("./users/duggina/dircpy /users/duggina/version1","r"))!=NULL)
            {
                while (fgets(path, PATH_MAX, fp2) != NULL)
                    fprintf(fp, "dircpy:%s", path);
                fflush(fp);
            }
            pclose(fp2);
            
            fprintf(fp, "calling client program:\n");
            fflush(fp);
                       
            if((fp3 = popen("./users/duggina/d2 localhost /users/duggina/version1", "r"))!=NULL)
            {
                while (fgets(path, PATH_MAX, fp3) != NULL)
                    fprintf(fp, "client:%s", path);
            fflush(fp);
            }
            pclose(fp3);
            fprintf(fp, "server got news\n");
            fflush(fp);
        }
    }
    close (kq);
    return 0;
}

