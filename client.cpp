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
    FIFORequestChannel chan ("control", FIFORequestChannel::CLIENT_SIDE);
    datamsg d = datamsg(0, 0, 0);
    int option;
    string filename = "";
    bool single = false;
    //d.person = 15, d.ecgno=1, d.seconds=0.008;
    //cout << argc << endl;

    while((option = getopt(argc, argv, "p:t:e:f:"))!=-1)
    {
        switch(option)
        {
            case 'p':
                    d.person = stoi(optarg);
                    break;
            case 't':
                    d.seconds = stod(optarg);
                    single = true;
                    break;

            case 'e':
                   d.ecgno = stoi(optarg);
                    break;
            case 'f':
                    filename = optarg;

        }
    }

    if(single)
    {
        chan.cwrite(&d, sizeof(datamsg));
        double result;
        chan.cread(&result, sizeof(double));
        cout << "The result is:" << result << endl;
    
    }
    else
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
        
    }
    

    if(true)
    {
        cout << "I make it here"
         = "15.csv";
        //int windowsize=;
        filemsg f(0,0);
        char buf[sizeof(filemsg) + filename.size()+1];
        memcpy(buf, &f, sizeof(filemsg));
        //memcpy (buf + sizeof(filemsg), filename.c_str(), filename.size() + 1);
        strcpy(buf + sizeof(buf), filename.c_str());
        chan.cwrite(buf, sizeof(buf));

        __int64_t filelen;
        chan.cread (&filelen, sizeof(__int64_t));
        cout << filelen << endl;
        // char recvbuf[windowsize];
        // chan.cread(recvbuf, sizeof(recvbuf));
        // cout << recvbuf << endl;


       
    }






    // sending a non-   sense message, you need to change this
    // char buf [MAX_MESSAGE];
    // char x = 55;
    // chan.cwrite (&x, sizeof (x));
    // int nbytes = chan.cread (buf, MAX_MESSAGE);
    // closing the channel    
    MESSAGE_TYPE m = QUIT_MSG;
    chan.cwrite (&m, sizeof (MESSAGE_TYPE));
}
