//////////////////////////////////////////////////////////////////
//File Name	: 2021202057_advanced_ls.c			//
//Date		: 2023/04/05					//
//OS		: Ubuntu 16.04 LTS 64bits			//
//Author	: HWang Se Young				//
//Student ID	: 2021202057					//
//-------------------------------------------			//
//Title : System Programming Assignment #1-2			//
//Description : advanced_ls					//
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

void f_info(char* filepath,char* filenme){
	struct stat filestat;
	if(lstat(filepath, &filestat)<0){
	  printf("could not stat file: %s\n", filepath);
	  return;
	} //filepath
	//filetype
	printf("%c", S_ISDIR(filestat.st_mode) ?'d':'-');
	printf("%c", filestat.st_mode & S_IRUSR ? 'r' : '-');
	printf("%c", filestat.st_mode & S_IWUSR ? 'w' : '-');
	printf("%c", filestat.st_mode & S_IXUSR ? 'x' : '-');
	printf("%c", filestat.st_mode & S_IRGRP ? 'r' : '-');
	printf("%c", filestat.st_mode & S_IWGRP ? 'w' : '-');
	printf("%c", filestat.st_mode & S_IXGRP ? 'x' : '-');
	printf("%c", filestat.st_mode & S_IROTH ? 'r' : '-');
	printf("%c", filestat.st_mode & S_IWOTH ? 'w' : '-');
	printf("%c\t", filestat.st_mode & S_IXOTH ? 'x' : '-');
	//link num
	printf("%3ld\t",filestat.st_nlink);
	//user and group
	struct passwd* pw = getpwuid(filestat.st_uid);
	struct group* gr = getgrgid(filestat.st_gid);
	printf("%s\t", pw ? pw->pw_name : "unkown");
	printf("%s\t", gr ? gr->gr_name : "unkown");
	//file size
	printf("%6ld\t",filestat.st_size);
	//time
	char mod_time[32];
	strftime(mod_time, sizeof(mod_time), "%b %d %H: %M", localtime(&filestat.st_mtime));
	printf("%s\t", mod_time);
	//file name
	printf("%s\n", filenme);
}

void ls_sort(int print_mode,char* dir_path);
int main(int argc, char* argv[]){
	int print_mode = 0, aflag=0, lflag=0 ;
	char* dir_path =".";
	int c=0;	
	opterr=0;
	while((c=getopt(argc, argv, "al")) != -1){
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
	    case '?':
		printf("Unknown option character\n");
		return 0;
		break;
        	}
	}
	if(aflag && lflag){ print_mode=3;} //3 == -al or -la


	if(argc>=3) {
		int pip=2; //default option input exist
		if(aflag ==0 && lflag==0){pip=1;} //if no option => pip==1
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
				 if(print_mode==0||print_mode==1){printf("%s\n",dir_path); continue;} //justprint
				 else {	char path_tmp[199]; getcwd(path_tmp,1024);//if option l include
					printf("Directory path: %s\n",path_tmp);
					printf("total : %ld\n",f_s.st_blocks);
					 strcat(path_tmp,"/");
					 strcat(path_tmp,dir_path);
					f_info(path_tmp, dir_path); continue;}//call f_info
			} }
			
			if(drip==NULL){ continue;}//if error continue;
			closedir(drip); //close directroy
			ls_sort(print_mode, dir_path); //sort filename list
			
		}
	}
	else{
		if(aflag ==0 && lflag==0){ dir_path = argv[1];} //if no option
		DIR *drip=opendir(dir_path); struct stat f_s; // print errorcode
		if(lstat(dir_path, &f_s)<0){printf("cannot access %s: No such file or directory\n",dir_path);}
		else{ if(!S_ISDIR(f_s.st_mode)){ //if not directory
			if(print_mode==0||print_mode==1){printf("%s\n",dir_path);return 0;} //just print
			 else{char path_tmp[199]; getcwd(path_tmp,1024);//if option l include
				printf("Directory path: %s\n",path_tmp); //directory path
				printf("total : %ld\n",f_s.st_blocks);
				 strcat(path_tmp,"/"); //form path and call funct
				 strcat(path_tmp, dir_path); f_info(path_tmp, dir_path);return 0;}
		}}
	
		closedir(drip);
		ls_sort(print_mode, dir_path);//file name list sort
	}

	return 0;
}
void ls_sort(int print_mode,char* dir_path){ //file name list sort
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
		if(absol){printf("Directory path: %s\n", dir_path);}
		else{	printf("Directory path: %s\n",buff);}//print dir path
		struct stat tota;
		if(absol){ strcpy(buff, dir_path);} 
		for(int e=0 ; e<n;e++){
			char plus_path[199];
			strcpy(plus_path,buff); strcat(plus_path,"/"); strcat(plus_path,filelist[i]);
			lstat(plus_path,&tota); total+= tota.st_blocks; //for store total st_block
		}	
		total = total/2; //division		
		printf("total :%d\n",total); }
	char buff2[1024]; //store past path
	for( i=0; i<n;i++){ //loop n
		char buff1[1024];
		if(dir_path[0]=='.'){ realpath(dir_path,buff1);} //realpath
		else if(dir_path[0]=='/'){ if(i==0){strcpy(buff2, dir_path);} else{strcpy(buff1,buff2);}}
		else{  getcwd(buff1,1024); } //if relative or absol or just directory
		 

		if(print_mode==0){ //if no option
			if(filelist[i][0]=='.'){continue;}//not print hidden
			printf("%s\n", filelist[i]);
		}
		else if( print_mode==1){ //if -a
			printf("%s\n",filelist[i]); //all print
		}
		else if( print_mode==2){ //if -l
			if(filelist[i][0]=='.'){continue;}//not printhidden
			strcat(buff1,"/");
			strcat(buff1,filelist[i]); //form path
			f_info(buff1,filelist[i]); //call f_info
		}
		else if(print_mode==3){ //if -al
			strcat(buff1,"/");
			strcat(buff1,filelist[i]); //form path
			f_info(buff1,filelist[i]); //call f_ifno
		}
	}

	for( i=0; i<n;i++){
		free(filelist[i]); //delete
	}
	free(filelist);
	return ;
			

}
