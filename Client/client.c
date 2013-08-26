#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdlib.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/stat.h>
#include<time.h>
#include<pthread.h>
#include<assert.h>
#include<string.h>
#include<errno.h>
#include<netdb.h>
#include<dirent.h>

#define Control_Port 6707
#define Data_Port 5707
#define Idle 14
#define Connected 15

char c[50][50];
int req;
unsigned char buf[266];
char str[6];
char str1[16];
char str2[50];
unsigned char contents[10][1000];
unsigned char contents2[20][50];
unsigned char contents3[20][50];
unsigned char buf2[20][20];
int socket_fd;

void get_file(int,char*);
void put_file(int,char*);
void rec_get(int,char*);
void change_client_local_current_directory();
void list_client_directory();
void change_server_directory(int);
void list_server_directory(int);
void get_wildcard(int);
void put_wildcard(int);


int main(int argc, char *argv[])
{
 //printf("%s\n","haha");
 struct sockaddr_in remoteSocketInfo, remoteSocketDataInfo;
 int socketHandle,i,state = Idle,j;
 bzero(&remoteSocketInfo,sizeof(remoteSocketInfo));
 if((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) < 0)
 {
  perror("Error in creating socket");
  exit(EXIT_FAILURE);
 }
 remoteSocketInfo.sin_family = AF_INET;
 remoteSocketInfo.sin_port = htons(Control_Port);
 int a = inet_pton(AF_INET, argv[1], &remoteSocketInfo.sin_addr);	//converting the Ipv4 addresses from text to binary form
 if( a<= 0)
 {
	if(a == 0)
		fprintf(stderr,"Not a valid address");
	else
		perror("Error in converting IPv4 address from text to binary form");
 }
 if(connect(socketHandle, (struct sockaddr *)&remoteSocketInfo, sizeof(remoteSocketInfo)) < 0)
 {
  perror("Error in establishing connection");
  exit(EXIT_FAILURE);
 }

 int rc = recv(socketHandle, buf, 266, 0);
 //printf("%s\n",buf);
 buf[rc]=0;
 
 if(strcmp(buf,"Connection granted")==0)
	{
		req=0;
		sprintf(str,"%d",req);
		int port = Data_Port;
		sprintf(str1,"%d",port);
		memcpy(buf,str,6);
		memcpy(buf+6,str1,16);
		for(i=22;i<266;i++)
			buf[i]=0;
		send(socketHandle,buf,266,0);
	  	socket_fd = socket(AF_INET, SOCK_STREAM, 0) ;
		if(socket_fd == -1)
		 	{
	  		perror("Error in creating socket");
	  		exit(EXIT_FAILURE);
		 	}
		bzero(&remoteSocketDataInfo,sizeof(remoteSocketDataInfo));
		remoteSocketDataInfo.sin_family = AF_INET;
	 	remoteSocketDataInfo.sin_port = htons(Data_Port);
	 	a = inet_pton(AF_INET, argv[1], &remoteSocketDataInfo.sin_addr);	//converting the Ipv4 addresses from text to binary form
	 	if( a<= 0)
		 	{
			if(a == 0)
				fprintf(stderr,"Not a valid address");
			else
				perror("Error in converting IPv4 address from text to binary form");
		 	}
		recv(socketHandle, buf, 266, 0);
		memcpy(str1,buf,16);
		if(strcmp(str1,"Transfer")==0)
			{
		 	if(connect(socket_fd, (struct sockaddr *)&remoteSocketDataInfo, sizeof(remoteSocketDataInfo)) < 0)
			 	{
		  		perror("Error in establishing connection");
		  		exit(EXIT_FAILURE);
			 	}
			else
				{
				state = Connected;
				printf("%s\n",buf);
				}
			}
	}

 if(state==Connected)
	{
		while(1)
			{
			printf("Enter the option or 'quit' to terminate\n");			
			scanf("%s",str1);

			if(strcmp(str1,"get_file")==0)
				{
				printf("Enter the file name\n");			
				scanf("%s",str2);
				get_file(socketHandle,str2);
				}

			if(strcmp(str1,"put_file")==0)
				{
				printf("Enter the file name\n");			
				scanf("%s",str2);
				put_file(socketHandle,str2);
				}

			if(strcmp(str1,"rget")==0)
				{
				printf("Enter the path\n");			
				scanf("%s",str2);
				rec_get(socketHandle,str2);
				}	

			if(strcmp(str1,"lcd") == 0)
				change_client_local_current_directory();
  	 		
			if(strcmp(str1,"cd") == 0)
				change_server_directory(socketHandle);

			if(strcmp(str1,"ldir") == 0)
				list_client_directory();
			
			if(strcmp(str1,"dir") == 0)
				list_server_directory(socketHandle) ;
			
			if(strcmp(str1,"mget") == 0)
				{
				int k=0;				
				printf("Enter the file name or enter '$' to exit\n");				
				scanf("%s",c[k]);
				i=0;
				while(1)
					{
					if (strcmp(c[k],"$")!=0)
						{
						get_file(socketHandle,c[k]);
						k++;						
						printf("Enter another file name or enter '$' to exit\n");				
						scanf("%s",c[k]);
						}
					else
						break;
					}
				}

			if(strcmp(str1,"mget*") == 0)
			 	get_wildcard(socketHandle);
	 
			if(strcmp(str1,"mput*") == 0)
				put_wildcard(socketHandle);
			
			if(strcmp(str1,"mput") == 0)
				{
				int k=0;
				printf("Enter the file name or enter '$' to exit\n");
				scanf("%s",c[k]);
				i=0;
				while(1)
					{
					if(strcmp(c[k],"$")!=0)
						{
						put_file(socketHandle,c[k]);
						k++;
						printf("Enter another file name or enter '$' to exit\n");				
						scanf("%s",c[k]);
						}
					else
						break;
					}
				}
			
			if(strcmp(str1,"quit")==0)
				{
				sprintf(str,"%d",20);
 				memcpy(buf,str,6);
				send(socketHandle,buf,266,0);
				close(socketHandle);
				close(socket_fd);				
				break;
				}

			}
	}			
 return 0;	
 }

