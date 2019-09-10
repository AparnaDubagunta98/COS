#ifndef _APIX_SHELL_H_
#define _APIX_SHELL_H_
#include<iostream>
#include<cstring>
#include<sstream>
#include<string>
#include<iomanip>
#include<stdio.h>
#include<unistd.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/wait.h>
#include <cmath>
#include <numeric>
#include <algorithm>
#include <sys/types.h>
#include<vector>

using namespace std;

//simple unix,input redirection, output redirection, pipe, redirection with pipe,amp,directory.
enum Cmd_type {SU,IR,OR,CHDIR}; //removed amp


bool chk_pat(string s,string c)
{
	if(strstr(s.c_str(),c.c_str()))
		return true;
	return false;

}

void split_space(string cmd_str,char *cmd[])
{
	int i =0;

	char *str = strtok((char*)cmd_str.c_str()," ");
	while(str != NULL)
	{
	 cmd[i] = str;
	 i++;
	 str = strtok(NULL," ");
	}
 cmd[i] = NULL;
 //number_of_commands=
}


vector<string> split_char(string cmd_str)
{
	int i =0;

	//char *cmd_p[100];
  vector<string> parts;
	char *str = strtok((char*)cmd_str.c_str(),"|");
	while(str != NULL)
	{
	  string s= str;
		parts.push_back(s);
	  i++;
	  str = strtok(NULL,"|");
	}

 return parts;
}


void split_chars(string cmd_str,char *cmd[],const char* ch)
{
	int i =0;

	char *str = strtok((char*)cmd_str.c_str(),ch);
	while(str != NULL)
	{
	 cmd[i] = str;
	 i++;
	 str = strtok(NULL," ");
	}
 cmd[i] = NULL;
 //number_of_commands=
}



void get_previous_path(char *cwd)
{
  for(int i=strlen(cwd)-1;i>=0;i--)
	{

		if(cwd[i] == '/')
		{
			cwd[i] = '\0';
			break;
		}
	}
}

string get_dir_name(char *dir)
{
	string name="";
	for(int i=strlen(dir)-1;i>0;i--)
	{
		if(dir[i] == '/')
		{
			break;
		}
		name = dir[i] + name;
	}

	return name;
}

void shell_loop();
void execute_cmd(char *cmd[],Cmd_type ctype,const char *fname);
void parse(string cmd_str,char *cmd[]);
void simple_unix_command(string cmd_str,char *cmd[]);
const char* redirection_unix_commmand(string cmd_str,char *cmd[],int pipe);
void directory_unix_command(string cmd_str,char *cmd[]);
void pipe_unix_command(string cmd_str,char *cmd[]);






#endif
