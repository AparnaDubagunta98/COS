#include "apix_shell.h"

string shell_prompt = "HOME apixshell@@ ";
string HOME = "/";
bool p,redir,dir,exit_cmd,amp,echo;
char old_cwd[100];

void shell_loop()
{
  while(1)
  {
  	cout<<shell_prompt;
     string st;

	   getline(cin,st);
     char *cmd[100];
     parse(st,cmd);
  }
}

void parse(string cmd_str,char *cmd[])
{
  echo = chk_pat(cmd_str,"echo");
  if(echo == false)
  {
    p = chk_pat(cmd_str,"|");
    redir  = chk_pat(cmd_str,">") || chk_pat(cmd_str,"<");
    amp = chk_pat(cmd_str,"&");
    dir = chk_pat(cmd_str,"cd") ;
    exit_cmd = chk_pat(cmd_str,"exit");



	if(p==false && redir == false  && dir == false)
	{
		simple_unix_command(cmd_str,cmd);
	}
	else if(dir >0)
	{
		directory_unix_command(cmd_str,cmd);
	}
	else if(redir == true && p == false)
	{
		redirection_unix_commmand(cmd_str,cmd,0);
	}
	else if(p == true)
	{
		pipe_unix_command(cmd_str,cmd);
	}
}
else
{
  simple_unix_command(cmd_str,cmd);
}

}

void simple_unix_command(string cmd_str,char *cmd[])
{
	 int i =0,number_of_commands;

	 char *str = strtok((char*)cmd_str.c_str()," ");
	 while(str != NULL)
	 {
	 	cmd[i] = str;
	 	i++;
	 	str = strtok(NULL," ");
	 }
	cmd[i] = NULL;
	number_of_commands= i;

	execute_cmd(cmd,SU,NULL);
}


void directory_unix_command(string cmd_str,char *cmd[])
{
  char cwd[100];

  getcwd(cwd,sizeof(cwd));
  char *current_path = cwd;

  int i=0,j=0;
  string p;
  char *cmd_red[100];
  string str = strtok((char*)cmd_str.c_str()," ");
  if(str == "cd")
  {
    p = strtok(NULL," ");

    if(p == "--" || p == "~" || p == "/")
    {
      cmd[0] = (char*)HOME.c_str();
      getcwd(old_cwd,sizeof(old_cwd));
      execute_cmd(cmd,CHDIR,NULL);
    }
    else if(p == "..")
    {
      if(strcmp(current_path,(char*)HOME.c_str()) == 0)
      {
        cmd[0] = (char*)HOME.c_str();
        getcwd(old_cwd,sizeof(old_cwd));
        execute_cmd(cmd,CHDIR,NULL);
      }
      else
      {
        get_previous_path(cwd);
        if(cwd[0] == '\0')
        {
          cwd[0] = '/' ;
          cwd[1] = '\0';
        }
        else
          cmd[0] = cwd;
        getcwd(old_cwd,sizeof(old_cwd));
        execute_cmd(cmd,CHDIR,NULL);
      }
    }
    else if(p == "-")
    {
      char to_put[100];
      copy(begin(old_cwd),end(old_cwd),begin(to_put));
      cmd[0] = to_put;
      getcwd(old_cwd,sizeof(old_cwd));
      execute_cmd(cmd,CHDIR,NULL);
    }
    else
    {
      if(p[0] == '/')
        {
          p = HOME + p;
        }
      cmd[0] = (char*)p.c_str();
      getcwd(old_cwd,sizeof(old_cwd));
      execute_cmd(cmd,CHDIR,NULL);
    }
  }

}


const char* redirection_unix_commmand(string cmd_str,char *cmd[],int pipe)
{
  //if ired - ret true else opred
  //<>
  bool type_of_redirection = chk_pat(cmd_str,">");
  if(type_of_redirection == true)
  {
    //output red
    int i=0,j=0;
    char *cmd_red[100];
    char *str = strtok((char*)cmd_str.c_str(),">");
    while(str != NULL)
    {
     // cout<<str<<endl;
     cmd_red[i] = str;
     i++;
     str = strtok(NULL," ");
    }
    cmd_red[i] = NULL;

    string unix_cmd = cmd_red[0];
    char *str1 = strtok((char*)unix_cmd.c_str()," ");
    while(str1 != NULL)
    {
     //cout<<str1<<endl;
     cmd[j] = str1;
     j++;
     str1 = strtok(NULL," ");
    }
    cmd[j] = NULL;
    //get fname
    const char *fname = cmd_red[1];
    if(pipe == 1)
    {
      return fname;
    }

    else if(pipe == 0)
    {
      execute_cmd(cmd,OR,fname);
    }

  }
  else
  {
    int i=0,j=0;
    char *cmd_red[100];
    char *str = strtok((char*)cmd_str.c_str(),"<");
    while(str != NULL)
    {
     cmd_red[i] = str;
     i++;
     str = strtok(NULL," ");
    }
    cmd_red[i] = NULL;
    string unix_cmd = cmd_red[0];
    char *str1 = strtok((char*)unix_cmd.c_str()," ");
    while(str1 != NULL)
    {
     cmd[j] = str1;
     j++;
     str1 = strtok(NULL," ");
    }
    cmd[j] = NULL;
    //get fname
    const char *fname = cmd_red[1];
    if(pipe == 0)
      execute_cmd(cmd,IR,fname);
    else if(pipe == 1)
    {
      //cout<<"in pipe"<<endl;
      return fname;
    }
  }

}

