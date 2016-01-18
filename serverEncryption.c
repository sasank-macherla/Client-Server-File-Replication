#include <sys/types.h> 
#include <sys/socket.h> 
#include <stdio.h> 
#include <arpa/inet.h> 
#include <string.h> 
#include <fcntl.h> 
#include <unistd.h>
#include <openssl/des.h>

char *
Encrypt( char *Key, char *Msg, int size)
{ 
        static char*    Res;
        int             n=0;
        DES_cblock      Key2;
        DES_key_schedule schedule;
 
        Res = ( char * ) malloc( size );
 
        /* Prepare the key for use with DES_cfb64_encrypt */
        memcpy( Key2, Key,8);
        DES_set_odd_parity( &Key2 );
        DES_set_key_checked( &Key2, &schedule );
 
        /* Encryption occurs here */
        DES_cfb64_encrypt( ( unsigned char * ) Msg, ( unsigned char * ) Res,
                           size, &schedule, &Key2, &n, DES_ENCRYPT );
          return (Res);
}
main() 
{ 
 int sd,cd; 
 char buf[1000]=""; 
 char *encrypted;
 char key[]="password";
 struct sockaddr_in ser; 
 sd=socket(AF_INET,SOCK_STREAM,0); 
 if(sd<0) 
 printf("SOCKET NOT CREATED\n"); 
 bzero(&ser,sizeof(struct sockaddr_in)); 
 ser.sin_family=AF_INET; 
 ser.sin_port=htons(1101); 
 inet_aton("localhost",&ser.sin_addr); 
 int b=bind(sd,(struct sockaddr *)&ser,sizeof(ser)); 
 printf("BIND VALUE:%d\n",b); 
 listen(sd,5); 
 for(;;) 
 {  
 cd=accept(sd,NULL,NULL); 
 int pid=fork(); 
 if(pid==0) 
 {  
 printf("accept value %d\n",cd); 
 read(cd,buf,1000); 
 int fd=open(buf,O_RDONLY); 
 read(fd,buf,1000); 
 write(cd,buf,strlen(buf)); 
 encrypted=malloc(sizeof(buf));
 memcpy(encrypted,Encrypt(key,buf,sizeof(buf)), sizeof(buf));
 printf("Encrypted text file from client\t : %s \n",encrypted);
 close(cd); 
 }
} 
 close(sd); 
}
