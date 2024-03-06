/////////////////////////////////////////////////////////////////
//File Name	: 2021202057_final_ls.c				//
//Date		: 2023/04/10					//
//OS		: Ubuntu 16.04 LTS 64bits			//
//Author	: HWang Se Young				//
//Student ID	: 2021202057					//
//-------------------------------------------			//
//Title : System Programming Assignment #1-3			//
//Description : final_ls					//
//////////////////////////////////////////////////////////////////
//fi_info							//
//==============================================================//
//Input: char* filepath -> filepath				//
//	 char* filenme -> file name				//
//Output: print ls -l element					//
//Purpose: print ls -l element					//
//////////////////////////////////////////////////////////////////
//ls_sort							//
//==============================================================//
//Input: int print_mode -> -a or -l or -la or non?		//
//	 char* dir_path -> directory path			//
//Output: x							//
//Purpose: sorting filename list				//
//////////////////////////////////////////////////////////////////
//Is_wild							//	
//==============================================================//
//Input: char* dir_path -> directory path			//
//	 int index	-> argv location			//
//Output: x							//
//Purpose: whether wildcard is presetn in argv			//
//////////////////////////////////////////////////////////////////
//wild_print							//
//==============================================================//
//Input: char* dir_path ->directory path			//
//	 int arc	->argc					//
//	 char* arv	-> argv					//
//	 int i		-> argv index				//
//Output: x							//
//Purpose: get directory and file list				//
//////////////////////////////////////////////////////////////////
//wild_sort							//
//==============================================================//
//Input: x							//
//Output: print file list and directory list			//
//Purpose: file list & directory list sort and print		//
//////////////////////////////////////////////////////////////////




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
#include <math.h>
#include <fnmatch.h>
#include <glob.h>

int hflag=0, Sflag=0, rflag=0, lflag=0;
int Is_wild_card[199]={0,};
char **file_list=NULL; int list_idx=0;
char **dir_list=NULL; int dir_idx=0;


FILE *file; 



