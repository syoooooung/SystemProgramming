//////////////////////////////////////////////////////////////////
//File Name   : 2021202057_ipc_server.c         //
//Date      : 2023/05/23               //
//OS      : Ubuntu 16.04 LTS 64bits         //
//Author   : HWang Se Young            //
//Student ID   : 2021202057               //
//-------------------------------------------         //
//Title : System Programming Assignment #3-2         //
//Description : shared memory & thread use               //
//////////////////////////////////////////////////////////////////
//fi_info                     //
//==============================================================//
//Input: char* filepath -> filepath            //
//    char* filenme -> file name            //
//      int client_fd ->client file descript
//      char* url->url
//      char* ip_clit -> client ip          //
//Output: print ls -l element               //
//Purpose: print ls -l element               //
//////////////////////////////////////////////////////////////////
//ls_sort                     //
//==============================================================//
//Input: int print_mode -> -a or -l or -la or non?      //
//    char* dir_path -> directory path         //
//      int client_fd ->client file descript
//      char* url->url
//      char* ip_clit -> client ip          //
//Output: x                     //
//Purpose: sorting filename list            //
//////////////////////////////////////////////////////////////////
//print_deny
//==============================================================//
//Input: int client_id  -> client id
//       char* ip       ->couldn't accessible ip
//Output: print html
//Purpose : print access deniede
//////////////////////////////////////////////////////////////////
//sig_history
//==============================================================//
//Input: int sig   -> signal                    //
//Output: print client information history
//Purpose : print client informaiton history
/////////////////////////////////////////////////////////////////
//child_make
//==============================================================
//Input:int i -> pids number
//      int socket_fd ->socket_file descriptor
//      int addrlen     -> client_addr size
//Output : return process id
//Purpose : make child process(fork)
//////////////////////////////////////////////////////////////////
//child_main
//==============================================================
//Input:int i-> pids number
//      int socket_fd -> socket_file descriptor
//      int addrlen     -> client_addr size
//Output : client information
//Purpose : save / management client information
/////////////////////////////////////////////////////////////////
//child_history
//==============================================================
//Input:int sig -> signal
//Output: if(SIGUSR1) print client infor
//Purpose : handle child sinal funct
////////////////////////////////////////////////////////////////
// getConfi
//==============================================================
//Input : const char* key -> find word
//Output : information that process count
//Purpose : Get httpd.conf num
////////////////////////////////////////////////////////////////
// threadFunc1
//==============================================================
//Input / Output : x
//Purpose : plus idle process number
////////////////////////////////////////////////////////////////
// threadFunc2
//==============================================================
//Input / Output : x
//Purpose : minus idle process number
////////////////////////////////////////////////////////////////
// threadFunc3
//==============================================================
//Input : x 
//Output : client list index number
//Purpose : Get client list index number
////////////////////////////////////////////////////////////////
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
#include <sys/wait.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include <fnmatch.h>
#include <stdbool.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define URL_LEN 256
#define BUFSIZE 1024
#define PORTNO 40000
#define KEY_NUM 40000
#define MEM_SIZE 2048
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int lflag=0;  int maxChilds, maxIdleNum, minIdleNum, startProcess, maxHistory;
int socket_fd_main, addrlen_main, shm_id_main;

int getConfi(const char* key);


pthread_t* threads;


static int numofReq=0; //number of request
static pid_t *pids;

struct client_info{ //client informaitonsturcut
    char ip[30]; //client ip
    int port; //client portnum
    int pid; //process id
    time_t timestamp; //server-client connect time
};
struct client_info client_list[1024]={0,};


typedef struct{
    int connectChild;
    int idleChild;
    int c_list_index;
    struct client_info client_listn[1024];
}ShareData; //공유할 구조체
ShareData* shdata;

