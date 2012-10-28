#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <sys/wait.h>   /* Wait for Process Termination */
#include <stdlib.h>     /* General Utilities */
#include <iostream>
#include <string>
using namespace std;
 
int main(int argc, char* argv[])
{  
	string URL = argv[argc-2];
	cout << URL << endl;
	system("mkdir temp");

	loop(./temp,);
	
}

int loop(DIR dir, int max, int current, String watermark){
	while(/*loop through dir*/){
		if(/*is dir*/ && current + 1 < max){
			loop(/*dir name*/,max,current++,watermark);
		}else if(/*is image*/){
			watermark(/*image*/,watermark);
		}
	}
}

int wm(String file, String watermark){
	//watermark
}