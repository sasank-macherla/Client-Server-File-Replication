#include <netinet/in.h> 
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
 char *
Decrypt( char *Key, char *Msg, int size)
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
        /* Decryption occurs here */
        DES_cfb64_encrypt( ( unsigned char * ) Msg, ( unsigned char * ) Res,
                           size, &schedule, &Key2, &n, DES_DECRYPT );
        return (Res);
}
main() 
{ 
 int sd,cd; 
 char buf[1000]="",buf1[1000]=""; 
 char key[]="password";
 char *decrypted;
 char *encrypted;
 struct sockaddr_in ser; 
 sd=socket(AF_INET,SOCK_STREAM,0); 
 if(sd<0) 
 printf("SOCKET NOT CREATED\n"); 
 bzero(&ser,sizeof(struct sockaddr_in)); 
 ser.sin_family=AF_INET; 
 ser.sin_port=htons(1101); 
 inet_aton("localhost",&ser.sin_addr); 
 connect(sd,(struct sockaddr *)&ser,sizeof(ser)); 
 for(;;) 
 { 
 printf("ENTER THE MESSAGE"); 
 scanf("%s",buf);  
 write(sd,buf,strlen(buf));
 encrypted=malloc(sizeof(buf)); 
 read(sd,buf,1000); 
 decrypted=malloc(sizeof(buf));
 memcpy(encrypted,Encrypt(key,buf,sizeof(buf)), sizeof(buf));
 printf("Encrypted text file sent to server\t : %s \n",encrypted);
 memcpy(decrypted,Decrypt(key,encrypted,sizeof(buf)), sizeof(buf));
 printf("Received file from server which is decrypted \t : %s \n",decrypted);
 return (0);
 } 
 close(sd); 
}