void f_info(char* filepath,char* filenme);
void ls_sort(int print_mode,char* dir_path);
void wild_sort();
int Is_wild(char* dir_path,int index){
	Is_wild_card[0]=-1;
	char *card ="*?["; //wildcard pattern
	char *Is_card=strpbrk(dir_path, card); //check pattern
	if(Is_card!=NULL){ Is_wild_card[index]=1; index++;  return 1;}//if wildcard existence
	else {Is_wild_card[index]=0; index++; return 0;} //no wildcard
	return 1;
}
void wild_sort(){
	for(int i=0; i<list_idx-1;i++){ //file_list sort
		int j=i, min=i;
		for( j=j+1; j<list_idx; j++){
			char* a= file_list[min], *b=file_list[j];
			int k; //two file compare
			for(k=0; a[k]&&b[k] ; k++){ //booth not end loop
				int ca= toupper(a[k]), cb=toupper(b[k]);//big alphabet
				if(ca != cb){ //if not equal
					if(ca< cb) break;//pass
					else {min=j; break;} //change
				}
			}
			if( !b[k] &&a[k]){min=j;} //mini is j
		}
		if( min !=i){ //if not equal mini and i
			char* tmp= file_list[i]; //i and mini swap
			file_list[i]= file_list[min];
			file_list[min] =tmp;
		}

	}

	for(int i=0 ; i<dir_idx-1; i++){ //dir_list sort
		int j=i, min=i;
		for( j=j+1; j<dir_idx; j++){ //dir_listindexloop
			char* a= dir_list[min], *b =dir_list[j];
			int k; //two file compare
			for(k=0; a[k]&&b[k] ;k++){//both not end loop
				int ca= toupper(a[k]), cb= toupper(b[k]);//big alphabet
				if(ca != cb){ //if not equal
					if(ca< cb) break; //pass
					else{min=j; break;} //change
				}
			}
			if( !b[k] && a[k]) {min=j;}//mini is j
		}
		if( min != i){ //if not equal mini andi
			char* tmp= dir_list[i]; //i and mini swap
			dir_list[i]= dir_list[min];
			dir_list[min]=tmp;
		}
	}
 
	if(list_idx!=0){
    	fprintf(file,"<table border='1'>\n"); fprintf(file,"<tr><th>Name</th><tr>\n");
		for(int i=0 ;i< list_idx;i++){ //printsort_filelist
			if(file_list[i][0]=='.'){continue;} //hiddenfile pass
			char tmp_imni[1024];
			char whatiam[1024]; strcpy(whatiam, file_list[i]); //filelist copy
			char* am = strstr(whatiam,"html_ls.html"); //is there html?
			if(am != NULL){continue;} //if true , continue

			if(file_list[i][0]=='.' || file_list[i][0]=='~' || file_list[i][0]=='/'){ //if startwithpath
				if(file_list[i][0]=='~'){ 
					char*home_d=getenv("HOME"); //~ is homeand user
					char*modi=malloc(strlen(home_d)+strlen(whatiam)+1); //allocate
					sprintf(modi, "%s%s", home_d, whatiam+1); //connect
					strcpy(tmp_imni,modi); //creat path
					strcat(tmp_imni,"/"); strcat(tmp_imni, file_list[i]);
				}
				else if(file_list[i][0]=='.'){ //relativepath->realpath
					realpath(whatiam, tmp_imni); strcat(tmp_imni,"/"); strcat(tmp_imni, file_list[i]);
				}
				else{strcpy( tmp_imni,file_list[i]); } //just copy
			}
			else{  getcwd(tmp_imni,1024); strcat(tmp_imni,"/"); strcat(tmp_imni, file_list[i]); }
			//strcat(tmp_imni,"/"); strcat(tmp_imni, file_list[i]);
    //------------------color----
        	char col_mode[19]={0,};
			struct stat st_color;
			lstat(tmp_imni, &st_color); //stat
			if(S_ISDIR(st_color.st_mode)){ strcpy(col_mode,"Blue");} //if directory->blue
			else if (S_ISLNK(st_color.st_mode)){strcpy(col_mode,"Green");} //if link->green
			else{strcpy(col_mode,"Red");} //if other->red
    //-------------------------
			//table setting
			fprintf(file,"<tr style='color:%s'><td ><a href='%s'>%s</a></td></tr>\n",col_mode,tmp_imni,file_list[i]);
		} 
    	fprintf(file,"</table>\n"); //end of table
	}
	fprintf(file,"<br>"); //enter
	
	for(int i=0; i<dir_idx;i++){
        fprintf(file,"Directory path: %s<br>\n",dir_list[i]);//directorypath
        fprintf(file,"<table border='1'>\n"); // setting table
        fprintf(file,"<tr><th>Name</th></tr>\n"); //setting table
		DIR *dir;
		char pppp[1024];
		strcpy(pppp,dir_list[i]); //copy
		if((dir=opendir(pppp))==NULL){printf("\n"); return;} //opendir
		struct dirent *entry;
		char **pt_list=NULL;//newdirectory elements
		int n=0;
		while((entry = readdir(dir)) !=NULL){ //not exist element loop
			if(strcmp(entry->d_name,"html_ls.html")==0){continue;}
			pt_list= (char**)realloc(pt_list,sizeof(char*)*(n+1)); //alloc
			pt_list[n++]=strdup(entry->d_name);//put dirname	
		}
		closedir(dir); //close dir 
		for(int I=0; I<n-1; I++){ //dir elements sort
			int j=I, min= I;
			for(int j=I+1; j<n;j++){
				char*a=pt_list[min], *b=pt_list[j];
				int k; //mini and j compare
				for(k=0; a[k]&&b[k]; k++){ //both not null
				  int ca=toupper(a[k]), cb=toupper(b[k]); //cmp big alpha
				  if(ca != cb){ //if not equal
				  	if(ca< cb) break; //pass
					else{min=j; break;} //mini=j
				  }
				}
				if( !b[k] &&a[k]){min=j;} //mini=j
			}
			if(min != I){ //if not equal min andi
				char* tmp= pt_list[I]; //mini and i swap
				pt_list[I]=pt_list[min];
				pt_list[min]=tmp;
			}
		}
        
		char whatiam[1024];
		strcpy(whatiam, dir_list[i]);
		for(int j=0; j<n;j++){ //print directory element
			if(pt_list[j][0]=='.'){continue;} //hiddend pass
			char tmp_imni[1024];
			if(dir_list[i][0]=='.' || dir_list[i][0]=='~' || dir_list[i][0]=='/'){ //if startwithpath
				if(dir_list[i][0]=='~'){ 
					char*home_d=getenv("HOME"); //if home path
					char*modi=malloc(strlen(home_d)+strlen(whatiam)+1);
					sprintf(modi, "%s%s", home_d, whatiam+1); //connect
					strcpy(tmp_imni,modi); //just copy
				}
				else if(dir_list[i][0]=='.'){ //if relative path
					realpath(whatiam, tmp_imni); //change absolpath
				}
				else{strcpy( tmp_imni,whatiam); } //just copy
			}
			else{  getcwd(tmp_imni,1024); } //get current working directory
			strcat(tmp_imni,"/"); strcat(tmp_imni, pt_list[j]); //connect

            char col_mode[19]={0,};
		    struct stat st_color; //stat
		    lstat(tmp_imni, &st_color); //get mode
		    if(S_ISDIR(st_color.st_mode)){ strcpy(col_mode,"Blue");} //if directory->blue
		    else if (S_ISLNK(st_color.st_mode)){strcpy(col_mode,"Green");} //if linkfile->green
		    else{strcpy(col_mode,"Red");} //if other-> red
			
			fprintf(file,"<tr style='color:%s' ><td ><a href='%s'>%s</a></td></tr>\n",col_mode,tmp_imni,pt_list[j]);//print element
		} 
        fprintf(file,"</table><br>"); //end of table
		printf("\n");
		
		for(int j=0; j<n; j++){ //n loop
			free(pt_list[j]); //memory free
		} free(pt_list); 
		
	}
    

	for( int i=0; i<list_idx; i++){
		free(file_list[i]); //file_lsit memroy free
	}
	free(file_list);
	for( int i=0; i< dir_idx; i++){ //directory memory free
		free(dir_list[i]);
	}
	free(dir_list);
	return ; //return
}
void wild_print(char* dir_path,int arc, char* arv,int i){
		if(Is_wild_card[i+1]==0){ //if doesnt have wildcard
			char arv1[1024];
            int just_name=0;
			if(arv[0]=='.' || arv[0]=='~' ||arv[0]=='/'){ //if startwithpath
				if(arv[0]=='~'){ 
					char*home_d=getenv("HOME"); //if startwith homepath
					char*modi=malloc(strlen(home_d)+strlen(arv)+1);
					sprintf(modi, "%s%s", home_d, arv+1); //connect
					strcpy(arv1,modi); //justcopy
				}
				else if(arv[0]=='.'){ //if relative
					realpath(arv, arv1); //change ablsolapath
				}
				else{strcpy( arv1, arv);}
			}
			else{ getcwd(arv1,1024); strcat(arv1,"/"); strcat(arv1, arv); just_name=1;}
			dir_path= arv1; // dirpath = arv
			DIR* drip=opendir(dir_path); //opendir
			struct stat f_s;
			if(lstat(dir_path, &f_s)<0){printf("errororororr\n"); return ;} //error
			if(!S_ISDIR(f_s.st_mode)){ //if not directory
				file_list=(char**)realloc(file_list,sizeof(char *)*(list_idx+1));
                if(just_name){ file_list[list_idx++]= arv;} //just filename
                else{file_list[list_idx++]=strdup(dir_path);}//put filelist
			}
			else{
				dir_list=(char**)realloc(dir_list,sizeof(char *)*(dir_idx+1));//memoryalloc
                if(just_name){file_list[list_idx++]= arv;} //just filename
				else {dir_list[dir_idx++]=strdup(dir_path);}//put dirlist
			}
			closedir(drip); //closedir
		}
		else if(Is_wild_card[i+1]==1){ //if have wildcard
			if(arv[0]=='.' || arv[0]=='~' || arv[0]=='/'){//if startwith path
				glob_t result; //globt
				char arv1[1024];
				if(arv[0]=='~'){ //if relativepath
				char* home_dir = getenv("HOME");//set homepath
				char* modi_str= malloc(strlen(home_dir)+ strlen(arv)+1);
				sprintf(modi_str, "%s%s", home_dir, arv+1); //connecthome&path
				strcpy(arv1, modi_str);} //copy
				else{ strcpy(arv1,arv);} //else just copy
				int ret = glob(arv1,0,NULL, &result); //use glob

				if(ret ==0 ){//if find
					for(int v=0; v<result.gl_pathc; v++){//element loop
						char bufglob[1024];
						if(arv[0]=='.' || arv[0]=='~'){realpath(result.gl_pathv[v],bufglob);}//if start relative
						if(arv[0]=='/'){strcpy(bufglob, result.gl_pathv[v]);}//abol
						struct stat f_s;
						if(lstat(bufglob, &f_s)<0){printf("erorimnida\n");return ;} //error
						if(!S_ISDIR(f_s.st_mode)){//if not directory
							file_list=(char**)realloc(file_list,sizeof(char *)*(list_idx+1)); //memoryalloc
							file_list[list_idx++]=strdup(bufglob);//put file
						}
						else{ //if directory
							dir_list=(char**)realloc(dir_list,sizeof(char *)*(dir_idx+1));
							dir_list[dir_idx++]=strdup(bufglob);//put dirlist	
                            	
						}
					} globfree(&result);//glob free
				}
			}
			else{ //not startwith path
				glob_t result; //glob result
				int ret = glob(arv,0,NULL, &result);//find pattern
				if(ret ==0){ //if find
					for(int v=0; v< result.gl_pathc; v++){//element loop
						char bufg[1024]; getcwd(bufg,1024); //get currentworkingpath
						strcat(bufg,"/"); strcat(bufg,result.gl_pathv[v]);//connect
						struct stat f_s;
						if(lstat(bufg, &f_s)<0){printf("eror\n");}
						if(!S_ISDIR(f_s.st_mode)){ //if is file
							file_list=(char**)realloc(file_list,sizeof(char *)*(list_idx+1));
							file_list[list_idx++]=strdup(result.gl_pathv[v]);//put filelist
						}
						else{ //if is directory
							dir_list=(char**)realloc(dir_list,sizeof(char *)*(dir_idx+1));
							dir_list[dir_idx++]=strdup(result.gl_pathv[v]);//put dirlsit
                            
						}
					}
				} globfree(&result); //memryfree
			}
		}

//	for(int i=0; i<list_idx ;i++){
//		printf("%s  ",file_list[i]);
//	}
//   printf("=======================\n");
//	for(int j=0; j<dir_idx ; j++){
//		printf("%s  ",dir_list[j]);
//	}
	
}
int main(int argc, char* argv[]){
	int print_mode = 0, aflag=0; //each flag
	char* dir_path =".";
	int c=0; //for switch	
	opterr=0;
	while((c=getopt(argc, argv, "alhSr")) != -1){
	  switch(c){
	    case 'a':
		print_mode=1; //1 == -a
		aflag=1;
	//	if(argc ==2) dir_path=argv[1];
		break;
	    case 'l':
		print_mode=2; //2 == -l
		lflag=1;
	//	if(argc ==2) dir_path=argv[1];
		break;
	    case 'h': //4==h;
		hflag=1;
		break;
	    case 'S':
	//	print_mode=5;
		Sflag=1;
		break;
	    case 'r':
		rflag=1;
		break;
	    case '?':
		printf("Unknown option character\n");
		return 0;
		break;
        	}
	}
	if(aflag && lflag){ print_mode=3;} //3 == -al or -la
	int wild_fg=0;

    file = fopen("html_ls.html","w"); //writehtml
	char curr_pwd[199]={0,}; //get currentworking direct
	getcwd(curr_pwd, 198); //getcwd
	fprintf(file,"<html>\n"); //opend
	fprintf(file,"<head>\n"); //start head
	fprintf(file,"<title>%s</title>\n",curr_pwd); //printcwd
	fprintf(file,"</head>\n"); //end of head
	fprintf(file,"<body>\n"); //start body
	fprintf(file,"<h1>"); //h1
	for(int t=0 ; argv[t] ; t++){
		fprintf(file,"%s ",argv[t]); //print command
	}
	fprintf(file, "</h1>\n"); //end of h1
//	fprintf(file,"<body>\n"); 

	for(int t=0; t<90; t++){ Is_wild_card[t]=-1;} //initialization
	for(int t=1; t<argc;t++){
		if(Is_wild(argv[t],t)){wild_fg=1; } //call wildfunction and return0;
		else{ }	
	}
	if(wild_fg){	for(int r=0; r< argc-1;r++){ //if have wildcard
			wild_print(dir_path, argc, argv[r+1],r);} //call funct
			wild_sort();  
             fprintf(file,"</body> </html>"); fclose(file); //closefile
            return 0;}  //return

	if(argc>=3) {
		int pip=2; //default option input exist
		if(!aflag && !lflag&& !rflag && !hflag && !Sflag ){pip=1;} //if no option => pip==1
		for(int k=0; k<argc-pip; k++){ //for error code
			dir_path = argv[k+pip];
			DIR*drip=opendir(dir_path); //open dir
			struct stat f_s;
			if(lstat(dir_path, &f_s)<0){printf("cannot access %s : No such file or directory\n",dir_path);}
			else{/* if(drip==NULL){printf("caanot access %s: No such file or directory\n",dir_path);}*/}
		}
		for(int k=0; k<argc-pip;k++){ //file or direc and call funct
			dir_path = argv[k+pip];
			DIR *drip=opendir(dir_path); //open dir
			struct stat f_s;
			if(lstat(dir_path, &f_s)<0){ continue; } //if error->continue
			else{ if(!S_ISDIR(f_s.st_mode)){ //if it is not directory
					fprintf(file,"<table border='1'>\n"); //open table
				 if(!lflag){
					fprintf(file,"<tr><th>Name</th><tr>\n"); 
                fprintf(file,"<tr><td style='color: Red'><a= href'%s'>%s</a></td></tr>\n",dir_path,dir_path);
                fprintf(file,"</table>\n");
					continue;} //justprint
				 else {	char path_tmp[199]; getcwd(path_tmp,1024);//if option l include
				 	fprintf(file,"Directory path: %s<br>",path_tmp); //directory path
				fprintf(file,"total : %ld<br>\n",f_s.st_blocks);
				 strcat(path_tmp,"/"); //form path and call funct
				 strcat(path_tmp, dir_path); fprintf(file,"<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>\n");	
					f_info(path_tmp, dir_path);
					 fprintf(file,"</table>\n"); continue;}//call f_info
			} }
			
			if(drip==NULL){ continue;}//if error continue;
			closedir(drip); //close directroy
			ls_sort(print_mode, dir_path); //sort filename list
			
		}
	}
	else{
		if(!aflag && !lflag&& !hflag &&!rflag && !Sflag &&argc==2 ){ 
			char tmppp[1024]={0,}; dir_path=argv[1]; //if anyflag and argv
			strcpy(tmppp, argv[1]);

			} //if no option
		DIR *drip=opendir(dir_path); struct stat f_s; // print errorcode
		if(lstat(dir_path, &f_s)<0){printf("cannot access %s: No such file or directory\n",dir_path);}
		else{ if(!S_ISDIR(f_s.st_mode)){ //if not directory
            fprintf(file,"<table border='1'>\n"); //table setting
			if(!lflag){ //if not lflag
                fprintf(file,"<tr><th>Name</th><tr>\n");  //set table
                fprintf(file,"<tr><td style='color: Red'><a= href'%s'>%s</a></td></tr>\n",dir_path,dir_path); //set table
                fprintf(file,"</table>\n"); fprintf(file,"</body> </html>"); //endof table
                return 0; //return
            } //just print
			else{char path_tmp[199]; getcwd(path_tmp,1024);//if option l include
				fprintf(file,"Directory path: %s<br>",path_tmp); //directory path
				fprintf(file,"total : %ld<br>\n",f_s.st_blocks);
				 strcat(path_tmp,"/"); //form path and call funct
				 strcat(path_tmp, dir_path); fprintf(file,"<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>\n");	
                 f_info(path_tmp, dir_path); //call funct
                 fprintf(file,"</table>\n"); fprintf(file,"</body> </html>"); //end of html
                 return 0;
            }
		}}
	
		closedir(drip);
		ls_sort(print_mode, dir_path);//file name list sort
	}
    fprintf(file,"</body> </html>"); //close html
    fclose(file); //close file
	
	return 0;
}
void ls_sort(int print_mode,char* dir_path){ //file name list sort
	DIR *dir;
	if((dir=opendir(dir_path))==NULL){printf("cannnot access %s: No such file or directory \n",dir_path);return ;}
	struct dirent *entry; //if opendir is fail, error
	char **filelist = NULL; //filename list
	int n=0;
	while((entry = readdir(dir)) !=NULL){ //realloc
        if(strcmp(entry->d_name,"html_ls.html")==0){continue;} //if html->pass
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
			  int ca = toupper(a[p]), cb=toupper(b[k]);
			  if(ca != cb){ //if different
			    if((ca< cb)&&rflag==0) break; //pass
			    else if((ca>cb)&&rflag==1)break;
			    else { min=j ; break;} //store
			  }
			}
			if (!b[k] && a[p]&& rflag==0) {min = j;} //store
			else if (b[k] && !a[p] && rflag==1){min = j;}
		}
		else if(b[0]=='.'&& a[0]!='.'){ //if b is hidden file
			for(k=0,p=1 ; a[k] &&b[p] ; k++,p++){ //. is ignore
			 int ca = toupper(a[k]), cb=toupper(b[p]); //small alphabet=>int
			 if(ca != cb){ //if differnet
			   if((ca < cb)&&rflag==0)break; //pass
			   else if((ca> cb)&&rflag==1)break; //if reverse
			   else{min = j; break;}
		     	 }
			}
			if(!b[p] && a[k]&& !rflag) {min=j;} //set
			else if(b[p] && !a[k] && rflag){min = j;}
		}
		else{
			for(k=0; a[k]&&b[k] ;k++){ //if both not hidden or both hidden
			  int ca = toupper(a[k]), cb=toupper(b[k]);
			  if(ca != cb){
			    if((ca <cb)&&rflag==0) break; //same process
			    else if((ca>cb)&&rflag==1)break; //if reverse
			    else{min=j; break;}
			  }
			}
			if(!b[k] && a[k] && !rflag) {min=j;}
			else if (b[k] && !a[k] && rflag){min =j;} //if reverse
		}
	  }
	  if(min != i){ //if min and i not same
		char *tmp = filelist[i]; //swap
		filelist[i] = filelist[min];
		filelist[min] = tmp;
	  }

	}
	

