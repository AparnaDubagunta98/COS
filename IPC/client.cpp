/*
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date  : 2/8/19
 */
#include<sys/wait.h>
#include "common.h"
#include "FIFOreqchannel.h"


using namespace std;

void Request_data_points(FIFORequestChannel chan)
{
   timeval start,end;
   gettimeofday(&start,NULL);
   ofstream fs;
   fs.open("x1.csv",ofstream::out);
   double t;
   
    for(t=0;t<0.064;t+=0.004)
    {
    datamsg data_point_request_msg1(1,t,1);
      char * dmsg1_address = (char*)(&data_point_request_msg1);
      int len = chan.cwrite(dmsg1_address,sizeof(datamsg));
      double *ecg_1 = (double*)(chan.cread(&len));
   
      datamsg data_point_request_msg2(1,t,2);
      char * dmsg2_address = (char*)(&data_point_request_msg2);
      int len2 = chan.cwrite(dmsg2_address,sizeof(datamsg));
      double *ecg_2 = (double*)(chan.cread(&len2));
    cout<<t <<","<<*ecg_1<<","<<*ecg_2<<endl;
    fs <<t <<","<<*ecg_1<<","<<*ecg_2<<endl;
   }
   gettimeofday(&end,NULL);
   double tt;
   tt = (end.tv_sec - start.tv_sec) * 1e6 ;
   tt = (tt + (end.tv_usec - start.tv_usec))* 1e-6;
   cout<<"TIME TAKEN TO GET DATA IS :"<< tt<<"sec"<<endl;

   fs.close();
} 

void Request_text_file(FIFORequestChannel chan)
{

   timeval start,end;
   gettimeofday(&start,NULL);

    string fname = "1.csv";
    filemsg *file_request_msg_o = new filemsg(0,0);

   // string rc_fname = "received/"+ fname;

    char *buffer = new char[sizeof(file_request_msg_o) + sizeof(fname)];
    
    buffer = (char*)(file_request_msg_o);
    strcpy(buffer + sizeof(filemsg),fname.c_str());

    int wrlen = chan.cwrite(buffer, (sizeof(filemsg) + sizeof(fname) + 1));

    __int64_t* file_length = (__int64_t*)(chan.cread(&wrlen));
    cout<<"fl: "<<*file_length<<endl;

    __int64_t number_of_requests = floor(float(*file_length)/float(MAX_MESSAGE));
    cout<<"nor"<<number_of_requests<<endl;

    __int64_t extra =*file_length -  number_of_requests*MAX_MESSAGE;
    cout<<extra<<endl;


    string fname1 = "1.csv";
    string rc_fname = "received/"+ fname;

    FILE *fp = fopen(rc_fname.c_str(),"a+");

    __int64_t os = 0;
    for(__int64_t i =0;i<number_of_requests;i++)
   {
    filemsg *file_request_msg_1 = new filemsg(os,MAX_MESSAGE);

    char *buffer1 = new char[sizeof(file_request_msg_1) + sizeof(fname1)];
    
    buffer1 = (char*)(file_request_msg_1);
    strcpy(buffer1 + sizeof(filemsg),fname1.c_str());

    int wrlen1 = chan.cwrite(buffer1, (sizeof(filemsg) + sizeof(fname1) + 1));

    char * response = (char*)chan.cread(&wrlen1);

    //fseek(fp,os,SEEK_SET);
    fwrite(response,sizeof(char),MAX_MESSAGE,fp);
   // cout<<response<<endl;
    os += MAX_MESSAGE;

  }

  if(extra != 0)
  {
   filemsg *file_request_msg_1 = new filemsg(os,extra);
    char *buffer1 = new char[sizeof(file_request_msg_1) + sizeof(fname1)];
    
    buffer1 = (char*)(file_request_msg_1);
    strcpy(buffer1 + sizeof(filemsg),fname1.c_str());

    int wrlen1 = chan.cwrite(buffer1, (sizeof(filemsg) + sizeof(fname1) + 1));

    char * response = (char*)chan.cread(&wrlen1);
    fseek(fp,os,SEEK_SET);
    fwrite(response,sizeof(char),extra,fp);
  }

  fclose(fp);

   double tt;
   gettimeofday(&end,NULL);
   tt = (end.tv_sec - start.tv_sec) * 1e6 ;
   tt = (tt + (end.tv_usec - start.tv_usec))* 1e-6;
   cout<<"TIME TAKEN TO GET TEXT FILE :"<< tt<<"sec"<<endl;

}

