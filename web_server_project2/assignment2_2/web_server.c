//////////////////////////////////////////////////////////////////
//File Name	: 2021202057_web_server.c			//
//Date		: 2023/05/03					//
//OS		: Ubuntu 16.04 LTS 64bits			//
//Author	: HWang Se Young				//
//Student ID	: 2021202057					//
//-------------------------------------------			//
//Title : System Programming Assignment #2-2			//
//Description : Web_server					//
//////////////////////////////////////////////////////////////////
//fi_info							//
//==============================================================//
//Input: char* filepath -> filepath				//
//	 char* filenme -> file name				//
//		int client_fd ->client file descript
//		char* url->url
//Output: print ls -l element					//
//Purpose: print ls -l element					//
//////////////////////////////////////////////////////////////////
//ls_sort							//
//==============================================================//
//Input: int print_mode -> -a or -l or -la or non?		//
//	 char* dir_path -> directory path			//
//		int client_fd ->client file descript
//		char* url->url
//Output: x							//
//Purpose: sorting filename list				//
//////////////////////////////////////////////////////////////////


#define _GNU_SOURCE //fnmatch

#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <fnmatch.h>
#include <stdbool.h>
#include <fcntl.h>
#define URL_LEN 256
#define BUFSIZE 1024
#define PORTNO 40000


int lflag=0; 