//////////////////////////////////option s///////////////////////////////
	if(Sflag){
		int w;
		for(w=0; w<n-1;w++){ //sort
		  int M=w, mini=w;
		  for(M=M+1 ; M<n; M++){
			char *a=filelist[mini] ,*b=filelist[M];
			int k;
			char bufa[1024], bufb[1024];
			if(dir_path[0]=='.'){ //if pathis relative
				realpath(dir_path,bufa); //get realpath
				realpath(dir_path,bufb); //get realpath
			}
			else if(dir_path[0]=='/'){ strcpy(bufa,dir_path); strcpy(bufb,dir_path);
			} //if absolute =>pass
			else{
				getcwd(bufa,1024); //else getcwd
				getcwd(bufb,1024); //getcurrent working directory
			}
			strcat(bufa,"/"); strcat(bufb,"/"); //connect
			strcat(bufa,filelist[mini]); strcat(bufb,filelist[M]); //connectfilename
			struct stat astat; struct stat bstat; 
			lstat(bufa, &astat); lstat(bufb,&bstat); //for get size
		//	printf("a=%ld, b=%ld ", astat.st_size, bstat.st_size);//get size
			if(astat.st_size!= bstat.st_size){ //compare size
				
				if(astat.st_size <= bstat.st_size){}
				else {mini=M; } //mini=M
			}
	  	  }
		  if(mini != w){ //if not equal mini and w
		   char *tmp= filelist[w]; //swap
		   filelist[w]=filelist[mini];
		   filelist[mini]=tmp;

		   }
		}
	}
