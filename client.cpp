/*
    Tanzir Ahmed
    Department of Computer Science & Engineering
    Texas A&M University
    Date  : 2/8/20
 */
#include "common.h"
#include "FIFOreqchannel.h"
//#include "server.h";
#include <getopt.h>
#include <sys/wait.h> // for wait() 
using namespace std;


int main(int argc, char *argv[])
{
    struct timeval start, end;
    int option;
    string filename = "";
    int singlepoint = 0;
    char* chararg[1] = {NULL};
    bool new_channel = false;
    datamsg d = datamsg(15, 0.008, 2);
    while((option = getopt(argc, argv, "p:t:e:f:m:c"))!=-1)
    {
        switch(option)
        {
            case 'p':
                    d.person = stoi(optarg);
                    //cout << "I enter here1"<< endl;
                    singlepoint++;
                    break;
                    
            case 't':
                    d.seconds = stod(optarg);
                    singlepoint++;
                    //cout << "I enter here2"<< endl;
                    break;

            case 'e':
                    d.ecgno = stoi(optarg);
                    singlepoint++;
                    //cout << "I enter here2"<< endl;
                    break;
            case 'f':
                    filename = optarg;
                    break;
            case 'c':
                    new_channel = true;
                    break;
            case 'm':
                    chararg[0] = optarg;
            default:
                    break;

        }
    }
    
    
    int child_status;
    int identity = fork();
    if(identity==0)
    {
        cout << "chararg is:" << chararg[0]<< endl;
        execvp("./server", chararg);
    }
    else
    {
        gettimeofday(&start, NULL);
        FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);
        //cout << "singlepoint is:"  << singlepoint<<endl;
        if(singlepoint ==3)
        {   
              
            chan.cwrite(&d, sizeof(datamsg));
            double result;
            chan.cread(&result, sizeof(double));
            cout << "The result is:" << result << endl;

        }
        if(singlepoint==2)
        {
            //cout << "I enter here" << endl;
            ofstream file("received/x1.csv");
            double seconds = 0;
            int i=0;
            while(i<1000)
            {
                d.seconds = seconds;
                chan.cwrite(&d, sizeof(datamsg));
                double result;
                chan.cread(&result, sizeof(double));
                file  << d.seconds << "," << result << endl;
                seconds = seconds + 0.004;
                i++;
                
            }
            file.close();
            gettimeofday(&end, NULL);
            double time_taken; 
        
            time_taken = (end.tv_sec - start.tv_sec) * 1e6; 
            time_taken = (time_taken + (end.tv_usec - start.tv_usec)) * 1e-6; 
        
            cout << "Time taken by program is : " << fixed << time_taken << setprecision(6); 
            cout << " sec" << endl; 
        }
        
        //filename = "100.dat";
        if(filename.size()>1)
        {
            ofstream file("received/" + filename);
            filemsg f(0,0);
            char buf[sizeof(filemsg) + filename.size()+1];
            memcpy(buf, &f, sizeof(filemsg));
            //memcpy (buf + sizeof(filemsg), filename.c_str(), filename.size() + 1);
            strcpy(buf + sizeof(filemsg), filename.c_str());
            chan.cwrite(buf, sizeof(buf));

            __int64_t filelen;
            chan.cread (&filelen, sizeof(__int64_t));
            

            int count = ceil((double)filelen/256);
            cout << "The ceil value is: " << count  << endl;
            int offset_val = 0;
            int length = 256;
            char recvbuf[256];
            while(count>0)
            {
                
                if(offset_val + length > filelen)
                {
                    length = filelen - offset_val;
                
                }
                // cout  << "The length is:"  << length << endl;
                // cout << "the offset val is:"  << offset_val << endl;
                // cout << "the count is:" << count  << endl;
                filemsg f1(offset_val, length);
                char buf1[sizeof(filemsg) + filename.size()+1];
                memcpy(buf1, &f1, sizeof(filemsg));
                //memcpy (buf + sizeof(filemsg), filename.c_str(), filename.size() + 1);
                strcpy(buf1 + sizeof(filemsg), filename.c_str());
                chan.cwrite(buf1, sizeof(buf1));
                chan.cread(recvbuf, sizeof(recvbuf));
                offset_val = offset_val + 256;
                file.write(recvbuf, length);
                count--;

            }
            
            file.close();
            gettimeofday(&end, NULL);
            double time_taken; 
            time_taken = (end.tv_sec - start.tv_sec) * 1e6; 
            time_taken = (time_taken + (end.tv_usec - start.tv_usec)) * 1e-6; 
            cout << "Time taken by program is : " << fixed << time_taken << setprecision(6); 
            cout << " sec" << endl; 

        }

        if(new_channel)
        {
            
            MESSAGE_TYPE m = NEWCHANNEL_MSG;
            chan.cwrite(&m, sizeof(MESSAGE_TYPE));
            char name[100];
            chan.cread(name, sizeof(name));
            //cout << "The name is: " << name << endl;
            FIFORequestChannel chan1 (name, FIFORequestChannel::CLIENT_SIDE);
            chan1.cwrite(&d, sizeof(datamsg));
            double result;
            chan1.cread(&result, sizeof(double));
            cout << "The result is:" << result << endl;
            MESSAGE_TYPE quit = QUIT_MSG;
            chan1.cwrite(&quit, sizeof(MESSAGE_TYPE));
            chan.cwrite(&quit, sizeof(MESSAGE_TYPE));

        }



        // closing the channel    
       
        wait(&child_status);
        
    }
}
