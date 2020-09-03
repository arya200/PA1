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
using namespace std;


int main(int argc, char *argv[])
{
    struct timeval start, end;
    gettimeofday(&start, NULL);
    FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);
    datamsg d = datamsg(15, 0.008, 2);
    int option;
    string filename = "";
    bool singlepoint = false;
    bool multiple_points = false;
    //d.person = 15, d.ecgno=1, d.seconds=0.008;
    //cout << argc << endl;

    while((option = getopt(argc, argv, "p:t:e:f:"))!=-1)
    {
        switch(option)
        {
            case 'p':
                    d.person = stoi(optarg);
                    multiple_points = true;
                    break;
            case 't':
                    d.seconds = stod(optarg);
                    singlepoint = true;
                    break;

            case 'e':
                   d.ecgno = stoi(optarg);
                    break;
            case 'f':
                    filename = optarg;

        }
    }

    if(singlepoint && multiple_points)
    {
        chan.cwrite(&d, sizeof(datamsg));
        double result;
        chan.cread(&result, sizeof(double));
        cout << "The result is:" << result << endl;
    
    }
    if(multiple_points)
    {
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
        //cout << "I enter here" << endl;
        //int windowsize=30;
        filemsg f(0,0);
        char buf[sizeof(filemsg) + filename.size()+1];
        memcpy(buf, &f, sizeof(filemsg));
        //memcpy (buf + sizeof(filemsg), filename.c_str(), filename.size() + 1);
        strcpy(buf + sizeof(filemsg), filename.c_str());
        chan.cwrite(buf, sizeof(buf));

        __int64_t filelen;
        chan.cread (&filelen, sizeof(__int64_t));
        //cout << filelen << endl;

        int count = ceil((double)filelen/256);
        cout << "The ceil value is: " << count  << endl;
        int offset_val = 0;
        int length = 256;
        char recvbuf[filelen];
        while(count>0)
        {
            
            if(offset_val + length > filelen)
            {
                length = filelen - offset_val;
               
            }
             cout  << "The length is:"  << length << endl;
             cout << "the offset val is:"  << offset_val << endl;
             cout << "the count is:" << count  << endl;
            filemsg f1(offset_val, length);
            char buf1[sizeof(filemsg) + filename.size()+1];
            memcpy(buf1, &f1, sizeof(filemsg));
            //memcpy (buf + sizeof(filemsg), filename.c_str(), filename.size() + 1);
            strcpy(buf1 + sizeof(filemsg), filename.c_str());
            chan.cwrite(buf1, sizeof(buf1));
            chan.cread(recvbuf, sizeof(recvbuf));
            offset_val = offset_val + 256;
            count--;

        }
        file.write(recvbuf, filelen);
        file.close(); 

    }

    // closing the channel    
    MESSAGE_TYPE m = QUIT_MSG;
    chan.cwrite (&m, sizeof (MESSAGE_TYPE));
}
