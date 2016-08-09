/*FTP Client*/
 #include <sys/socket.h> 
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
      
/*for getting file size using stat()*/
#include<sys/stat.h>
 
/*for sendfile()*/
#include<sys/sendfile.h>
 
/*for O_RDONLY*/
#include<fcntl.h>
#define PORT     2000
 
int main(int argc,char *argv[])
{
	struct sockaddr_in server;
	struct stat obj;
  	int sock;
  	int choice;
  	char buf[100], command[5], filename[20], *f;
  	int k, size, status;
  	int filehandle;
	FILE *fp1,*fp2;
	char line[128];
	size_t len = 0;
       	ssize_t read;
	char answer[100];
  	sock = socket(AF_INET, SOCK_STREAM, 0);
  	if(sock == -1)
    	{
      		printf("socket creation failed\n");
      		exit(1);
    	}
  	server.sin_family = AF_INET;
  	server.sin_port = htons(PORT);
  	server.sin_addr.s_addr =htonl(INADDR_ANY);
  	k = connect(sock,(struct sockaddr*)&server, sizeof(server));
  	if(k == -1)
    	{
      		printf("Connect Error\n");
      		exit(1);
    	}
  	int i = 1;
  	while(1)
    	{
      		printf("Enter a choice:\n1- get\n2- Take test and submit\n3- List of Study Materials\n4- pwd\n5- ls\n6- quit\n");
      		scanf("%d", &choice);
      		switch(choice)
		{
			case 1:
	  			printf("Enter filename to get: ");
	  			scanf("%s", filename);
	  			strcpy(buf, "get ");
	  			strcat(buf, filename);
	  			send(sock, buf, 100, 0);
	  			recv(sock, &size, sizeof(int), 0);
	  			if(!size)
	    			{
	      				printf("No such file on the remote directory\n\n");
	    				break;
	    			}
	  			f = malloc(size);
	  			recv(sock, f, size, 0);
	  			while(1)
	    			{
	      				filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
	      				if(filehandle == -1)
					{
		  				sprintf(filename + strlen(filename), "%d", i);//needed only if same directory is used for both server and client
					}
	      				else break;
	    			}
	  			write(filehandle, f, size, 0);
				printf("%s",f);
	  			close(filehandle);
	  			strcpy(buf, "cat ");
	  			strcat(buf, filename);
	  			system(buf);
	  			break;
			case 2:

				fp1 = fopen("questions.txt","r");
				while(fgets ( line, sizeof(line), fp1 ) != NULL)
				/*while((getline(&line, &len, fp1))!=NULL)*/
				{
					/*read = getline(&line, &len, fp1);
					if(feof(fp1))
					{
						break;
					}*/
					printf("%s", line);
					printf("Answer: \n");
					scanf("%s",answer);
					fp2=fopen("answer.txt","a");
        				fputs(answer,fp2);
					fputs("\n",fp2);
					fclose(fp2);
				}
				fclose(fp1);
				/*if(fp1==NULL)
					exit(EXIT_FAILURE);
				while((read=getline(&line,&len,fp1))!=-1)
				{
					printf("%s", line);
					printf("Answer: \n");
					scanf("%s",answer);
					fp2=fopen("answer.txt","a");
        				fputs(answer,fp2);
					fputs("\n",fp2);
					fclose(fp2);
					if(feof(fp1))
					{
						break;
					}
				}
				fclose(fp1);*/
				strcpy(filename,"answer.txt");
  				filehandle = open(filename, O_RDONLY);
       				if(filehandle == -1)
          			{
              				printf("No such file on the local directory\n\n");
              				break;
            			}
          			strcpy(buf, "put ");
	  			strcat(buf, filename);
				printf("request is %s\n",buf);
	  			send(sock, buf, 100, 0);
				stat(filename, &obj);
	  			size = obj.st_size;
	  			send(sock, &size, sizeof(int), 0);
	  			sendfile(sock, filehandle, NULL, size);
	  			recv(sock, &status, sizeof(int), 0);
	  			if(status)
	    				printf("File stored successfully\n");
	  			else
	    				printf("File failed to be stored to remote machine\n");
	  			break;
			case 3:
	  			strcpy(buf, "get ");
	  			strcat(buf, "study_materials.txt");
	  			send(sock, buf, 100, 0);
	  			recv(sock, &size, sizeof(int), 0);
	  			if(!size)
	    			{
	      				printf("No such file on the remote directory\n\n");
	    				break;
	    			}
	  			f = malloc(size);
	  			recv(sock, f, size, 0);
	  			while(1)
	    			{
	      				filehandle = open(filename, O_CREAT | O_EXCL | O_WRONLY, 0666);
	      				if(filehandle == -1)
					{
		  				sprintf(filename + strlen(filename), "%d", i);//needed only if same directory is used for both server and client
					}
	      				else break;
	    			}
	  			write(filehandle, f, size, 0);
				printf("Study Materials \n");
				printf("%s",f);
				close(filehandle);
	  			  					
			case 4:
				strcpy(buf, "pwd");
	  			send(sock, buf, 100, 0);
	  			recv(sock, buf, 100, 0);
	  			printf("The path of the remote directory is: %s\n", buf);
	  			break;
			case 5:
	  			strcpy(buf, "ls");
          			send(sock, buf, 100, 0);
	  			recv(sock, &size, sizeof(int), 0);
          			f = malloc(size);
          			recv(sock, f, size, 0);
	  			filehandle = creat("temp.txt", O_WRONLY);
	  			write(filehandle, f, size, 0);
	  			close(filehandle);
          			printf("The remote directory listing is as follows:\n");
	  			system("cat temp.txt");
	  			break;
			case 6:
	  			strcpy(buf, "quit");
          			send(sock, buf, 100, 0);
          			recv(sock, &status, 100, 0);
	  			if(status)
	    			{
	      				printf("Server closed\nQuitting..\n");
	      				exit(0);
	    			}	
	    			printf("Server failed to close connection\n");
		}
	}
}

