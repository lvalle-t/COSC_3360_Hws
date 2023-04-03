
//server.cpp

#include <pthread.h>    // the first 3 is for the use of pthreads
#include <stdio.h>      // for stream/buffered i/o like fprint()
#include <unistd.h>     // read()

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>          // strcutures needed for sockets
#include <netinet/in.h>         // structures needed for interent domain address
#include <netdb.h> 
#include <sys/wait.h>

#include <string>
#include <utility>          // to use for the pairs 
#include <deque>

#include <iostream>
using namespace std;


/////REMINDER:::///////////////////
//////////// code was dowloaded from black board. the client/sever .c / used as reference/template //////////////
/////////// code for fireman was also dowloaded from black board 
////////////////////////// code for the rabinkarp was copied from geeks for geeks \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\


void fireman(int) {
    while(waitpid(-1, NULL, WNOHANG) > 0){
    }
}

string rabinKarp(string text,string pattern) { 
 
   // int pm = 256;
    
    int n = text.length();          // findind teh length of both the text and the pattern 
    int m = pattern.length();
    int i, j;                                // iterators for loops
    int s = 0, p = 0;                        // s = hash of string, p = hash of pattern
    const int q = 101;                      // large prime number
    int h = 1;                               // h = multiplier for MSB
    bool flag = false;
    std::string location;
    std::string toReturn = "";                   // final string that will return and be used to send to server 

    for (i = 0; i < m-1; i++){               // the value of h would be eqv to pow(pm, m-1)
        h = (h * 256) % q;
    }

    for (i = 0; i < m; i++) {                   // calculating initial hash for string and pattern
        s = (256 * s + text[i]) % q;     //finding number of text and pattern to compaare and find the locations
        p = (256 * p + pattern[i]) % q;
    }

    for (i = 0; i <= n-m; i++) {
        if (s == p) {                       // comparing hash of string and pattern
            for (j = 0; j < m; j++){     
                if (text[i+j] != pattern[j]){  //if cannot find pattern in the text it will be break 
                    break;
                }
            }

            if (j == m) {                       // the locations will be added to a string in order to send to the client 
                location= to_string(i);         // for every location it will also add a blank space in between them 
                toReturn += location + " ";
                flag = true;                    
            }
        }

        s = (256 * (s - h * text[i]) + text[i+m]) % q;       //calculating the value of the next pattern 
        if (s < 0){                                                     // in case the value is neg
            s = s + q;
        }
    }
    
    if(!flag){                                                  // in case the pattern is unable to be found in the text 
        string nope = "";                                       // an empty string will be added, that way in the final loop, it can check if it has any locations and print "not found"
        toReturn += nope + " ";
        
    }
    return toReturn;            //return finalstring
}


int main(int argc, char *argv[])
{
    signal(SIGCHLD, fireman);       // calling fireman
        
    int sockfd, newsockfd, portno, clilen;                  //the first 2 are file descriptors
    
    std::string text;
    getline(std::cin, text);        // get the text that will be working with 

    struct sockaddr_in serv_addr, cli_addr;
    int n;
    if (argc < 2)
    {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);               //creating a socket
    if (sockfd < 0){
        perror("ERROR opening socket");
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));           // bzero server address  //erases the data in the n bytes of the memory starting at the location pointed to by s, by writing zeros
    portno = atoi(argv[1]);                                 // port number that will be received by teh command line
    serv_addr.sin_family = AF_INET;                         // allow possible network protocols or address family 
    serv_addr.sin_addr.s_addr = INADDR_ANY;                 // will be able to recieve any address that is available to the user
    serv_addr.sin_port = htons(portno);                     // assign port number // gurantees data will be represented as an int

    if (bind(sockfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){
        perror("ERROR on binding");
    }
    
    if(listen(sockfd,SOMAXCONN) < 0){               // waiting for caller/ client
        perror("DID NOT LISTEN"); 
    }
    
    clilen = sizeof(cli_addr);                      //clilen stores the size of the address of the client.  
    
    while(true) //inifnite loop, will keep listening to sockets
    { 
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, (socklen_t *)&clilen);     // receiving request
    
        if (newsockfd < 0){
            perror("ERROR on accept");
        }
        
        // handling each incoming reuqest in its own process
        char buffer[256];
        if (fork() == 0){
            
            //reading from client
            bzero(buffer, 256);
            n = read(newsockfd, buffer, 256);                  
            if (n < 0){
                perror("ERROR reading from socket");        //checking if reading is possible 
            }
            
            string pattern = buffer;                        // getting the pattern that was sent from the client 
            string toClient = rabinKarp(text, pattern);    // calling the function to get the locations
          
            //writing to client
            strcpy(buffer,toClient.c_str());                // copying the locations to the buffer
            n = write(newsockfd, buffer, 256);  
            if (n < 0){
                perror("ERROR writing to socket");          //checking if writing to socket is possible 
            }
            
            close(newsockfd);                       //closing socket 
            _exit(0);
      
        }
    }
    
   close(sockfd);
   return 0;
}

