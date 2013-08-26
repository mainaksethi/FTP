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
#define Idle 14
#define Connected 15

void service_get_file();
void service_put_file();
void service_rec_get();
void service_change_server_directory();
void service_list_server_directory();

int req;
unsigned char buf[266];
char str2[50];
char str[16];
int socketConnection;
unsigned char contents[10][1000];
int socket_data;

int main()
{
 struct sockaddr_in socketInfo, socketInfo_data;
 char sysHost[257];
 int socketHandle;
 bzero(&socketInfo, sizeof(socketInfo));
 if((socketHandle = socket(AF_INET, SOCK_STREAM, 0)) == -1)
 {
  perror("Error in creating socket");
  exit(EXIT_FAILURE);
 }
 socketInfo.sin_family = AF_INET;
 socketInfo.sin_addr.s_addr = htonl(INADDR_ANY);
 socketInfo.sin_port = htons(Control_Port);
 if( bind(socketHandle, (struct sockaddr *) &socketInfo, sizeof(socketInfo)) == -1)
 {
  perror("bind");
  exit(EXIT_FAILURE);
 }
 listen(socketHandle, 1);
 
 if( (socketConnection = accept(socketHandle, NULL, NULL)) < 0)
 {
  perror("error in accept");
  exit(EXIT_FAILURE);
 }
 close(socketHandle);

 strcpy(buf,"Connection granted");
 send(socketConnection, buf, strlen(buf)+1, 0);
 recv(socketConnection, buf, 266, 0);
 memcpy(str,buf,6);
 int req = atoi(str);
 if (req==0)
	{
		memcpy(str,buf+6,16);
		int port = atoi(str);
		int socket_fd = socket(AF_INET, SOCK_STREAM, 0) ;
		if(socket_fd == -1)
		{
			  perror("Error in creating socket");
	 		  exit(EXIT_FAILURE);
		}
		bzero(&socketInfo_data, sizeof(socketInfo_data));
		socketInfo_data.sin_family = AF_INET;
	 	socketInfo_data.sin_addr.s_addr = htonl(INADDR_ANY);
	 	socketInfo_data.sin_port = htons(port);
	 	if( bind(socket_fd, (struct sockaddr *) &socketInfo_data, sizeof(socketInfo_data)) == -1)
	 	{
	  		perror("bind");
	  		exit(EXIT_FAILURE);
	 	}
	 	if(listen(socket_fd, 1) == -1)
		{
			perror("Error in listening");
			exit(EXIT_FAILURE);
		}
		strcpy(str,"Transfer");
		memcpy(buf,str,16);	 		
		send(socketConnection, buf, 266, 0);
 		if( (socket_data = accept(socket_fd, NULL, NULL)) < 0)
 		{
  			perror("error in accept");
  			exit(EXIT_FAILURE);
 		}
 	}

 while(1)
 	{
   		recv(socketConnection, buf, 266, 0);
   		memcpy(str,buf,6);
	   	req = atoi(str);
		//printf("%d\n",req);
   		if(req == 1)				
   			service_get_file();
   		
		if(req == 2)
			service_put_file();
  		
		if(req == 3)
			service_change_server_directory();
   		
		if(req == 4 || req == 5 )
			service_list_server_directory();

		if(req == 6)
			service_rec_get();
		
		if(req == 20)
  			{
			close(socketConnection);
			close(socket_data);
			exit(0);
  		 	}
 	}
 return 0;
}

void service_get_file()
 {
 memcpy(str2,buf+6,50);
 FILE *fp = fopen(str2,"rb");
 int i = 0;
 int num = 0;
 //printf("%s\n","hahah");
 while(fp)
	{
	//printf("%s\n","hahah");
	num = fread(contents[i],sizeof(unsigned char),1000,fp);
	send(socket_data,contents[i],num,0);
	//printf("%d\n",num);
	//printf("%s\n",contents[i]); 	
	if(num<1000)
		break;
	else
		i=(i+1)%10;
	}
 fclose(fp);
 //printf("%s\n","hahah");
 }

void service_put_file()
 {
 memcpy(str2,buf+6,50);
 FILE *fp = fopen(str2,"wb");
 int i = 0;
 int num = 0;
 //printf("%s\n","hahah");
 while(1)
	{
	//printf("%s\n","hahah");
	num = recv(socket_data, contents[i], 1000, 0);
	fwrite(contents[i],sizeof(unsigned char),num, fp);
	if (num<1000)
		break;
	else
		i=(i+1)%10;
	}
 fclose(fp);
 char *str3;
 str3 = "File Successfully Transferred";
 memcpy(buf,str3,60);
 send(socketConnection,buf,266,0);
 } 


void service_rec_get()
{
 char str[60];
 char str2[32];
 memcpy(str,buf+6,60);
 struct stat buf2;
 int a  = stat(str,&buf2);
 unsigned int d;
 if(a == -1)
	perror("Error in stat");
 else
 {
	d = (unsigned int)(buf2.st_mode);
	sprintf(str2,"%d",d);
	memcpy(buf,str2,32);
	for (a=32;a<266;a++)
		buf[a] = 0;
	send(socketConnection,buf,266,0);
 }
 struct dirent *entry;
 DIR *dir;
 dir = opendir(str);
 if(dir == NULL)
 {
  perror("Error in opening the directory");
 } 
 int i=0,rc,j;
 while((entry = readdir(dir)))
 {
	if(strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name,"..") != 0)
	{
	if(entry->d_type == DT_DIR)
	{
		contents[i][0] = '$';
		rc= sprintf(contents[i]+1,"%s",entry->d_name);
		rc++;
	}
	else
	{
		rc = sprintf(contents[i],"%s",entry->d_name);
	}
	for(j = rc;j<1000;j++)
		contents[i][j] = 0;
	send(socketConnection,contents[i],1000,0);
	i = (i+1)%10;
	}
 }
 contents[i][0] = '#';
 send(socketConnection,contents[i],1000,0);
 closedir(dir);
}


void service_list_server_directory()
{
 char str[40];
 getcwd(str,40);
 struct dirent *entry;
 DIR *dir;
 dir = opendir(str);
 if(dir == NULL)
 	perror("Error in opening the directory");
 int i=0,rc,j;
 while((entry = readdir(dir)))
 	{
	rc= sprintf(contents[i],"%s",entry->d_name);
	for(j = rc;j<1000;j++)
		contents[i][j] = 0;
	send(socket_data,contents[i],1000,0);
	i = (i+1)%10;
 	}
 contents[i][0] = '#';
 send(socket_data,contents[i],1000,0);
 closedir(dir);
}


void service_change_server_directory()
{
 char str2[50];
 memcpy(str2,buf+6,50);
 if(chdir(str2) == -1)
	 perror("Error in changing the server directory") ;
}
