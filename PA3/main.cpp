#include <pthread.h>    // the first 3 is for the use of pthreads
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <utility>          // to use for the pairs 
#include <semaphore.h> //need this for semaphores
#include <queue>
#include <iostream>

using namespace std;


struct thread_info{
    string* text;        // the text that wil be used to find the pattern 
    string pattern;     // the pattern you are looking for in the text     
    //std::vector <std::pair<std::string,int>> vec;  // vector of pairs  
    //int num; //nthread
    pthread_mutex_t* nmutex; //PA3
    pthread_cond_t *nt;     //PA3
    int* parIndex = 0;          //PA3
    int index = 0;            //PA3
};


////////////////////////// code for the rabinkarp was copied from geeks for geeks \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

//must print info in the chld thread -professor
//the order of printing the patterns and locations 
//

void* rabinKarp(void* first) { 
    thread_info* base = (struct thread_info* ) first;
    int localID = base->index;
    
    int pm = 256;
    
    int n = base -> text->length();          // findind teh length of both teh text and the pattern 
    int m = base -> pattern.length();
    int i, j;                                // iterators for loops
    int s = 0, p = 0;                        // s = hash of string, p = hash of pattern
    const int q = 101;                      // large prime number
    int h = 1;                               // h = multiplier for MSB
    bool flag = false;

    string sentence = *base->text;              //creating a variables that represents the pointers 
    string style = base->pattern;
    deque <pair<string,int>> outcome;

    pthread_mutex_unlock(base->nmutex);     //unlock
    
    for (i = 0; i < m-1; i++){               // the value of h would be eqv to pow(pm, m-1)
        h = (h * pm) % q;
    }

    for (i = 0; i < m; i++) {                   // calculating initial hash for string and pattern
        s = (pm * s + sentence[i]) % q;     //finding number of text and pattern to compaare and find the locations
        p = (pm * p + style[i]) % q;
    }

    for (i = 0; i <= n-m; i++) {
        if (s == p) {                       // comparing hash of string and pattern
            for (j = 0; j < m; j++){     
                if (sentence[i+j] != style[j]){  //if cannot find pattern in the text it will be break 
                    break;
                }
            }

            if (j == m) {
               outcome.push_back(make_pair(style,i)); //push the pair into the queue// the pair includes(pattern, location)
        
                flag = true;
               
            }
        }

        s = (pm * (s - h * sentence[i]) + sentence[i+m]) % q;       //calculating the value of the next pattern 
        if (s < 0){                                                     // in case the value is neg
            s = s + q;
        }
    }
    
    if(!flag){                                                  // in case the pattern is unable to be found in the text 
        int nope = -1;                                          // the pair will be a fixed statement(pattern, nope)
        outcome.push_back(std::make_pair(style,nope));     // nope could of easily be any number, as long that is less that 0( 0 represents basically the smallest whole number for the location of vectors) 

    }
    
    
    pthread_mutex_lock(base->nmutex);
    while((*base->parIndex) != localID){
        pthread_cond_wait(base->nt, base->nmutex);      //Wait for Condition
    }
    
    pthread_mutex_unlock(base->nmutex);
    
    // print here
    
    for(int i = 0; i < outcome.size(); i++)
    {
        std::cout <<  "Pattern \"" << style << "\"" ; 
        if(outcome[i].second >= 0){
            std::cout << " in the input text at position " << outcome[i].second<< std::endl;
        }
        else{
            std::cout << " not found" << std::endl;
        }
    }
    std::cout << "\n";

    pthread_mutex_lock(base->nmutex);
    (*base->parIndex) =  (*base->parIndex) + 1;
    pthread_cond_broadcast(base->nt);           
    pthread_mutex_unlock(base->nmutex);
    
    /*
    //pthread_cond_broadcast() -function wakes up all threads that are currently waiting on the condition variable specified by cond. 
    //If no threads are currently blocked on the condition variable, this call has no effect.
    */
    return NULL; 
}



int main(){
    
    int input;
    string message; // message that will be analyzed
    //int enter;   // the number that will be inputted to determine how many pattern it will look in the message
    std::string phrase;

    getline(std::cin, message);       /// getting the string message
    cin >> input;        //getting the number of patterns that will be found 
    
    pthread_t* n_thread = new pthread_t[input];       // will create  certain number of pthreads depending on the number inputted
    
    static pthread_mutex_t nthreads;                    //creating/stablishing semaphores
    static pthread_cond_t turn;
    thread_info child;      //struct that holds info for the nthreads
    
    deque <string> first;                    // text that was read will be added to the deque
    for(int i = 0; i< input; i++){  // reading the inputs to the queue of what to find, pattern
        cin >> phrase;
        first.push_back(phrase);
        
    }
    
    child.nmutex =&nthreads;        /// connecting the variables to the struct 
    child.nt = &turn;
    int parIdx = 0;
    child.parIndex = &parIdx;
    //child.num = input; // 
    child.text = &message;          //constant in struct

    std::cout << "SEARCH RESULTS:\n" << std::endl;        //results will be printed in each child thread 

    for(int i =0; i < input; i++){                             
        pthread_mutex_lock(&nthreads);                          // semaphore will be used for each pattern 
        child.index = i;
        child.pattern = first[i];                               //sending pattern to struct 
        pthread_create(&n_thread[i], NULL, rabinKarp, &child);   // creating pthreads that will be used
    }

    for(int i = 0; i< input; i++){
        pthread_join(n_thread[i],NULL);
    } 
    

    return 0;
}
