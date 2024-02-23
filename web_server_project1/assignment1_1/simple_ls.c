//////////////////////////////////////////////////////////////////////////
// File Name	:Main.c							//
// Date		:2023/03/26						//
// Os		:Ubuntu 16.04 LTS 64bits				//
// Author	:Hwang Se Young						//
// Student ID	:2021202057						//
//--------------------------------------------------------------------  //
// Title : System Programming Assignment #1-1 				//
// Description : simple_ls						//
//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////
// InsertNode								//
// =====================================================================//
// Input :char*  directory path						//
// Output:char*  directory name						//
// Purpose : sort directory name (ls)					//
//////////////////////////////////////////////////////////////////////////


#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char *argv[]){
	if(argc  >2){ // 2 and more direct paht = >error
	  printf("simple_ls: only one directory path can be processed\n");
	  return 0;	
	}
	char *dirname="."; //initialization default=>current loc
	if(argc == 2) dirname=argv[1]; //directory path

	DIR *dir = opendir(dirname); //open directory

	if (!dir){ printf("simple_ls: cannot access \'%s\':No such directory\n  ",argv[1]);
		return 0; //if not directory or not existence
	}
	struct dirent *entry; //entry
	char **filelist = NULL;
	int n=0;
	
	while ((entry = readdir(dir)) != NULL){ //loop until null
		if(entry->d_name[0] =='.') continue; //hidden file exception
		filelist = (char **) realloc(filelist , sizeof(char *)*(n+1));
		filelist[n++] = strdup (entry->d_name); //alloc and put name
	}
	closedir(dir); //close dir
	int i; //iteger
	//sort filename
	for( i=0; i<n-1 ; i++){ //compare name
		int j, min = i;
		for(j=i+1 ; j<n ; j++){ //one by one
		char *a= filelist[min], *b = filelist[j];
		int k;
			for(k=0; a[k] && b[k];k++){ //if both arr not null
				int ca = tolower(a[k]), cb=tolower(b[k]); //compare
				if(ca != cb){ //if not same
				if(ca <cb) break; //dontcare
				else { min = j; break;} 
				}
			}
			if ( !b[k] && a[k]) {min = j;}
		}
		if(min != i){ //if min and j not equal
			char * tmp = filelist[i]; //swap
			filelist[i] = filelist[min];
			filelist[min]=tmp;
		}
	}
	for( i=0; i<n; i++){ //print sorting dir list 
		printf("%s\n",filelist[i]);
		free(filelist[i]); //memory free
	}
	free(filelist); //memory free
	return 0;
}