void f_info(char* filepath,char* filenme, int client_fd, char* url , char* ip_clit);
void ls_sort(int print_mode,char* dir_path, int client_fd, char* url, char* ip_clit);
void print_deny(int client_id, char* ip);
pid_t child_make(int i, int socket_fd, int addrlen, int shm_id,pthread_t* threads, int mainflag);
void child_main(int i, int socket_fd, int addrlen, int shm_id,pthread_t* threads, int mainflag);
void child_history(int sig);

void* threadFunc1(void* vptr);
void* threadFunc2(void* vptr);
void* threadFunc3(void* vptr);


void sig_history(int sig){

    if(sig==SIGINT){ //if signal is SIGINT
        printf("\n");


        for(int i=0; i<maxChilds;i++){ //5times
           if(pids[i]<=0){continue;}

            pthread_t tid_his;
            time_t t = time(NULL);  //get time
            char* time_str = ctime(&t); //change type to char
            time_str[strlen(time_str) - 1] = '\0'; //remove enter
            pthread_create(&tid_his, NULL,&threadFunc2,NULL );  //thread minus idle processcount

            pthread_join(tid_his, NULL);

            printf("[%s] %ld process is terminated.\n[%s] IdleProcesscount : %d\n",time_str,(long)pids[i],time_str,shdata->idleChild);

           

            kill(pids[i],SIGTERM); //send the signal to childprocess
           // waitpid(children[i], NULL, 0);
        }

    }
    else if(sig==SIGCHLD){ //if signal is SIGCHLD
        int status; pid_t c_pid;
        while((c_pid = waitpid(-1, &status, WNOHANG))>0){
          //  printf("%ld process is terminated.\n",(long)c_pid);
        }
    }
    else if(sig==SIGUSR1){ //idelcount 맞춰주기
        
        if(minIdleNum >shdata->idleChild){
            for(int i=startProcess; i<maxChilds ; i++){ // fork 해줄 경우
                if(pids[i] == 0){
                    pthread_t tid_his;
                    pthread_create(&tid_his, NULL,&threadFunc1,NULL );

                    pthread_join(tid_his, NULL);


                    pids[i]=child_make(i, socket_fd_main, addrlen_main, shm_id_main, &threads[i],0);

                    //idle 생성출력문은 fork가 되면 알아서 하니까 안해줘도돼
                }
                if(startProcess <= shdata->idleChild){return ;}
            }
        }
        else if(maxIdleNum < shdata->idleChild){ //무작위로 exit 해줌 idle 수 줄여줘야하는데.

        
            for(int i=startProcess; i<maxChilds ; i++){
                if(pids[i]!=0){
                    if(pids[i]==-1){continue;}
                    
                    pthread_t tid_his;
                    time_t t = time(NULL);  //get time
                    char* time_str = ctime(&t); //change type to char
                    time_str[strlen(time_str) - 1] = '\0'; //remove enter
                    pthread_create(&tid_his, NULL,&threadFunc2,NULL ); //thread_create

                    pthread_join(tid_his, NULL); //thread_wait

         
                    kill(pids[i],SIGTERM); //exit child process

                    waitpid(pids[i], NULL, 0);
                    printf("[%s] %ld process is terminated.\n[%s] IdleProcesscount : %d\n",time_str,(long)pids[i],time_str,shdata->idleChild);


                    pids[i]=0;
                }
                if(startProcess >= shdata->idleChild){return ;}
            }
        }
        
        return ;

    }
    else if(sig==SIGUSR2){ //아이들프로세스 변경됐을 때
        time_t t = time(NULL);  //get time
        char* time_str = ctime(&t); //change type to char
        time_str[strlen(time_str) - 1] = '\0'; //remove enter
        printf("[%s] IdleProcesscount : %d\n",time_str,shdata->idleChild);

        return ;
    }
    else {
        char buf_his[5999]={0,};
        printf("====================Connection History=====================\n");
       // printf("Number of request(s) : %d\n",numofReq);  //print numberof request
        printf("No.\tIP\t\tPID\tPORT\tTIME\n"); //row
        if(shdata->c_list_index<maxHistory){ //if numberof request less than 10
            int j=1;
            for(int i= shdata->c_list_index-1 ; i>-1; i--){
            //struct client_info tmp_info = client_list[i];
                if(shdata->client_listn[i].port == 0){ continue;} //get infor

                printf("%d\t",j);
                printf("%s\t", shdata->client_listn[i].ip); //ip
                printf("%d\t", shdata->client_listn[i].pid); //process id
                printf("%d\t", shdata->client_listn[i].port); //port num
                printf("%s", ctime(&(shdata->client_listn[i].timestamp))); //time 
                j++;
            } 
        }
         else{ //if more than 9
            int j=1;
            for(int i=shdata->c_list_index-1 ; j<=maxHistory ; i--){
                if(shdata->client_listn[i].port == 0){ continue;}
                printf("%d\t",j);
                printf("%s\t", shdata->client_listn[i].ip); //ip
                printf("%d\t", shdata->client_listn[i].pid); //process id
                printf("%d\t", shdata->client_listn[i].port); //port num
                printf("%s", ctime(&(shdata->client_listn[i].timestamp))); //time 
                j++;
            }
        }
        //printf("%s",buf_his); //print client infor
      //  printf("===========================================================\n");
    }
    alarm(10);
}

