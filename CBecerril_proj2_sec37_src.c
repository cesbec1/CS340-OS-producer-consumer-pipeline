//Cesar Becerril
//gcc CBecerril_proj2_sec37_src.c -o CBecerril_proj2_sec37_src.exe
#include <pthread.h>
#include <sched.h>
#include <unistd.h>
#include <fcntl.h>
#define BUFFERSIZE 10
#define BUFFERSIZE2 20
void tostring(char [], int );
//variables between thread 1 & 2
typedef struct {
    char singleLine[256];
    int length;
}line;
line buffer[BUFFERSIZE];
int in = 0;
int out = 0;
int count = 0;
int flag = 1;
int lineCount = 0;
//variables between thread 2 & 3
typedef struct{
    char singleWord[100];
    int size;
} word;
word buffer2[BUFFERSIZE2];
int in2 = 0;
int out2 = 0;
int count2 = 0;
int flag2 = 1;
int wordCount = 0;
//thread 1 function
void* tOneFunc(void * arg){
    char threadOne[] = "thread one function \n";
    write(STDOUT_FILENO, threadOne, sizeof(threadOne));
    //assinging a filedescriptor the passed argument to read
    //declaring all variables to keep track of the contents being read 
    line nextProduced;
    int *fileDes = (int * ) arg; 
    char writeBuff[200];
    char buff[1];
    int writing;
    int bufferCounter = 0;
        //producer code modified to run while there is something to be read from the filedescriptor
        while((writing = read(*fileDes, buff, 1)) > 0 ) {
            if(buff[0] == '\n'){
                //copying the contents to the line struct from the thread local buffer and assigning the length of the line
                int p = 0;
                while(p != bufferCounter){
                    nextProduced.singleLine[p] = writeBuff[p];
                    p++;
                }
                nextProduced.length = bufferCounter;
                //case where it has to pass control because the buffer is full and the consumer needs to consume
                while(((in+1)%BUFFERSIZE)==out){ 
                    sched_yield();
                }
                //these next lines allow one thread to access the globlal variables one at time so that they dont both access at the same time 
                in = (in + 1) % BUFFERSIZE;
                buffer[in] = nextProduced;
                count++;
                lineCount++;
                //clearing the local char array so that it can be reused
                for(int i = 0; i < 200 ; i++){
                    writeBuff[i] = 0 ;
                }
                bufferCounter = 0;
            }else{
                writeBuff[bufferCounter] = buff[0];
                bufferCounter++;
            } 
        }   
    //the signal used to say that the thread is done executing its contents
    if(writing == 0){
        flag = 0;
    }
    close(*fileDes);
    pthread_exit(0);
}

//thread two function
void* tTwoFunc(void * arg){
    char threadTwo[] = "thread two function \n";
    write(STDOUT_FILENO, threadTwo, sizeof(threadTwo));
    //allowing 1 sec for thread 1 to progress before this thread begins to consume
    sleep(1);
    line nextConsumed;
    word nextProduced;
    int wordBufferCounter = 0;
    char word[50];
    //consumer code, runs while there is somthing in the array or thread1 is still running
    while (flag || count!=0) {
        while (in == out ){
            sched_yield();
            
        }
        //getting something to be consumed from the shared array between thread 1 and 2
        out = (out + 1) % BUFFERSIZE;
        nextConsumed = buffer[out];
            int j = 0;
            //the start of producer code that will work with thread 3, it runs while there j does not reach the length of the line that was consumer before
            while(j != nextConsumed.length){
                if(nextConsumed.singleLine[j] == ' ' || nextConsumed.singleLine[j] == '.' || nextConsumed.singleLine[j]==','){
                        if(word[0] != 0){
                        int p= 0;
                        //copying contents to word struct from local word buffer to be stored
                        while(p != wordBufferCounter){
                            nextProduced.singleWord[p]=word[p];
                            p++;
                        }
                        nextProduced.size = wordBufferCounter;
                        wordCount++;
                        while(((in2 +1) % BUFFERSIZE2)==out2){ 
                            sched_yield();
                         }
                        //here as the producer for thread 3 it is adding into the shared array the word that was parsed aboved 
                        in2 = (in2 + 1) % BUFFERSIZE2;
                        buffer2[in2] = nextProduced;
                        count2++;
                        //clearing the buffers to be reused
                        for(int k = 0; k < 50 ; k++){
                            nextProduced.singleWord[k] = 0 ;
                        }
                        for(int k = 0; k < 50 ; k++){
                            word[k] = 0 ;
                        }
                         wordBufferCounter = 0;
                         
                        }                 
                }else{
                    word[wordBufferCounter] = nextConsumed.singleLine[j];
                    wordBufferCounter++;                
                }
                j++;
            }
        count--;
    }
    //signal for thread 3 to know if thread 2 is done running 
    flag2 = 0;
    pthread_exit(0);
}

