#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <cstdio>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <regex>
#include <vector>
using namespace std;

vector<struct dirent*> results;
vector<struct dirent*>::iterator it;

bool isdir(struct stat buf){
	if(S_ISDIR(buf.st_mode)){
		return 1;	
	}
	return 0;
}

void find(char *path, char *name, char *mtime){
	DIR *target_path_dir = opendir(path);
	struct dirent *target_path_file;

	while(target_path_file = readdir(target_path_dir)){
		if(strcmp(target_path_file->d_name, ".") == 0 ||
			strcmp(target_path_file->d_name, "..") == 0)
			continue;

		char *search_result_path = new char[strlen(path) + strlen(target_path_file->d_name) + 1];
		strcpy(search_result_path, path);
		strcat(search_result_path, target_path_file->d_name);
		struct stat statbuf;
		int status = stat(search_result_path, &statbuf);

		if (isdir(statbuf) == 0){
			if(name != nullptr){
				regex re_name(name);
				if(regex_match(target_path_file->d_name, re_name)){
					continue;
				}	
			}
	
			if(mtime != nullptr){
				time_t nowtime = time(0);
				int m = atoi(mtime);
				if(status != -1){
					if (nowtime - statbuf.st_mtime < m * 86400){
						continue;
					}
				}					
			}
			results.push_back(target_path_file);
		}
		else if(isdir(statbuf) == 1){
			find(search_result_path, name, mtime);
		}
		
	}
}

int main(int argc, char *argv[]){
	char *path;
	char *name = nullptr;
	char *mtime = nullptr;
	
	if(argc <= 1){
		cout<<"Invalid Path Input!"<<endl;
		return 0;
	}
	path = argv[1];
	for(int i=2;i<argc;i+=2)
	{
		char *tmp = argv[i];
		if (strcmp(tmp, "-name") == 0){
			name = argv[i+1];
		}
		else if (strcmp(tmp, "-mtime") == 0){
			mtime = argv[i+1];	
		}
		else{
			cout<<"Invalid Optimal Input"<<endl;		
		}
	}
	//cout<<path<<" "<<name<<" "<<mtime<<endl;
	
	find(path, name, mtime);
	
	if(results.size() == 0){
		cout<<"No File Found!"<<endl;
	}
	else{
		for(auto i:results){
			cout<<i->d_name<<endl;
		}
	}
	return 0;
}