/////////////////////////////////////////////////////////////////////////////		
	

	char buff[1024]; //store path
	int absol=0;
	if(dir_path[0]=='.'){ realpath(dir_path, buff);} //relative->absolute
	else if(dir_path[0]=='/'){absol=1;}//absolute
	else { //just file or directory
		getcwd(buff,1024);}
	//	strcat(buff,"/");}
	//	strcat(buff,filelist[i]);}
	//filepath print
	int total=0;
	if(print_mode==2 || print_mode==3){ //if contain l
		if(absol){fprintf(file, "Directory path: %s<br>", dir_path);} //printfdirpath
		else{	fprintf(file, "Directory path: %s<br>",buff);}//print dir path
		struct stat tota;
		if(absol){ strcpy(buff, dir_path);} //ifabsol->copy
		for(int e=0 ; e<n;e++){
			if(print_mode==2&&filelist[e][0]=='.'){continue;} //printmode
			char plus_path[199];
			strcpy(plus_path,buff); strcat(plus_path,"/"); strcat(plus_path,filelist[e]);
			lstat(plus_path,&tota); total+= tota.st_blocks; //for store total st_block
		}	
		total = total/2; //division		
		fprintf(file,"total :%d<br>\n",total); } //print totalsize

    fprintf(file,"<table border='1'>\n"); //open table
	if(lflag == 0){ //if lflag not set
		fprintf(file,"<tr><th>Name</th></tr>\n"); //just name
	}
	else{ //or
	fprintf(file,"<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>\n");
	} //set table focus on lflag


	char buff2[1024]; //store past path
	for( i=0; i<n;i++){ //loop n
		char buff1[1024];
		if(dir_path[0]=='.'){ realpath(dir_path,buff1);} //realpath
		else if(dir_path[0]=='/'){ if(i==0){strcpy(buff2, dir_path); strcpy(buff1,buff2);} else{strcpy(buff1,buff2);}}
		else{  getcwd(buff1,1024); } //if relative or absol or just directory
		 
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
			fprintf(file,"<tr><td style='color:%s'><a href='%s'>%s</a></td></tr>",col_mode,buff1, filelist[i]);
		} //print filename without hidden
		else if( print_mode==1){ //if -a
			fprintf(file,"<tr><td style='color:%s'><a href='%s'>%s</a></td></tr>",col_mode,buff1,filelist[i]); //all print
		}
		else if( print_mode==2){ //if -l
			if(filelist[i][0]=='.'){continue;}//not printhidden
			f_info(buff1,filelist[i]); //call f_info
		}
		else if(print_mode==3){ //if -al
			
			f_info(buff1,filelist[i]); //call f_ifno
		}
	}
    fprintf(file,"</table>\n"); //endof table
	for( i=0; i<n;i++){
		free(filelist[i]); //delete
	}
	free(filelist); //delete
	return ;
			

}
void f_info(char* filepath,char* filenme){
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

	
	fprintf(file,"<tr style='color:%s'><td><a href='%s'>%s</a></td>", col_mode, filepath, filenme);
	fprintf(file,"<td>%c", S_ISDIR(filestat.st_mode) ?'d':'-');
	fprintf(file,"%c", filestat.st_mode & S_IRUSR ? 'r' : '-'); //read
	fprintf(file,"%c", filestat.st_mode & S_IWUSR ? 'w' : '-'); //write
	fprintf(file,"%c", filestat.st_mode & S_IXUSR ? 'x' : '-'); //excute
	fprintf(file,"%c", filestat.st_mode & S_IRGRP ? 'r' : '-');
	fprintf(file,"%c", filestat.st_mode & S_IWGRP ? 'w' : '-');
	fprintf(file,"%c", filestat.st_mode & S_IXGRP ? 'x' : '-');
	fprintf(file,"%c", filestat.st_mode & S_IROTH ? 'r' : '-');
	fprintf(file,"%c", filestat.st_mode & S_IWOTH ? 'w' : '-');
	fprintf(file,"%c</td>", filestat.st_mode & S_IXOTH ? 'x' : '-');
	//link num
	fprintf(file,"<td>%3ld</td>",filestat.st_nlink);
	//user and group
	struct passwd* pw = getpwuid(filestat.st_uid);
	struct group* gr = getgrgid(filestat.st_gid);
	fprintf(file,"<td>%s</td>", pw ? pw->pw_name : "unkown");
	fprintf(file,"<td>%s</td>", gr ? gr->gr_name : "unkown");
	//file size
	fprintf(file,"<td>");
	if(hflag){ //if hflag is 1
		if(filestat.st_size<1024){fprintf(file,"%.1fB",roundf(filestat.st_size*100)/100);} //byte
		else if(filestat.st_size<1024*1024){fprintf(file,"%.1fK",roundf(filestat.st_size/1024.0*100)/100);} //kilo
		else if(filestat.st_size<1024*1024*1024){fprintf(file,"%.1fM",filestat.st_size/(1024.0*1024.0));} //mega
		else{fprintf(file,"%.1fG", filestat.st_size/(1024.0*1024.0*1024.0));} //giga
	}
	else{	fprintf(file,"%6ld",filestat.st_size);}
	fprintf(file,"</td>"); //table col end
	//time
	char mod_time[32];
	strftime(mod_time, sizeof(mod_time), "%b %d %H: %M", localtime(&filestat.st_mtime));
	fprintf(file,"<td>%s</td></tr>", mod_time);
	//file name
}