void Request_binary_file(FIFORequestChannel chan)
{

   timeval start,end;
   gettimeofday(&start,NULL);

    string fname;
    cout<<"Enter file name: "<<endl;
    cin>>fname;
    filemsg *file_request_msg_o = new filemsg(0,0);

   // string rc_fname = "received/"+ fname;

    char *buffer = new char[sizeof(file_request_msg_o) + sizeof(fname)];
    
    buffer = (char*)(file_request_msg_o);
    strcpy(buffer + sizeof(filemsg),fname.c_str());

    int wrlen = chan.cwrite(buffer, (sizeof(filemsg) + sizeof(fname) + 1));

    __int64_t* file_length = (__int64_t*)(chan.cread(&wrlen));
    //cout<<"fl: "<<*file_length<<endl;

    __int64_t number_of_requests = floor(float(*file_length)/float(256));
   // cout<<"nor"<<number_of_requests<<endl;

    __int64_t extra =*file_length -  number_of_requests*256;
   // cout<<extra<<endl;


    string fname1 = fname;
    string rc_fname = "received/"+ fname;

    FILE *fp = fopen(rc_fname.c_str(),"wb");

    __int64_t os = 0;
    for(__int64_t i =0;i<number_of_requests;i++)
   {
    filemsg *file_request_msg_1 = new filemsg(os,MAX_MESSAGE);

    char *buffer1 = new char[sizeof(file_request_msg_1) + sizeof(fname1)];
    
    buffer1 = (char*)(file_request_msg_1);
    strcpy(buffer1 + sizeof(filemsg),fname1.c_str());

    int wrlen1 = chan.cwrite(buffer1, (sizeof(filemsg) + sizeof(fname1) + 1));

    char * response = (char*)chan.cread(&wrlen1);

    fseek(fp,os,SEEK_SET);
    fwrite(response,sizeof(char),MAX_MESSAGE,fp);
   // cout<<response<<endl;
    os += MAX_MESSAGE;

  }

  if(extra != 0)
  {
   filemsg *file_request_msg_1 = new filemsg(os,extra);
    char *buffer1 = new char[sizeof(file_request_msg_1) + sizeof(fname1)];
    
    buffer1 = (char*)(file_request_msg_1);
    strcpy(buffer1 + sizeof(filemsg),fname1.c_str());

    int wrlen1 = chan.cwrite(buffer1, (sizeof(filemsg) + sizeof(fname1) + 1));

    char * response = (char*)chan.cread(&wrlen1);
    fseek(fp,os,SEEK_SET);
    fwrite(response,sizeof(char),extra,fp);
  }
  fclose(fp);

   double tt;
   gettimeofday(&end,NULL);
   tt = (end.tv_sec - start.tv_sec) * 1e6 ;
   tt = (tt + (end.tv_usec - start.tv_usec))* 1e-6;
   cout<<"TIME TAKEN TO GET A BINARY FILE :"<< tt<<"sec"<<endl;
}


void Request_new_channel(FIFORequestChannel chan)
{
 //REQUESTING A NEW CHANNEL
   timeval start,end;
   gettimeofday(&start,NULL);

    MESSAGE_TYPE msg = NEWCHANNEL_MSG;
    char *new_cmsg_address  = (char*)(&msg);
    int new_len = chan.cwrite(new_cmsg_address,sizeof(msg));
    char *name = chan.cread(&new_len);
    cout<<"name of new channel: "<<name<<endl;
    FIFORequestChannel new_chan (name, FIFORequestChannel::CLIENT_SIDE);

  
   double tt;
   gettimeofday(&end,NULL);
   tt = (end.tv_sec - start.tv_sec) * 1e6 ;
   tt = (tt + (end.tv_usec - start.tv_usec))* 1e-6;
   cout<<"TIME TAKEN TO REQUEST NEW CHANNEL :"<< tt<<"sec"<<endl;
   cout<<"\n";
   cout<<"NEW CHANNEL TEST: REQUESTING DATAPOINTS"<<endl;

   //channel test
   double t;
    for(t=0;t<0.012;t+=0.004)
    {
      datamsg data_point_request_msg1(14,t,1);
      char * dmsg1_address = (char*)(&data_point_request_msg1);
      int len = new_chan.cwrite(dmsg1_address,sizeof(datamsg));
      double *ecg_1 = (double*)(new_chan.cread(&len));
   
      datamsg data_point_request_msg2(14,t,2);
      char * dmsg2_address = (char*)(&data_point_request_msg2);
      int len2 = new_chan.cwrite(dmsg2_address,sizeof(datamsg));
      double *ecg_2 = (double*)(new_chan.cread(&len2));
      cout<<t <<","<<*ecg_1<<","<<*ecg_2<<endl;

   }
}

void Send_quit_msg(FIFORequestChannel chan)
{
	MESSAGE_TYPE qm = QUIT_MSG;
    char *new_cmsg_address  = (char*)(&qm);
    chan.cwrite(new_cmsg_address,sizeof(qm));

}




int main(int argc, char *argv[]){
    
    int child_status;
    char *cmd_server = "./dataserver";
    char *child[] = {"./dataserver",NULL};
    int ret = fork();
    if(ret == 0)
    {

    	execvp(cmd_server,child);
    }
    else
    {

    int n = 100;    // default number of requests per "patient"
	int p = 15;		// number of patients
    srand(time_t(NULL));
    FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);

    cout<<"Enter option: "<<endl;
    cout<<"1 - Request data points "<<endl;
    cout<<"2 - Request Text File "<<endl;
    cout<<"3 - Request Binary file "<<endl;
    cout<<"4 - Request New channel creation "<<endl;
    int ch;
    cin>>ch;
    switch(ch)
    {
    	case 1:Request_data_points(chan);
    	       break;
    	case 2:Request_text_file(chan);
    	        break;
    	case 3:Request_binary_file(chan);
    	       break;
    	case 4:Request_new_channel(chan);
    	       break;

    	default: Send_quit_msg(chan);
    	        // wait(&child_status);
    	         break;
    }

    wait(0);
    }

	return 0;
}