//thread 3 function 
void* tThreeFunc(void * arg){
    char threadThree[] = "thread three function \n";
    write(STDOUT_FILENO, threadThree, sizeof(threadThree));
    word nextConsumed;
    //allowing some time for thread 2 to begin working 
    sleep(1);
    //checking to see if the contents of the array have all been read or if thread 2 is still working 
    while (flag2 ||count2 != 0) { 
        while (in2 == out2){
            sched_yield();
            
        }
        //getting content to consume from the shared buffer array between 2 and 3
        out2 = (out2 + 1) % BUFFERSIZE2;
        nextConsumed = buffer2[out2];
        count2--;
        //consuming the word and printing it out the format specified 
         write(STDOUT_FILENO, "*", 1);
         write(STDOUT_FILENO, nextConsumed.singleWord, nextConsumed.size); 
         write(STDOUT_FILENO, "*\n", 2);
    }
    //printing final statistics here
    char str[10];
    char str2[10];
    tostring(str,lineCount);
    tostring(str2,wordCount);
    write(STDOUT_FILENO, "LINECOUNT: ", 12);
    write(STDOUT_FILENO, str, 10);
    write(STDOUT_FILENO, "\nWORDCOUNT: ", 12);
    write(STDOUT_FILENO, str2, 10);
    
    pthread_exit(0);
}

int main( int argc, char* argv[] ) {
    //creating 3 threads to mutext so that data from shared buffer array can be accessed by one at a time 
    pthread_t pthread1;
    pthread_t pthread2;
    pthread_t pthread3;
   
    
    int fileDescriptor;
    
    char opening[] = "opening the file to be pipelined \n";
    write(STDOUT_FILENO, opening, sizeof(opening));

    if((fileDescriptor = open( argv[1], O_RDONLY, 0 )) == -1){
        char opErr[] = "can't open file ";
        write(STDOUT_FILENO, opErr, sizeof(opErr));
        int length = 0;
        while(argv[1][length] != '\0'){
            length++;
        }
        write(STDOUT_FILENO, argv[1], length);
        return 2;
    }

    //creating the threas and passing the file opened to be used by them 
    pthread_create(&pthread1,NULL,tOneFunc,&fileDescriptor);
    pthread_create(&pthread2,NULL,tTwoFunc,NULL);
    pthread_create(&pthread3,NULL,tThreeFunc,NULL);
    
    //joining the threads 
    pthread_join(pthread1, NULL);
    pthread_join(pthread2, NULL);
    pthread_join(pthread3, NULL);
   
    
    close(fileDescriptor);
     
    char finished[] = "\nend of the pipeline \n";
    write(STDOUT_FILENO, finished, sizeof(finished));
    return 0;
}
//function to convert int to a string 
void tostring(char str[], int num){
    int i, rem, len= 0, n;

    n = num;
    while(n != 0){
        len++;
        n/=10;
    }
    for(i = 0; i< len; i++){
        rem = num%10;
        num = num/10;
        str[len - (i+1) ] = rem + '0';
    }
    str[len] = '\0';
    
}
