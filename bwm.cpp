#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <sys/wait.h>   /* Wait for Process Termination */
#include <stdlib.h>     /* General Utilities */
#include <iostream>
#include <string>
using namespace std;

int wm(string, string);
int loop(string,int,int,string);
int main(int argc, char* argv[])
{  
	pid_t wgetPID;
	int status;

	//Create the temp directory that is going to be used for temp files
	pid_t mkdirPID = fork();
	if (mkdirPID == 0) {
		execl ("/bin/mkdir", "mkdir", "temp",  (char*)0);
	} else {
		wait (&status);
	}

	//Loop through the arguments and see if -r or --recursive=NUM was specefied,
	//otherwise
	bool recursive = false;
	bool save = false;
	string rnum, str;
	string URL = argv[argc-2];
	for ( int i = 1; i < argc-1; i++) {
		str = argv[i];
		cout << str << endl;

		if (str.length() > 2) {
			if (str.at(2) == 's' && str.length() > 2) {
				save=true;
			}
			if (str.at(2) == 'r') {
				recursive =true;
				rnum=argv[i];
				rnum = rnum.substr(12, rnum.length()-1);
			}
		}	
		//check to see if -r is called
		if (str.at(1) == 'r' ) {
			recursive=true;
			rnum = argv[i+1];
			i++;
		//check to see if --recursive is called, then set rnum to whatever is after the =
		}  
		if (str.at(1) == 's' ) {
			save=true;
		}	
		
	}
	//If the URL is a directory, then see if -r or --recursive=NUM was specefied
	if (URL.at(URL.length()-1) == '/') {
		wgetPID = fork();
		if (wgetPID == 0) {
		//Need to change this depending on what time of image or folder, need if statement
		//to see if we have a directory and if its just an image we just need to use the same
		//switch statement from project 2.

		//use -r and -l # to recursivly download to a certain depth.
			execl ("/usr/bin/wget", "wget", "-r","-l",rnum.c_str(), "-P","./temp/","-A", "jpg,tif,png", argv[argc-2], (char*)0); 
		}
		else {
			wait(&status);
		}
		
	} else if (recursive) {
		//Need to exit because the user tried to call -r or --recursive=NUM on 
		// a single image.
		cout << "bwm will now exit because -r or --recursive=NUM was specefied without specifying a directory" << endl;
		exit(0);
	} else {
		switch (URL.at(URL.length()-2) ) {
		case 'p': 
			cout << "***Image is a jpg" << endl;
	    		execl ("/usr/bin/wget", "wget", "-q", "-O", "./image.jpg", argv[argc-2], (char*)0); 
			break;
		case 'n': 
			cout << "***Image is a png" << endl;
			execl ("/usr/bin/wget", "wget", "-q", "-O", "./image.png", argv[argc-2], (char*)0); 
			break;
		case 'i': 
			cout << "***Image is a TIFF" << endl;
			execl ("/usr/bin/wget", "wget", "-O", "./image.tif", argv[argc-2], (char*)0); 
			break;
 		}
	}

	//If -s or --save was specefied, copy the directory so that we save the 
	if (save) {

		pid_t mkdirOriginalPID = fork();
		if (mkdirOriginalPID == 0) {
			execl ("/bin/mkdir", "mkdir", "Original", (char*)0);
		} else {
			wait (&status);
		}
		pid_t rmdirPID = fork();
		if (rmdirPID == 0) {	
			cout << "Copy Originals because -s or --save was specefied." << endl;
			execl("/bin/","cp","-r","./temp", "./Original",(char*)0);
		}
		else {
			wait(&status);
		}
	}
	
	
	//loop(./temp,);
	string name = "./temp";
	string watermark = "/watermark";
	//loop(name,3,0,watermark);
	//Call wm for a single image
	wm (argv[argc-2], argv[argc-1]);

	

	pid_t rmtemp = fork();
	if (rmtemp == 0) {
		if ( save ) {
			cout << "Remove temp files, but save Originals." << endl;
			execl ("/bin/rm", "rm", "-r", "temp",(char*)0);
		}
		else {
			cout << "Remove all temp files." << endl;
			execl ("/bin/rm","rm","-r", "temp","Original",(char*)0);
		}
	}
	else {
		wait(&status);
	}

	
}
/**
int loop(string name, int max, int current, string watermark){

	DIR* dir;
	dir = opendir(name.c_str());
	struct dirent* ent;
	if(dir != NULL){
		while((ent = readdir(dir)) != NULL){
			DIR* cur = opendir(ent->d_name);
			if(cur != NULL && current + 1 < max){
				cout << "directory:" << ent->d_name << endl;
				current++;
				loop(ent->d_name,max,current++,watermark);
			}else{
				//watermark(ent->d_name,watermark);
				cout<< "image:" << ent->d_name << endl;
			}
		}
	}
	return 0;

}
**/
int wm( string file,  string watermark){
	
string filePath = file;
string wmPath = watermark;

/*Create child pid_t so we can wait fork off children and wait on them to finish*/
    pid_t childpid2;
    int status;     /* parent process: child's exit status */
    //use this to set the type of image, (1=jpg, 2=png, 3=tif) for saving
    //the image as well as when viewing the final image on the web.
    
    childpid2 = fork();		/* Create new child to apply watermark*/
    if ( childpid2 >= 0 )
    {
	 if ( childpid2 == 0 )
	 { 
		//call the exec in child2 to apply the watermark on the image.
		cout << "CHILD2: Begin Apply Watermark." << endl;
		
  		execl ("/usr/bin/composite", "composite", "-compose","bumpmap", "-tile", wmPath.c_str(),filePath.c_str(), wmPath.c_str(), (char*)0);
	}
	else {
		//wait for child2 to finish applying the watermark.
		wait (&status);

   	  }	
    }     
        
    return 0;
}