void child_history(int sig){
    char buf_his[2048]={0,};
    if(sig== SIGTERM){ //if signal is SIGTERM

       
        exit(0); //exit
    }
    else if(sig == SIGUSR1){ //if signal is SIGUSR1
        if(numofReq<10){ //if numberof request less than 10
            for(int i= numofReq-1 ; i>-1; i--){
            //struct client_info tmp_info = client_list[i];
                if(client_list[i].port == 0){break;} //get infor
                sprintf(buf_his,"%s%d\t",buf_his,i+1);
                sprintf(buf_his,"%s%s\t",buf_his, client_list[i].ip); //ip
                sprintf(buf_his,"%s%d\t",buf_his, client_list[i].pid); //process id
                sprintf(buf_his,"%s%d\t",buf_his, client_list[i].port); //port num
                sprintf(buf_his,"%s%s",buf_his, ctime(&(client_list[i].timestamp))); //time 
            } 
        }
         else{ //if more than 9
            int j=10;
            for(int i=numofReq-1 ; i>numofReq-11 ; i--){
                if(client_list[i].port == 0){break;}
                sprintf(buf_his,"%s%d\t",buf_his,i+1);
                sprintf(buf_his,"%s%s\t",buf_his, client_list[i].ip); //ip
                sprintf(buf_his,"%s%d\t",buf_his, client_list[i].pid); //process id
                sprintf(buf_his,"%s%d\t",buf_his, client_list[i].port); //port num
                sprintf(buf_his,"%s%s",buf_his, ctime(&(client_list[i].timestamp))); //time 
            }
        }
        printf("%s",buf_his); //print client infor
    }
}


pid_t child_make(int i, int socketfd, int arrlen, int shm_id, pthread_t* threads, int mainflag){ //child make
    for(int j=i; j<maxChilds; j++) { pids[j]=0;}
    time_t t = time(NULL); 
   // client_addr=(struct sockaddr *)malloc(addrlen);
    char* time_str = ctime(&t);
    time_str[strlen(time_str) - 1] = '\0';
    if(mainflag){
        pthread_create(threads, NULL,&threadFunc1,NULL );

        pthread_join(*threads, NULL);
    }
    pid_t pid;
    if((pid=fork())>0){
        printf("[%s] %d process is forked.\n[%s]IdleProcesscount : %d\n",time_str, pid, time_str, shdata->idleChild);
        return (pid);
    } //fork and if parent=>return pid
    child_main(i, socketfd, arrlen, shm_id, threads, mainflag); //call child_main funct
    return 0;
}

