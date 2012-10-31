#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <sys/wait.h>   /* Wait for Process Termination */
#include <stdlib.h>     /* General Utilities */
#include <iostream>
#include <string>
#include <dirent.h>
#include <sys/stat.h>
using namespace std;

int wm(string, string);
int loop(DIR*,string,int,int,string);
int main(int argc, char* argv[])
{  
	pid_t wgetPID;
	int status;
	int rlevel;
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
	if(recursive){
		rlevel = atoi(rnum.c_str());
	}else{
		rlevel = -1;
	}
	//If the URL is a directory, then see if -r or --recursive=NUM was specefied
	if (URL.at(URL.length()-1) == '/') {
		wgetPID = fork();
		if (wgetPID == 0) {
		//Need to change this depending on what time of image or folder, need if statement
		//to see if we have a directory and if its just an image we just need to use the same
		//switch statement from project 2.

		//use -r and -l # to recursivly download to a certain depth.
			if (recursive) {
				execl ("/usr/bin/wget", "wget","-q", "-r","-l",rnum.c_str(), "-P","./temp/","-A", "jpg,tif,png", argv[argc-2], (char*)0); 
			} else {
				execl ("/usr/bin/wget", "wget","-q", "-r", "-P","./temp/","-A", "jpg,tif,png", argv[argc-2], (char*)0); 
			}
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
		
		pid_t rmdirPID = fork();
		if (rmdirPID == 0) {	
			cout << "Copy Originals because -s or --save was specefied." << endl;
			execl("/bin/cp","cp","-r","./temp/", "./Original",(char*)0);
		}
		else {
			wait(&status);
		}
	}
	DIR* dir;
	dir = opendir("./temp/accounts.cs.ou.edu/~phil7017");
	
	string watermark = argv[argc-1];
	cout << "**********************Start Loopin****************************" << endl;
	loop(dir,"./temp/accounts.cs.ou.edu/~phil7017",rlevel,0,watermark);
	//Call wm for a single image
	wm (argv[argc-2], argv[argc-1]);

	pid_t uploadPID = fork();
	if (uploadPID == 0) {
		cout << "Begin Upload" << endl;
		execl ("/usr/bin/scp", "scp", "-r", "./temp/accounts.cs.ou.edu/~phil7017/images", "phil7017@gpel4.cs.ou.edu :$HOME/public_html/", (char*)0); 
		cout << "Execed" << endl; 
	} else {
		wait(&status);
	}

	
/**
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
**/
	
}

int loop(DIR* dir, string name, int max, int current, string watermark){
	DIR* dent = opendir(name.c_str());	
	struct dirent* ent;
	string filepath;
	if(dent == NULL){
		cout << "error opening directory" << endl;
		return -1;	
	}
	while((ent = readdir(dent))){
		string file = ent->d_name;
		filepath = name + "/" + ent->d_name;
		struct stat st;
		if(stat(filepath.c_str(), &st)) continue;
		if(S_ISDIR(st.st_mode) && (file.compare(".") != 0 && file.compare("..") != 0)){
   			cout << "Directory:" << ent->d_name << endl;
			DIR* cur = opendir(ent->d_name);
			loop(cur, filepath,max,current++,watermark);
		}else if(file.compare(".") != 0 && file.compare("..") != 0){
   			cout << "File:" << ent->d_name << endl;
			cout << "Pathname:" << filepath << endl;
			if(filepath.at(filepath.length() - 1) != '/')
				wm(filepath,watermark);
		}
	}
	return 1;
}

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
		if(filePath.at(filePath.length() - 1) != '/')
  		execl ("/usr/bin/composite", "composite", "-compose","bumpmap", "-tile", wmPath.c_str(),filePath.c_str(), filePath.c_str(), (char*)0);
	}
	else {
		//wait for child2 to finish applying the watermark.
		wait (&status);

   	  }	
    }     
        
    return 0;
}
