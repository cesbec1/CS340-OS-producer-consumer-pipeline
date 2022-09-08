# producer-consumer-pipeline
A producer consumer problem in C

This program is a solution to creating a pipeline with threads in C. Each thread is either a consumer,producer or both and the main task of this application is to read a text file and return some statistics to the console. In order for this program to be successful thread synchronization needed to take place and with some techniques such as mutex or thread scheduling synchronization is achieved. 

The flow of this program 

-A command line argument is passed in, in this case a text file, to be passed to Thread1

-Thread1(producer) begins reading and storing into a global buffer whole lines while thread2 waits until the buffer is full

-Thread2(consumer/producer) begins to consume the lines in the global buffer to start reading a word a time to be stored into another global buffer for thread3 to consume.

-Thread3's(consumer) only function is to consume the words that were read and stored by thread2 and print them out to the console

Sample output that was produced 


![Screenshot 2022-01-25 003255](https://user-images.githubusercontent.com/76544232/150917163-620ada62-9ace-4a7b-8664-5f281ed520a5.png)