void get_file(int sock_handle,char *name)
{
 int i=0;
 req=1;
 sprintf(str,"%d",req);
 strcpy(str2,name);
 memcpy(buf,str,6);
 memcpy(buf+6,str2,50);
 for(i=56;i<266;i++)
	buf[i]=0;
 send(sock_handle,buf,266,0);
 FILE *fp= fopen(str2,"wb");
 i=0;
 int num=0;
 while(1)
	{
	num = recv(socket_fd, contents[i], 1000, 0);
	fwrite(contents[i],sizeof(unsigned char),num,fp);
	//printf("%s\n",contents[i]);	
	if(num<1000)
		break;
	else
		i=(i+1)%10;
	}
 fclose(fp);
 //printf("%s\n","hahah");
}
	
 
void put_file(int sock_handle,char *name)
{
 int i=0;
 req = 2;
 sprintf(str,"%d",req);
 memcpy(buf,str,6);
 strcpy(str2,name);
 memcpy(buf+6,str2,50);
 for(i=56;i<266;i++)
	buf[i]=0;
 send(sock_handle,buf,266,0);
 FILE *fp = fopen(str2,"rb");
 i=0;
 int num=0;
 while(fp)
	{
		num = fread(contents[i],sizeof(unsigned char),1000,fp);
		send(socket_fd,contents[i],num,0);
		if(num<1000)
			break;
		else
			i=(i+1)%10;
	}
 fclose(fp);
 recv(sock_handle,buf,266,0);
}


void get_wildcard(int sock_handle)
{
 req = 5;
 char str[6];
 char str1[16];
 int i,j,k;
 printf("Enter the wildcard (Do not put '*')\n");
 scanf("%s",str1);
 sprintf(str,"%d",req);
 memcpy(buf,str,6);
 for(i=0;i<260;i++)
  	buf[i+6] = 0;
 send(sock_handle,buf,266,0);
 i=0;
 k=0;
 while(1)
 {
  recv(socket_fd, contents[i], 1000, 0);
  if(contents[i][0] == '#')
	break;
  j=0;
  while(contents[i][j] != '.' && contents[i][j] != 0)
	j++;
  if(strcmp(str1,contents[i]+j) == 0)
  	strncpy(buf2[k++],contents[i],60);
  i = (i+1)%10;
 }
 while(k--)
 {
   	get_file(sock_handle,buf2[k]);
 } 
}

