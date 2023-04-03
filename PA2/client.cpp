//client.cpp

#include <pthread.h>    // the first 3 is for the use of pthreads
#include <stdio.h>      // for stream/buffered i/o like fprint()
#include <unistd.h>     // read()

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>     // strcutures needed for sockets
#include <netinet/in.h>     // structures needed for interent domain address
#include <netdb.h> 

#include <iostream>
#include <deque>

using namespace std;

/////REMINDER:::///////////////////
//////////// code was dowloaded from black board. The client/sever .c / used as reference/template //////////////
///////


struct args{
    std::string pattern;        // the pattern you are looking for in the text  
    char* hostD;
    int port_num;
    std::deque <std::pair<std::string,string>> outcome;                 // deque of pairs 
    
    
};

void* socket_func (void* pthreads_socket){                              //pthreads and sockets
    struct args *base = (struct args *) pthreads_socket;
    struct sockaddr_in serv_addr;

    struct hostent *server = gethostbyname(base->hostD);
    char buffer[256];
    
    if(server == NULL){                                                           
        fprintf(stderr,"ERROR, no such host\n");                    //checking if host exists
        exit(0);
    }
    
    // initializing socket for each thread
    int sockfd = socket(AF_INET,SOCK_STREAM,0);                         
    if (sockfd < 0){
        perror("ERROR opening socket");
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));                          // bzero server address  //erases the data in the n bytes of the memory starting at the location pointed to by s, by writing zeros
    serv_addr.sin_family = AF_INET;                                         // since the family is all teh same for the socket it will ve set to af_inet
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(base->port_num);
     // copies info from 1st paramter to 2nd, will have info on address(1st para) -> copy char array that represents the  address(2nd para)
               // will all depend on the length on how the parameter will be adajusted/ copied 
                // assign port number 
                // gurantees data will be represented as an int

    if (connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){ 
        perror("ERROR connecting");     // connection with server begins
    }

    ///writing to server
    strcpy(buffer,base->pattern.c_str());                                // send here the pattern to the server
    if(write(sockfd,buffer,256)<0){
        
        perror("ERROR writing pattern to socket\n");
    }

    /// read form the server 
    if(read(sockfd, buffer, 255) < 0){
        perror("ERROR reading from server");                            // check if there is an error readin from the server
    }
    else{
        // will split the string that was received with the locations into individual string
        
        std::string locations_split = buffer;
        std::string temp = "";
	    for(int i=0;i<locations_split.length();++i){
		    if(locations_split[i]==' '){
			    base->outcome.push_back(std::make_pair(base->pattern,temp));        //pushing pattern and location to a pair every time it splits
			    temp = "";
		    }
		    else{
			    temp.push_back(locations_split[i]);
		    }
		
    	}
        
    }
    
    close(sockfd);
    return NULL;
}



int main(int argc, char *argv[])
{
    int sockfd, portno, n;      //sockfd is a file descriptor
    struct sockaddr_in serv_addr;
    struct hostent *server;

    char* host;
    host = argv[1];
    portno = atoi(argv[2]);             //server will listen for connections is passed in as an argument
    
    
    int count=0;
    std::string phrase;
    
    deque <string> text;                    // text that was read will be added to the deque 
    
    while (getline(cin, phrase)){           // get phrase patterns 
        text.push_back(phrase);
        count++;                            // counts while it loops to determine number of pthreads that will be created
    }
    
    struct args arg[count];                     // strcut is created
    
    for(int i = 0; i < count; i++){
        arg[i].pattern = text[i]; // each pattern and text  will be added to the struct to be used
        arg[i].port_num = portno;
        arg[i].hostD = host;
        
    }

    pthread_t tid[count];       // will create  certain number of pthreads depending on the number inputted
    
    for(int i =0; i < count; i++){                              // creating pthreads that will be used
        if(pthread_create(&tid[i], NULL, socket_func, &arg[i])){
            fprintf(stderr, "Error creating thread\n");             //checking for errors
            return 1;
        }
    }

    for(int i = 0; i< count; i++){
        pthread_join(tid[i],NULL);
    }
    
    std::cout << "SEARCH RESULTS:\n" << std::endl; 
    for(int i = 0; i < count; i++){                         // will loop by the number of patterns that there are  
        for(int j = 0; j <arg[i].outcome.size(); j++){   /// print the vector pairs for each pattern 
            std::cout << "Pattern \"" << arg[i].pattern << "\"" ;   // the pattern 
            
            //check if a pattern was found in the message 
            if (!arg[i].outcome[j].second.empty()){
                std::cout << " in the input text at position " << arg[i].outcome[j].second;  // if pattern was found the second part of vector of pairs will be printed
            }
            
            else{
                std::cout << " not found";
            }
            
            std::cout <<std::endl;
            
        }
        std::cout << std::endl;       // will add space between the patterns when it finished it locations 
    }
    
    close(sockfd);
    return 0;
}