void child_main(int i, int socket_fd, int addrlen, int shm_id, pthread_t* threads, int mainflag){ 
    signal(SIGINT, SIG_IGN); //SIGINT is ingnored
    signal(SIGUSR1, child_history);
    signal(SIGTERM, child_history);

    //==============자식 프로세스에서 공유메모리 첨부=========
    pthread_t tid;
   
    shdata = (ShareData*)shmat(shm_id, NULL, 0);
    //======================================================

    struct sockaddr_in client_addr;
    int len,aflag = 0,print_mode = 0;
    int client_fd, len_out;
    char buf[BUFSIZE];
    char dir_path[199];
    socklen_t clilen;
   // struct sockaddr* client_addr;

 //   pthread_mutex_init(&mutex, NULL); //뮤텍스초기화

     time_t t = time(NULL); 
   // client_addr=(struct sockaddr *)malloc(addrlen);
    char* time_str = ctime(&t);
    time_str[strlen(time_str) - 1] = '\0';
    
    

/*    for (int j = 0; j < startProcess; j++) {
        pthread_join(threads[j], NULL);
    }
*/

   

    //kill(getppid(), SIGUSR2);


    if(shdata->idleChild > maxIdleNum){ //max보다 커지면 terminate
        
        kill(getppid(), SIGUSR1); //프로세스종료해주는시그널

        kill(getppid(), SIGUSR2); //현재idle 수 출력

        //exit(0);

    }
 
    while(1){
        
        struct in_addr inet_client_address;
        char buf[BUFSIZE] = { 0, };
        char tmp[BUFSIZE] = { 0, };
        char response_header[BUFSIZE] = { 0, }; //header buf
        char response_message[BUFSIZE] = { 0, }; //messgae buf
        char url[BUFSIZE] = { 0, };
        char method[20] = { 0, };
        char* tok = NULL;
        //printf("iam lating here!");
        len = sizeof(client_addr);
        client_fd = accept(socket_fd, (struct sockaddr*)&client_addr, &len);//accept incoming ceonnections
        if (client_fd < 0)
        {
            printf("server fail");
            return ;
        }
   //     close(socket_fd);
        inet_client_address.s_addr = client_addr.sin_addr.s_addr; //set client adder
        //printf("[%s : %d] client was connected\n", inet_ntoa(inet_client_address), client_addr.sin_port);

        //===============스레드에서 index값가져오기=========
        int* thread_result;
        pthread_create(threads, NULL,&threadFunc3,NULL );

        pthread_join(*threads, (void**)&thread_result);
        int clnt_idx = *thread_result;
        free(thread_result);

        //=============================================


        

        pid_t pid;
        
        t = time(NULL); //set time

        read(client_fd, buf, BUFSIZE); //read request message from client
        strcpy(tmp, buf); //copy


        tok = strtok(tmp, " ");
        if (tok == NULL) {
            continue; //jump
        }
        strcpy(method, tok);
        if (strcmp(method, "GET") == 0) //get url
        {
            tok = strtok(NULL, " ");
            strcpy(url, tok);
        }

        char* host = strstr(buf, "Host: "); // search for "Host: " in the buffer
        if (host != NULL) {
            // move the pointer to the start of the host address
            host += strlen("Host: ");
            // get the host address by copying until the next '\r' character
            char* end = strchr(host, '\r');
            char host_address[end - host + 1];
            strncpy(host_address, host, end - host);
            host_address[end - host] = '\0';
            strcpy(host, host_address); //get host ip
        }

        FILE* fp; char line[100]; //read buf
        int ret = 0;
        fp = fopen("accessible.usr", "r"); //open accessible.usr
        char* server_ip_str = inet_ntoa(inet_client_address); //change type
       // printf("Server IP addressmymymymy: %s\n", server_ip_str);
        char tmp_hst[1024];

        unsigned int ip_addr = inet_client_address.s_addr; //address set
        char ip_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ip_addr, ip_str, INET_ADDRSTRLEN); //change type

        while (fgets(line, 100, fp)) {
            // remove trailing newline character
            strtok(line, "\n"); //remove enter
            strcpy(tmp_hst, host);
            strtok(tmp_hst, ":");

            if (fnmatch(line, server_ip_str, FNM_NOESCAPE) == 0) { //is it accessible
                ret = 1; // match found
                break;
            }
        }
        fclose(fp);
        if (ret == 0) {
            //printf("not match!! %s\n\n", tmp_hst);
            print_deny(client_fd, ip_str); //print acccessdeny
            continue;
        } 

        char ip_clit[1024];
        strcpy(ip_clit, tmp_hst);

         //fork and child process
            
            time_str = ctime(&t);
            time_str[strlen(time_str) - 1] = '\0';
            puts("==============New Client=============="); //print new clientinfo
            printf("[%s]\n", time_str); //time 
            printf("IP : %s\n", ip_clit);
            printf("Port : %d\n", client_addr.sin_port);
            //puts(buf); //print inform
            puts("=======================================");

            //---------client struct store-----
            shdata->client_listn[clnt_idx].port = client_addr.sin_port;
            strcpy(shdata->client_listn[clnt_idx].ip, ip_clit);
            shdata->client_listn[clnt_idx].pid = getpid();
            shdata->client_listn[clnt_idx].timestamp = t;
            //-----------------------------------
                    
            //===============idleprocesscount 줄이기===========
            pthread_create(threads, NULL,&threadFunc2,NULL );

            pthread_join(*threads, NULL);

            kill(getppid(), SIGUSR2); //현재 아이들카운트수 출력
           
            sleep(1);
            if(shdata->idleChild < minIdleNum){ //minidlenum>idleprocess 일 경우엔 fork하여 수를 늘려줌
                kill(getppid(), SIGUSR1);
  
               //pthread_create(threads, NULL,&threadFunc1,NULL );

               // pthread_join(*threads, NULL);

                

                //kill(getppid(), SIGUSR2);
            }

            //==============================================
        
            
        


            //---------------------state----------------
            struct stat is_Dir;
            char real_url[1024];
            char dot_pt[199] = ".";
            strcat(dot_pt, url);
            realpath(dot_pt, real_url); //get realpath
            int is_ex = lstat(real_url, &is_Dir); //is exist
            int state = 0; //not eixt404 dir 1 justfile 2

            if (is_ex < 0) { //404
                state = 0;
            }
            else {
                if (S_ISDIR(is_Dir.st_mode)) { state = 1; } //dir->state=1
                else { state = 2; } //other file->state=2
            }
            //-------------------------------------------

            if (state == 0) { //if not exist directory
                sprintf(response_message, "<link rel='icon' href='data:,'><h1>Not Found</h1><br> <h3>The request URL %s was not found on this server<br>HTTP 404 - Not Page Found</h3>", url); //write error print
                sprintf(response_header, "HTTP/1.1 404 OK\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n", (int)strlen(response_message));
                write(client_fd, response_header, strlen(response_header)); //write header
                write(client_fd, response_message, strlen(response_message)); //write message

            }
            else if (state == 1) { //directory
                if (strcmp(url, "/") == 0) { //root
                    aflag = 0; lflag = 1; print_mode = 2; strcpy(dir_path, "."); //set flag
                    sprintf(response_header, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n", 100000);
                    write(client_fd, response_header, strlen(response_header)); //write header
                    sprintf(response_message, "<link rel='icon' href='data:,'><h1>Welcome to System Programming Http</h1><br>");
                    write(client_fd, response_message, strlen(response_message)); //write message
                    ls_sort(print_mode, dir_path, client_fd, url, ip_clit); //funct call
                }
                else { //child directory
                    aflag = 1; lflag = 1; print_mode = 3; //set flag
                    sprintf(response_header, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n", 100000);
                    write(client_fd, response_header, strlen(response_header)); //write header
                    sprintf(response_message, "<link rel='icon' href='data:,'><h1>System Programming Http</h1><br>");
                    write(client_fd, response_message, strlen(response_message)); //write message
                    strcpy(dir_path, "."); //set path
                    strcat(dir_path, url); //relative path
                    ls_sort(print_mode, dir_path, client_fd, url, ip_clit);
                    //function call
                }

            }
            else if (state == 2) { //file
                if (fnmatch("*.jpg", url, FNM_CASEFOLD) == 0 || fnmatch("*.png", url, FNM_CASEFOLD) == 0 || fnmatch("*.jpeg", url, FNM_CASEFOLD) == 0)
                { //if image file
                    char url_tmp[1024];

                    sprintf(url_tmp, ".%s", url); //set path


                    FILE* file = fopen(url_tmp, "rb"); //read binary
                    if (file == NULL) { printf("file open error\n"); }
                    fseek(file, 0, SEEK_END); //fiel seek
                    long int file_size = ftell(file); //filesize return
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
                else if (fnmatch("*.html", url, FNM_CASEFOLD) == 0)
                {
                    char url_tmp[1024];
                    sprintf(url_tmp, ".%s", url); //set path
                    FILE* file = fopen(url_tmp, "rb"); //read binary
                    if (file == NULL) { printf("file open error\n"); }
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
                    sprintf(url_tmp, ".%s", url); //set path
                    FILE* file = fopen(url_tmp, "rb"); //read binary
                    if (file == NULL) { printf("file open error\n"); } //filenot open
                    fseek(file, 0, SEEK_END);
                    long int file_size = ftell(file); //filesize return
                    fseek(file, 0, SEEK_SET); //set start
                    char* buffer = malloc(file_size);
                    fread(buffer, file_size, 1, file); //file read
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

            sleep(8);

            t = time(NULL);
            time_str = ctime(&t);
            time_str[strlen(time_str) - 1] = '\0';
            puts("=========Disconnected Client==========="); //print disconnect client info
            printf("[%s]\n", time_str); //time 
            printf("IP : %s\n", ip_clit);
            printf("Port : %d\n", client_addr.sin_port);
            puts("=======================================");

            pthread_create(threads, NULL,&threadFunc1,NULL );

            pthread_join(*threads, NULL);

            kill(getppid(), SIGUSR2);

            if(shdata->idleChild > maxIdleNum){ //max보다 더 크면 idle줄여준다
                kill(getppid(), SIGUSR1);

             //   kill(getppid(), SIGUSR2);
                
            }


            close(client_fd);
           // exit(0);


       
    }
}
int main(int argc, char* argv[]) {
    int print_mode = 0, aflag = 0;
    char dir_path[199];
    int c = 0;
    opterr = 0;
   
    for(int i=0; i<maxChilds ; i++){
        pids[i]=0;
    }

    //httpd.conf 파일 읽어오기
     maxChilds = getConfi("MaxChilds");
     maxIdleNum = getConfi( "MaxIdleNum");
    minIdleNum = getConfi("MinIdleNum");
     startProcess = getConfi("StartProcess");
     maxHistory = getConfi("MaxHistory");
    
  //  printf("MaxChilds: %d\n", maxChilds);
  //  printf("MaxIdleNum: %d\n", maxIdleNum);
   // printf("MindIdleNum: %d\n", minIdleNum);
   // printf("StartProcess: %d\n", startProcess);
 //   printf("MaxHistory: %d\n", maxHistory);
    //-----------------------
    threads = malloc(sizeof(pthread_t) * maxChilds);  //스레드배열
   
    //pthread_mutex_init(&mutex, NULL); //뮤텍스초기화

    //====================공유메모리 생성 및 연결================
    void *shm_addr;
    
    if((shm_id_main = shmget((key_t)KEY_NUM, MEM_SIZE, IPC_CREAT|0666)) == -1){
        printf("shmget fail\n"); return 0;
    }
    /*
    if(shm_addr = shmat(shm_id,(void*)0, 0) == (void*)-1){
        printf("shmat fail\n"); return 0;
    }*/
    shdata = (ShareData*)shmat(shm_id_main, NULL, 0);
    if(shdata == (ShareData*)(-1)){
        printf("shmat fail in main\n");
        return 0;
    }
    shdata->connectChild = 0; //초기화해주기
    shdata->idleChild = 0;
    shdata->c_list_index=0;

    //==========================================================

    struct sockaddr_in server_addr, client_addr; //define strcut
    int  client_fd;
    int len, len_out;
    int opt = 1;

    signal(SIGALRM, sig_history); //set signal
    signal(SIGCHLD, sig_history); 
    signal(SIGINT, sig_history);
    signal(SIGUSR1, sig_history);
    signal(SIGUSR2, sig_history);

    time_t t = time(NULL); //set time
    char* time_str = ctime(&t);
    time_str[strlen(time_str) - 1] = '\0'; // remove the newline character at the end of the string
    printf("[%s] Server is started.\n", time_str);
   
    if ((socket_fd_main = socket(PF_INET, SOCK_STREAM, 0)) < 0) //create a socketfile
    {
        printf("Server : can't open stream socket\n"); //error code
        return 0;
    }

    setsockopt(socket_fd_main, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));//set optionof the socket
    alarm(10); //10sec
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //initialisze the server adress
    //server_addr.sin_addr.s_addr = htonl(INADDR_ANY);//setting
    server_addr.sin_port = htons(PORTNO); //port number is 40000

    if (bind(socket_fd_main, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0)
    { //bind the socket file descirptor to hte server address
        printf("Serve err\n");
        return 0;
    }

    listen(socket_fd_main, 5); //listen



    int maxNchildren=5;
    pids = (pid_t *)malloc(maxChilds * sizeof(pid_t));
    addrlen_main =sizeof(client_addr);

   
    for(int i=0; i<startProcess ; i++){ //5 times
        pid_t tmp = child_make(i, socket_fd_main, addrlen_main, shm_id_main, &threads[i],1); //call fork funct
        if(tmp==0 ){break;}
        pids[i] = tmp; //save pid
    }

   // pause();
    
    for(int i=0; i<maxChilds ; i++){ //5times
        if(pids[i]==0 ){continue;}
        waitpid(pids[i], NULL, 0); //wait all child process
        close(pids[i]);
    }

    shmctl(shm_id_main, IPC_RMID, 0);
    pthread_mutex_destroy(&mutex);
         t = time(NULL); //get time 
         time_str = ctime(&t); //change type to char
        time_str[strlen(time_str) - 1] = '\0'; //remove enter
        printf("[%s] %ld Server is terminated.\n",time_str, (long) getpid()); //print
        exit(0);
//==========================================================================
   
    close(socket_fd_main);

    return 0;
}
void ls_sort(int print_mode,char* dir_path, int client_fd, char* url ,char* ip_clit){ //file name list sort
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
   //   strcat(buff,"/");}
   //   strcat(buff,filelist[i]);}
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
         f_info(buff1,filelist[i], client_fd, url, ip_clit); //call f_info
      }
      else if(print_mode==3){ //if -al
         
         f_info(buff1,filelist[i], client_fd, url, ip_clit); //call f_ifno
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

void f_info(char* filepath,char* filenme, int client_fd, char* url, char* ip_clit){
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
      sprintf(buf_msg, "<tr style = 'color:%s'><td><a href='http://%s:40000%s'> %s</a></td>", col_mode,ip_clit,url, filenme); //currentdir
   }
   else if(strcmp(filenme,"..")==0){ //filenameis..
      char* str_sub = strdup(url);
      char substr[100];
         char *last_slash = strrchr(str_sub, '/'); //get parentdir
       if (last_slash != NULL) {  // '/' exist
           strncpy(substr, str_sub, last_slash - str_sub);  // '/' pre copy
           substr[last_slash - str_sub] = '\0';  // plus null word
           //printf("%s\n", substr);  // wanna str
         }
      sprintf(buf_msg, "<tr style = 'color:%s'><td><a href='http://%s:40000%s'> %s</a></td>", col_mode,ip_clit,substr, filenme);
   }
   else if(strcmp(col_mode,"Blue")!=0){ //ifisdir
        if(strcmp(url,"/")==0){
            sprintf(buf_msg, "<tr style = 'color:%s'><td><a href='http://%s:40000/%s'> %s</a></td>", col_mode,ip_clit,filenme, filenme); 
        }
        else{
            sprintf(buf_msg, "<tr style = 'color:%s'><td><a href='%s/%s'> %s</a></td>", col_mode, url,filenme, filenme); 
        }
   }
   else{ //ifisother file
      if(strcmp(url,"/")==0){sprintf(buf_msg, "<tr style = 'color:%s'><td><a href='http://%s:40000/%s'> %s</a></td>", col_mode,ip_clit,filenme,filenme);} //root
      else{sprintf(buf_msg, "<tr style = 'color:%s'><td><a href='http://%s:40000%s/%s'> %s</a></td>", col_mode,ip_clit,url,filenme,filenme);} //other
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

void print_deny(int client_fd, char* ip){
    char response_header[BUFSIZE] = { 0, }; //header buf
    char response_message[BUFSIZE] = { 0, }; //messgae buf
    sprintf(response_message, "<link rel='icon' href='data:,'><h1>Access denied!</h1><br> <h3>Your IP : %s <br>You have no permission to access this web server<br>HTTP 4043.6 - Forbidden: IP address reject</h3>", ip); //write error print
    sprintf(response_header, "HTTP/1.1 404 OK\r\nContent-Type: text/html\r\nContent-Length:%d\r\n\r\n", (int)strlen(response_message));
    write(client_fd, response_header, strlen(response_header)); //write header
    write(client_fd, response_message, strlen(response_message)); //write message
}

int getConfi(const char* key){
    FILE* file = fopen("httpd.conf", "r"); //open file
    if (file == NULL) { //if error
        perror("fopen");
        exit(1);
    }
    
    char line[BUFSIZE];
    int value = 0;
    while (fgets(line, sizeof(line), file) != NULL) { //read line
        if (strstr(line, key) != NULL) { //if exist key
            sscanf(line, "%*[^:]:%d", &value); //read decimal
            break;
        }
    }
    
    fclose(file);
    return value;
}

void* threadFunc1(void* vptr){ //idlechild가 늘리기
   
    pthread_mutex_lock(&mutex);

  //  printf("%d curr idel \n", shdata->idleChild);
    shdata->idleChild += 1 ;
    
    pthread_mutex_unlock(&mutex);

  //  free(arg);
    return NULL;

}

void* threadFunc2(void* vptr){ //idlechild 줄이기
     pthread_mutex_lock(&mutex);

  //  printf("%d curr idel \n", shdata->idleChild);
   if(shdata->idleChild != 0){
        shdata->idleChild -= 1 ;
   }
    
    pthread_mutex_unlock(&mutex);

 //   free(arg);
    return NULL;
}
void* threadFunc3(void* vptr){  //클라이언트정보저장해줄 인덱스반환해주는 함수

    pthread_mutex_lock(&mutex);

    int return_idx= shdata->c_list_index;

    shdata->c_list_index += 1;
    int* ptr = malloc(sizeof(int)); // int 값을 저장하기 위한 동적 할당
    *ptr = return_idx;

    pthread_mutex_unlock(&mutex);

    return (void*)ptr;
    
}