void f_info(char* filepath,char* filenme, int client_fd, char* url);
void ls_sort(int print_mode,char* dir_path, int client_fd, char* url);
int main(int argc, char* argv[]){
	int print_mode = 0, aflag=0;
	char dir_path[199];
	int c=0;	
	opterr=0;
	

    struct sockaddr_in server_addr, client_addr; //define strcut
    int socket_fd, client_fd;
    int len, len_out;
    int opt = 1;

    if((socket_fd = socket(PF_INET, SOCK_STREAM,0))<0) //create a socketfile
    {
        printf("Server : can't open stream socket\n"); //error code
        return 0;
    }

    setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));//set optionof the socket

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); //initialisze the server adress
	//server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//setting
    server_addr.sin_port = htons(PORTNO); //port number is 40000

    if(bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr))<0)
    { //bind the socket file descirptor to hte server address
        printf("Serve err\n");
        return 0;
    }

    listen(socket_fd, 5); //listen

    while(1)
    {
        struct in_addr inet_client_address;
        char buf[BUFSIZE] ={0,};
        char tmp[BUFSIZE] ={0,};
        char response_header[BUFSIZE] ={0,}; //header buf
        char response_message[BUFSIZE] ={0,}; //messgae buf
        char url[BUFSIZE] ={0,};
        char method[20] ={0,};
        char* tok=NULL;

        len = sizeof(client_addr);
        client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &len);//accept incoming ceonnections
        if(client_fd < 0)
        {
            printf("server fail");
            return 0;
        }
        inet_client_address.s_addr = client_addr.sin_addr.s_addr; //set client adder
        printf("[%s : %d] client was connected\n",inet_ntoa(inet_client_address),client_addr.sin_port);
        read(client_fd, buf, BUFSIZE); //read request message from client
        strcpy(tmp, buf); //copy
        puts("==========================================");
        printf("Request from[%s : %d]\n",inet_ntoa(inet_client_address), client_addr.sin_port);
        puts(buf); //print inform
        puts("==========================================");

        tok = strtok(tmp, " ");
		if(tok==NULL){
			continue; //jump
		}
        strcpy(method, tok);
        if(strcmp(method, "GET")==0) //get url
        {
            tok = strtok(NULL," ");
            strcpy(url, tok);
        }
		//---------------------state----------------
		struct stat is_Dir;
		char real_url[1024];
		char dot_pt[199]=".";
		strcat(dot_pt, url);
		realpath(dot_pt, real_url); //get realpath
		int is_ex = lstat(real_url, &is_Dir); //is exist
		int state=0; //not eixt404 dir 1 justfile 2

		if(is_ex < 0){ //404
			state=0;
		}
		else{
			if(S_ISDIR(is_Dir.st_mode)){ state=1;} //dir->state=1
			else{state = 2;} //other file->state=2
		}
		//-------------------------------------------

		if(state == 0){ //if not exist directory
			sprintf(response_message, "<link rel='icon' href='data:,'><h1>Not Found</h1><br> <h3>The request URL %s was not found on this server<br>HTTP 404 - Not Page Found</h3>",url); //write error print
            sprintf(response_header, "HTTP/1.1 404 OK\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n", (int)strlen(response_message));
            write(client_fd, response_header, strlen(response_header)); //write header
            write(client_fd, response_message, strlen(response_message)); //write message

		}
		else if(state == 1){ //directory
			if(strcmp(url,"/")==0){ //root
				aflag=0; lflag=1; print_mode=2; strcpy(dir_path,"."); //set flag
				sprintf(response_header, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n", 100000);
            	write(client_fd, response_header, strlen(response_header)); //write header
				sprintf(response_message, "<link rel='icon' href='data:,'><h1>Welcome to System Programming Http</h1><br>");
    	        write(client_fd, response_message, strlen(response_message)); //write message
				ls_sort(print_mode, dir_path, client_fd, url); //funct call
			}
			else{ //child directory
				aflag=1; lflag=1; print_mode=3; //set flag
				sprintf(response_header, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n", 100000);
    	        write(client_fd, response_header, strlen(response_header)); //write header
				sprintf(response_message, "<link rel='icon' href='data:,'><h1>System Programming Http</h1><br>");
        	    write(client_fd, response_message, strlen(response_message)); //write message
				strcpy(dir_path,"."); //set path
            	strcat(dir_path,url); //relative path
				printf("---%s---iam state",dir_path);
				ls_sort(print_mode, dir_path, client_fd, url);
				//function call
			}

		}
		else if(state ==2){ //file
			if(fnmatch("*.jpg",url, FNM_CASEFOLD) == 0 ||fnmatch("*.png",url, FNM_CASEFOLD) == 0 ||fnmatch("*.jpeg",url, FNM_CASEFOLD) == 0 )
            { //if image file
				char url_tmp[1024];
				sprintf(url_tmp, ".%s",url); //set path
                FILE* file = fopen(url_tmp, "rb"); //read binary
				if(file ==NULL){ printf("file open error\n");}
				fseek(file, 0, SEEK_END); //fiel seek
            	long int file_size = ftell(file); //filesize return
				printf("%ld",file_size);
            	fseek(file, 0, SEEK_SET); //set strart
           	 	char* buffer = malloc(file_size); 
            	fread(buffer, file_size, 1, file); //file read
            	sprintf(response_header, "HTTP/1.1 200 OK\r\nContent-Type: image/*\r\nContent-Length:%ld\r\n\r\n", file_size);
				write(client_fd, response_header, strlen(response_header)); //write header
       			write(client_fd, buffer, file_size); //write message
        	    free(buffer);
            	fclose(file);


                close(client_fd);
                continue;
            }
            else if(fnmatch("*.html",url, FNM_CASEFOLD) == 0)
            {
				char url_tmp[1024];
				sprintf(url_tmp, ".%s",url); //set path
                FILE* file = fopen(url_tmp, "rb"); //read binary
				if(file ==NULL){ printf("file open error\n");}
				fseek(file, 0, SEEK_END); //file seek
            	long int file_size = ftell(file); //filesize return
            	fseek(file, 0, SEEK_SET); //set start
           	 	char* buffer = malloc(file_size); 
            	fread(buffer, file_size, 1, file); //file read

                fclose(file);
                sprintf(response_header, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%ld\r\n\r\n<link rel='icon' href='data:,'>\r\n\r\n", file_size);
                //sprintf(response_message, "%s",text_m);
                write(client_fd, response_header, strlen(response_header)); //write header
                write(client_fd, buffer, file_size); //write message
                close(client_fd);
                continue;
            }
            else
            {
				char url_tmp[1024];
				sprintf(url_tmp, ".%s",url); //set path
                FILE* file = fopen(url_tmp, "rb"); //read binary
				if(file ==NULL){ printf("file open error\n");} //filenot open
				fseek(file, 0, SEEK_END);
            	long int file_size = ftell(file); //filesize return
            	fseek(file, 0, SEEK_SET); //set start
           	 	char* buffer = malloc(file_size);
            	fread(buffer, file_size, 1, file); //file read
				
				printf("\n%s\n",buffer);
                fclose(file);
				sprintf(response_header, "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\nContent-Length:%ld\r\n\r\n", file_size);
               // sprintf(response_message, "%s", buffer);
                write(client_fd, response_header, strlen(response_header)); //wrtie header
               // write(client_fd, response_message, strlen(response_message));
				write(client_fd, buffer, file_size); //write filesize
                close(client_fd);
                continue;
            }
		}


        printf("[%s : %d] client was disconnect\n", inet_ntoa(inet_client_address), client_addr.sin_port);
        close(client_fd);
    }
    close(socket_fd);

	return 0;
}
void ls_sort(int print_mode,char* dir_path, int client_fd, char* url){ //file name list sort
	DIR *dir;
	if((dir=opendir(dir_path))==NULL){printf("cannnot access %s: No such file or directory \n",dir_path);return ;}
	struct dirent *entry; //if opendir is fail, error
	char **filelist = NULL; //filename list
	int n=0;
	while((entry = readdir(dir)) !=NULL){ //realloc
		filelist = (char**)realloc(filelist,sizeof(char *)*(n+1));
		filelist[n++]=strdup(entry->d_name); //put file name
	}
	closedir(dir);//close dir
	int i;
	//sort file name
	for(i=0; i<n-1;i++){ //n-1 loop
	  int j=i, min=i; //what is mini
	  for( j=j+1 ; j<n ; j++){
		char *a= filelist[min], *b=filelist[j]; //two filename cmp
		int k,p;
		if(a[0]=='.'&& b[0] !='.'){ //if a is hidden file
			for(k=0,p=1; a[p]&&b[k] ; k++,p++){ //. is ignore
			  int ca = tolower(a[p]), cb=tolower(b[k]);
			  if(ca != cb){ //if different
			    if(ca< cb) break; //pass
			    else { min=j ; break;} //store
			  }
			}
			if (!b[k] && a[p]) {min = j;} //store
		}
		else if(b[0]=='.'&& a[0]!='.'){ //if b is hidden file
			for(k=0,p=1 ; a[k] &&b[p] ; k++,p++){ //. is ignore
			 int ca = tolower(a[k]), cb=tolower(b[p]); //small alphabet=>int
			 if(ca != cb){ //if differnet
			   if(ca < cb)break; //pass
			   else{min = j; break;}
		     	 }
			}
			if(!b[p] && a[k]) {min=j;}
		}
		else{
			for(k=0; a[k]&&b[k] ;k++){ //if both not hidden or both hidden
			  int ca = tolower(a[k]), cb=tolower(b[k]);
			  if(ca != cb){
			    if(ca <cb) break; //same process
			    else{min=j; break;}
			  }
			}
			if(!b[k] && a[k]) {min=j;}
		}
	  }
	  if(min != i){ //if min and i not same
		char *tmp = filelist[i]; //swap
		filelist[i] = filelist[min];
		filelist[min] = tmp;
	  }

	}


	char buff[1024]; //store path
    char buf_msg[1024];
	int absol=0;
	realpath(dir_path, buff); //relative->absolute
	//absolute
	//just file or directory
	//	strcat(buff,"/");}
	//	strcat(buff,filelist[i]);}
	//filepath print
	int total=0;
	if(print_mode==2 || print_mode==3){ //if contain l
		//printfdirpath
		//fprintf(file, "Directory path: %s<br>",buff);//print dir path
        sprintf(buf_msg, "Directory path: %s<br>",buff);
        write(client_fd, buf_msg, strlen(buf_msg));
		struct stat tota;
		if(absol){ strcpy(buff, dir_path);} //ifabsol->copy
		for(int e=0 ; e<n;e++){
			if(print_mode==2&&filelist[e][0]=='.'){continue;} //printmode
			char plus_path[199];
			strcpy(plus_path,buff); strcat(plus_path,"/"); strcat(plus_path,filelist[e]);
			lstat(plus_path,&tota); total+= tota.st_blocks; //for store total st_block
		}	
		total = total/2; //division		
		//fprintf(file,"total :%d<br>\n",total); 
        sprintf(buf_msg, "total :%d<br>\n",total);
        write(client_fd, buf_msg, strlen(buf_msg));
    } //print totalsize
       

    //fprintf(file,"<table border='1'>\n"); //open table
    strcpy(buf_msg,"<table border='1'>\n");
    write(client_fd, buf_msg, strlen(buf_msg));
	if(lflag == 0){ //if lflag not set
		//fprintf(file,"<tr><th>Name</th></tr>\n"); //just name
        strcpy(buf_msg,"<tr><th>Name</th></tr>\n");
        write(client_fd, buf_msg, strlen(buf_msg));
	}
	else{ //or
	    //fprintf(file,"<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>\n");
        strcpy(buf_msg,"<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>\n");
        write(client_fd, buf_msg, strlen(buf_msg));
	} //set table focus on lflag


	char buff2[1024]; //store past path
	for( i=0; i<n;i++){ //loop n
		char buff1[1024];
		realpath(dir_path,buff1); //realpath
		//else if(dir_path[0]=='/'){ if(i==0){strcpy(buff2, dir_path); strcpy(buff1,buff2);} else{strcpy(buff1,buff2);}}
		//else{  getcwd(buff1,1024); } //if relative or absol or just directory
		 
        //-----------color select----------
		strcat(buff1,"/");
		strcat(buff1,filelist[i]); //make path
		char col_mode[19]={0,}; //color mode
		struct stat st_color;
		stat(buff1, &st_color); //st_mode
		if(S_ISDIR(st_color.st_mode)){ strcpy(col_mode,"Blue");} //if dir->blue
		else if (S_ISLNK(st_color.st_mode)){strcpy(col_mode,"Green");} //if link->green
		else{strcpy(col_mode,"Red");} //else ->red
		//-------------------------------------



		if(print_mode==0){ //if no option
			if(filelist[i][0]=='.'){continue;}//not print hidden
			//fprintf(file,"<tr><td style='color:%s'><a href='%s'>%s</a></td></tr>",col_mode,buff1, filelist[i]);
            sprintf(buf_msg, "<tr><td style='color:%s'><a href='%s'>%s</a></td></tr>",col_mode,buff1, filelist[i]);
            write(client_fd, buf_msg, strlen(buf_msg));
		} //print filename without hidden
		else if( print_mode==1){ //if -a
			//fprintf(file,"<tr><td style='color:%s'><a href='%s'>%s</a></td></tr>",col_mode,buff1,filelist[i]); //all print
            sprintf(buf_msg, "<tr><td style='color:%s'><a href='%s'>%s</a></td></tr>",col_mode,buff1,filelist[i]);
            write(client_fd, buf_msg, strlen(buf_msg));
		}
		else if( print_mode==2){ //if -l
			if(filelist[i][0]=='.'){continue;}//not printhidden
            strcpy(buff1,"./");
            strcat(buff1,filelist[i]);
			f_info(buff1,filelist[i], client_fd, url); //call f_info
		}
		else if(print_mode==3){ //if -al
			
			f_info(buff1,filelist[i], client_fd, url); //call f_ifno
		}
	}
    //fprintf(file,"</table>\n"); //endof table
    strcpy(buf_msg,"</table>\n");
    write(client_fd, buf_msg, strlen(buf_msg));
	for( i=0; i<n;i++){
		free(filelist[i]); //delete
	}
	free(filelist); //delete
	return ;
			

}

void f_info(char* filepath,char* filenme, int client_fd, char* url){
    char buf_msg[1024];
	struct stat filestat;
	if(lstat(filepath, &filestat)<0){ //get infor
	  printf("could not stat file: %s\n", filepath);
	  return;
	} //filepath
	
	//------------------select_color---------------
	char col_mode[19]={0,}; //colormode
	if(S_ISDIR(filestat.st_mode)){ strcpy(col_mode,"Blue");} //ifdirectory->blue
	else if (S_ISLNK(filestat.st_mode)){strcpy(col_mode,"Green");} //linkfile->green
	else{strcpy(col_mode,"Red");} //other->red
	//---------------------------------------------

	if(strcmp(filenme,".")==0){ //if filenameis.
		sprintf(buf_msg, "<tr style = 'color:%s'><td><a href='http://127.0.0.1:40000%s'> %s</a></td>", col_mode,url, filenme); //currentdir
	}
	else if(strcmp(filenme,"..")==0){ //filenameis..
		char* str_sub = strdup(url);
		char substr[100];
   		char *last_slash = strrchr(str_sub, '/'); //get parentdir
    	if (last_slash != NULL) {  // '/' exist
        	strncpy(substr, str_sub, last_slash - str_sub);  // '/' pre copy
        	substr[last_slash - str_sub] = '\0';  // plus null word
        	printf("%s\n", substr);  // wanna str
   		}
		sprintf(buf_msg, "<tr style = 'color:%s'><td><a href='http://127.0.0.1:40000%s'> %s</a></td>", col_mode,substr, filenme);
	}
	else if(strcmp(col_mode,"Blue")!=0){ //ifisdir
		sprintf(buf_msg, "<tr style = 'color:%s'><td><a href='%s/%s'> %s</a></td>", col_mode, url,filenme, filenme); 
	}
	else{ //ifisother file
		if(strcmp(url,"/")==0){sprintf(buf_msg, "<tr style = 'color:%s'><td><a href='http://127.0.0.1:40000/%s'> %s</a></td>", col_mode,filenme,filenme);} //root
		else{sprintf(buf_msg, "<tr style = 'color:%s'><td><a href='http://127.0.0.1:40000%s/%s'> %s</a></td>", col_mode,url,filenme,filenme);} //other
	}
	write(client_fd, buf_msg, strlen(buf_msg)); //write hyperrink
	
    sprintf(buf_msg, "<td>%c", S_ISDIR(filestat.st_mode) ?'d':'-');
    write(client_fd, buf_msg, strlen(buf_msg));
    sprintf(buf_msg, "%c", filestat.st_mode & S_IRUSR ? 'r' : '-');
    write(client_fd, buf_msg, strlen(buf_msg));
    sprintf(buf_msg, "%c", filestat.st_mode & S_IWUSR ? 'w' : '-');
    write(client_fd, buf_msg, strlen(buf_msg));
    sprintf(buf_msg, "%c", filestat.st_mode & S_IXUSR ? 'x' : '-');
    write(client_fd, buf_msg, strlen(buf_msg));
    sprintf(buf_msg, "%c", filestat.st_mode & S_IRGRP ? 'r' : '-');
    write(client_fd, buf_msg, strlen(buf_msg));
    sprintf(buf_msg, "%c", filestat.st_mode & S_IWGRP ? 'w' : '-');
    write(client_fd, buf_msg, strlen(buf_msg));
    sprintf(buf_msg, "%c", filestat.st_mode & S_IXGRP ? 'x' : '-');
    write(client_fd, buf_msg, strlen(buf_msg));
    sprintf(buf_msg, "%c", filestat.st_mode & S_IROTH ? 'r' : '-');
    write(client_fd, buf_msg, strlen(buf_msg));
    sprintf(buf_msg,"%c", filestat.st_mode & S_IWOTH ? 'w' : '-');
    write(client_fd, buf_msg, strlen(buf_msg));
    sprintf(buf_msg,"%c</td>", filestat.st_mode & S_IXOTH ? 'x' : '-');
    write(client_fd, buf_msg, strlen(buf_msg));
	//link num
	
    sprintf(buf_msg,"<td>%3ld</td>",filestat.st_nlink);
    write(client_fd, buf_msg, strlen(buf_msg));
	//user and group
	struct passwd* pw = getpwuid(filestat.st_uid);
	struct group* gr = getgrgid(filestat.st_gid);
    sprintf(buf_msg,"<td>%s</td>", pw ? pw->pw_name : "unkown");
    write(client_fd, buf_msg, strlen(buf_msg));
    sprintf(buf_msg,"<td>%s</td>", gr ? gr->gr_name : "unkown");
    write(client_fd, buf_msg, strlen(buf_msg));
	//file size
    strcpy(buf_msg,"<td>");
    write(client_fd, buf_msg, strlen(buf_msg));
	
    sprintf(buf_msg,"%6ld",filestat.st_size);
    write(client_fd, buf_msg, strlen(buf_msg));
	
    strcpy(buf_msg,"</td>");
    write(client_fd, buf_msg, strlen(buf_msg));
	//time
	char mod_time[32];
	strftime(mod_time, sizeof(mod_time), "%b %d %H: %M", localtime(&filestat.st_mtime));
	
    sprintf(buf_msg,"<td>%s</td></tr>", mod_time);
    write(client_fd, buf_msg, strlen(buf_msg));
	//file name
    return;
}