void pipe_unix_command(string cmd_str,char *cmd[])
{
  vector<string> parts = split_char(cmd_str);

  int num_of_pipes = parts.size() -1;
  int num_processes = parts.size();
  int pid,fd_out,fd_in,input_file;
  int of = 0;

  int curr_in = dup(0);
  int curr_out = dup(1);


  //get_file_names(cmd_str,&input_file_name,&output_file_name);
  const char *in_fname = NULL;
  const char *out_fname = NULL;
  char * cmd_of[100];
  char * cmd_f[100];
  for(int i=0;i < num_processes;i++)
  {
  if((chk_pat(parts[i],">") || chk_pat(parts[i],"<")) == true)
  {
    if((chk_pat(parts[i],">")) == true)
    {
      out_fname = redirection_unix_commmand(parts[i],cmd_of,1);
      //cout<<cmd_of[0]<<endl;
    }
    else if((chk_pat(cmd_str,"<")) == true)
    {
      in_fname = redirection_unix_commmand(parts[i],cmd_f,1);
    }
  }
 }

  if(in_fname != NULL)
  {
    fd_in = open(in_fname,O_CREAT|O_RDONLY|S_IRUSR|S_IRGRP|S_IROTH);
  }
  else
  {
      fd_in = dup(curr_in);
  }


  for(int i=0;i < num_processes;i++)
  {
    char * cmd_p[100];

    split_space(parts[i],cmd_p);

    dup2(fd_in,0);
    close(fd_in);

    if(i == num_processes - 1)
    {
      if(out_fname != NULL)
      {
        fd_out = open(out_fname,O_CREAT|O_WRONLY|O_TRUNC|S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
        of = 1;
      }
      else
      {
        fd_out = dup(curr_out);
        of = 0;
      }
    }

    else
    {
      //not last need a pipe to next process
      int fds[2];
      pipe(fds);

      fd_in = fds[0];
      fd_out = fds[1];
    }

    dup2(fd_out,1);
    close(fd_out);

    pid = fork();
    if(pid == 0)
    {
      if(of ==0 )
      {
        execvp(cmd_p[0],cmd_p);
        //cout<<"command not found"<<endl;
      }

      else if(of == 1)
      {
        execvp(cmd_of[0],cmd_of);
        //cout<<"command not found"<<endl;
      }
    }
  }

  dup2(curr_in,0);
  dup2(curr_out,1);
  close(curr_in);
  close(curr_out);

  if(amp == false)
	{
		waitpid(pid,NULL,0);
	}
}



void execute_cmd(char *cmd[],Cmd_type ctype,const char *fname)
{
	int pid = fork();
	if(pid == 0)
	{
    if(ctype == SU)
		  {
        execvp(cmd[0],cmd);
        //cout<<"command not found"<<endl;
      }
    else if(ctype == OR)
    {
      int fd = open(fname,O_CREAT|O_WRONLY|O_TRUNC|S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
      dup2(fd,1);
      execvp(cmd[0],cmd);
      //cout<<"command not found"<<endl;
    }
    else if(ctype == IR)
    {
      int fd = open(fname,O_CREAT|O_RDONLY|S_IRUSR|S_IRGRP|S_IROTH);
      dup2(fd,0);
      execvp(cmd[0],cmd);
      //cout<<"command not found"<<endl;
    }
    else if(ctype == CHDIR)
    {
      char directory[1024];
      if(chdir(cmd[0]) == 0)
      {
        getcwd(directory, sizeof(directory));
        if(strcmp(directory,(char*)HOME.c_str()) == 0)
        {
          shell_prompt = "HOME apixshell@@ ";
        }
        else
        {
          string new_dir = get_dir_name(directory);
          shell_prompt = new_dir + " apixshell@@ ";
        }
      }
      else
      {
        cerr<<"cd: No such file or directory"<<endl;
      }
    }

    }

	else if(amp == false)
	{
		waitpid(pid,NULL,0);
	}
}


int main()
{

 const char *home = "/";
 chdir(home);
 shell_loop();
 return 0;

}
