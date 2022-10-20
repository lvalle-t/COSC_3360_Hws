#include <pthread.h>    // the first 3 is for the use of pthreads
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <iostream>
#include <string>
#include <utility>          // to use for the pairs 
//using namespace std;

//#define pm 256                                      // no of characters in alphabet

struct args{
    std::string text;        // the text that wil be used to find the pattern 
    std::string pattern;     // the pattern you are looking for in the text     
    std::vector <std::pair<std::string,int>> vec;  // vector of pairs  
};

////////////////////////// code for the rabinkarp was copied from geeks for geeks \\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\

void* rabinKarp(void* first) { 
    struct args* base = (args* ) first;
    int pm = 256;
    
    int n = base -> text.length();          // findind teh length of both teh text and the pattern 
    int m = base -> pattern.length();
    int i, j;                                // iterators for loops
    int s = 0, p = 0;                        // s = hash of string, p = hash of pattern
    const int q = 101;                      // large prime number
    int h = 1;                               // h = multiplier for MSB
    bool flag = false;

    for (i = 0; i < m-1; i++){               // the value of h would be eqv to pow(pm, m-1)
        h = (h * pm) % q;
    }

    for (i = 0; i < m; i++) {                   // calculating initial hash for string and pattern
        s = (pm * s + base -> text[i]) % q;     //finding number of text and pattern to compaare and find the locations
        p = (pm * p + base -> pattern[i]) % q;
    }

    for (i = 0; i <= n-m; i++) {
        if (s == p) {                       // comparing hash of string and pattern
            for (j = 0; j < m; j++){     
                if (base -> text[i+j] != base -> pattern[j]){  //if cannot find pattern in the text it will be break 
                    break;
                }
            }

            if (j == m) {
                base->vec.push_back(std::make_pair(base->pattern,i)); //push the pair into the vector// the pair includes(pattern, location)
                flag = true;
            }
        }

        s = (pm * (s - h * base->text[i]) + base->text[i+m]) % q;       //calculating the value of the next pattern 
        if (s < 0){                                                     // in case the value is neg
            s = s + q;
        }
    }
    
    if(!flag){                                                  // in case the pattern is unable to be found in the text 
        int nope = -1;                                          // the pair will be a fixed statement(pattern, nope)
        base->vec.push_back(std::make_pair(base->pattern,nope));     // nope could of easily be any number, as long that is less that 0( 0 represents basically the smallest whole number for the location of vectors) 
    }

    return NULL; 
}

int main(){
    
    int input = 0;
    std::string message; // message that will be analyzed
    std::string enter;   // the number that will be inputted to determine how many pattern it will look in the message
    std::string phrase;

    getline(std::cin, message);       /// getting the string message
    getline(std::cin, enter);        //getting the number of patterns that will be found /// professor said can use just use cin here

    input = std::stoi(enter);        // string number inputted into integer   
    pthread_t tid[input];       // will create  certain number of pthreads depending on the number inputted
    
    struct args arg[input];    

    std::string first[input];        // patterns inputted into an array
    for(int i = 0; i< input; i++){  // reading the inputs to the array of what to find, pattern
        std::cin >> phrase;  //professor said get line will be recommened to be implemented to be used here
        first[i] = phrase;
    }

    for(int i = 0; i < input; i++){
        arg[i].pattern = first[i]; // each pattern and text  will be added to the struct to be used
        arg[i].text = message;   
    }

    for(int i =0; i < input; i++){                              // creating pthreads that will be used
        pthread_create(&tid[i], NULL, rabinKarp, &arg[i]);
    }

    for(int i = 0; i< input; i++){
        pthread_join(tid[i],NULL);
    }

    // printing final answer here 
    std::cout << "SEARCH RESULTS:\n" << std::endl;                    
    for(int i = 0; i < input; i++){                    // will loop by the number of patterns that there are                    
        for(int j = 0; j <arg[i].vec.size(); j++){   /// print the vector pairs for each pattern 
            std::cout << "Pattern \"" << arg[i].pattern << "\"" ;   // the pattern 
            //check if a pattern was found in the message 
            if (arg[i].vec[j].second >= 0){
                std::cout << " in the input text at position " << arg[i].vec[j].second<<std::endl;    // if pattern was found the second part of vector of pairs will be printed
            }
            else{
                std::cout << " not found" << std::endl;
            }
        }

        std::cout << std::endl;
    }
    

    return 0;
}