void rec_get(int sock_handle, char *name)
{ 
 req = 6;
 int index1=0;
 int index2=0;
 int index3=0;
 int index4=0;
 int i;				//i=index1,j=index2,k=index3
 sprintf(str,"%d",req);
 memcpy(buf,str,6);
 //printf("%s\n",name);
 strcpy(str2,name);
 //printf("%s\n",str2);
 memcpy(buf+6,str2,60);
 printf("%s\n",buf);
 for(i=0;i<216;i++)
	buf[i+56]=0;
 printf("%s\n",buf);
 send(sock_handle,buf,266,0);
 printf("%s\n",buf);
 recv(sock_handle,buf,266,0);
 printf("%s\n",buf);
 char temp1[32];
 char temp2[50];
 memcpy(temp1,buf,32);			//temp1=str5
 mode_t mode = atoi(temp1);
 mkdir(name,mode);
 printf("%s\n",temp1);
 while(1)
 	{
  	recv(socket_fd, contents[index1], 1000, 0);
	printf("%s\n",contents[index1]);  	
	if(contents[index1][0] == '#')
		break;
  	strcpy(temp2,name);			//temp2=str4
  	strcat(temp2,"/");
  	if(contents[index1][0] != '$')
  		{strcat(temp2,contents[index1]);strcpy(contents2[index2++],temp2);}		//contents2=buf3
  	else
  		{strcat(temp2,contents[index1]+1);strcpy(contents3[index4++],temp2);}			//contents3=buf4
  	i = (i+1)%10;
 	}

 for(index3=0;index3<index2;index3++)
	get_file(sock_handle,contents2[index3]);
 for(index3=0;index3<index4;index3++)
	rec_get(sock_handle,contents3[index3]);
 }
  
void put_wildcard(int sock_handle)
{
 char str[60];
 getcwd(str,60);
 struct dirent *entry;
 DIR *dir;
 int i;
 char str1[16];
 printf("Enter the wildcard (Do not put '*')\n");
 scanf("%s",str1);
 dir = opendir(str);
 if(dir == NULL)
 {
  perror("Error in opening the directory");
 } 
 while((entry = readdir(dir)))
 {
	i=0;
  	while(entry->d_name[i] != '.' && entry->d_name[i] != 0)
		i++;
	if(strcmp(str1,entry->d_name + i) == 0)
		put_file(sock_handle,entry->d_name);
 }
 closedir(dir); 
}

void change_client_local_current_directory()	
{
 char directory[50]; 
 printf("Enter the path\n");
 scanf("%s",directory);
 if(chdir(directory) == -1)
 	perror("Error in changing the local current directory") ;
}


void list_client_directory()
{
 char str[50];
 getcwd(str,50);
 struct dirent *entry;
 DIR *dir;
 dir = opendir(str);
 if(dir == NULL)
 	perror("Error in opening the directory");
  
 while((entry = readdir(dir)))
	printf("%s \n",entry->d_name);
 closedir(dir);
}


void change_server_directory(int sock_handle)
{
 req = 3;
 char str[50];
 char str1[6];
 int i;
 printf("Enter the path\n");
 scanf("%s",str);
 sprintf(str1,"%d",req);
 memcpy(buf,str1,6);
 memcpy(buf+6,str,50);
 for(i=56;i<266;i++)
	buf[i] = 0;
 send(sock_handle,buf,266,0);
}

void list_server_directory(int handle)
{
 req = 4;
 char str[6];
 int i;
 sprintf(str,"%d",req);
 memcpy(buf,str,6);
 for(i=0;i<260;i++)
  	buf[i+6] = 0;
 send(handle,buf,266,0);
 i=0;
 while(1)
 	{
  	recv(socket_fd, contents[i], 1000, 0);
  	if(contents[i][0] == '#')
		break;
  	printf("%s \n",contents[i]);
  	i = (i+1)%10;
 	}
}
