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

		//If the string length is <2 we need to check 
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

	if (recursive && URL.at(URL.length()-1) == '/') {
		//Need to exit because the user tried to call -r or --recursive=NUM on 
		// a single image.
		cout << "bwm will now exit because -r or --recursive=NUM was specefied without specifying a directory" << endl;
		exit(1);
	}
 
	//If the URL is a directory, then see if -r or --recursive=NUM was specefied
	if (URL.at(URL.length()-1) == '/') {
		wgetPID = fork();
		if (wgetPID == 0) {

		//use -r and -l # to recursivly download to a certain depth.
			if (recursive) {
				execl ("/usr/bin/wget", "wget","-q", "-r","-l",rnum.c_str(), "-P","./temp/","-A", "jpg,tif,png", argv[argc-2], (char*)0); 
			} else {
				execl ("/usr/bin/wget", "wget", "-q","-r", "-P","./temp/","-A", "jpg,tif,png", argv[argc-2], (char*)0); 
			}
		}
		else {
			wait(&status);
		}
		
	} else {
		//If it is a single image, download it
		pid_t singleImagePID = fork();
		if (singleImagePID == 0) {
			switch (URL.at(URL.length()-2) ) {
			case 'p': 
				cout << "***Image is a jpg" << endl;
		    		execl ("/usr/bin/wget", "wget", "-q","-P","./temp", argv[argc-2], (char*)0); 
				break;
			case 'n': 
				cout << "***Image is a png" << endl;
				execl ("/usr/bin/wget", "wget", "-q", "-P","./temp", argv[argc-2], (char*)0); 
				break;
			case 'i': 
				cout << "***Image is a TIFF" << endl;
				execl ("/usr/bin/wget", "wget", "-P","./temp", argv[argc-2], (char*)0); 
				break;
			}
		} else {
			wait(&status);
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

	//Create the directory stuff for the looping
	DIR* dir;
	dir = opendir("./temp/accounts.cs.ou.edu/~phil7017");
	string URL2;
	string watermark = argv[argc-1];
	if (URL.at(URL.length()-1) == '/') {
		cout << "*****************Start Sweeping For Images**********************" << endl;
		loop(dir,"./temp/accounts.cs.ou.edu/~phil7017",rlevel,0,watermark);
	} else {
	//Call wm for a single image
		URL2 = "./temp/" + URL.substr(URL.find_last_of('/'), URL.length()-1);
		wm (URL2, argv[argc-1]);
	}

	//Fork off the process to upload the images. 
	pid_t uploadPID = fork();
	if (uploadPID == 0) {
		if ( URL.at(URL.length()-1) == '/' ) {
			cout << "Begin Upload" << endl;
			execl ("/usr/bin/scp", "scp", "-r", "./temp/accounts.cs.ou.edu/~phil7017/images/", "phil7017@gpel1.cs.ou.edu:~/public_html/", (char*)0); 
		} else {
			cout << "Begin Upload" << endl;
			execl ("/usr/bin/scp", "scp", URL2.c_str(), "phil7017@gpel1.cs.ou.edu:~/public_html/", (char*)0); 
		}
	} else {
		wait(&status);
	}
	
	//Fork off process to remove the temp files.
	pid_t rmtemp = fork();
	if (rmtemp == 0) {
		cout << "Remove Temporary Files" << endl;
		execl ("/bin/rm", "rm", "-r", "temp",(char*)0);
	}
	else {
		wait(&status);
	}
}

/**
Method that loops through the files recursivly and when an image is found
 calls the wm method to apply the watermark.
**/
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
			DIR* cur = opendir(ent->d_name);
			loop(cur, filepath,max,current++,watermark);
		}else if(file.compare(".") != 0 && file.compare("..") != 0){
			if(filepath.at(filepath.length() - 1) != '/') {
				cout << "Watermark image " << filepath << endl;
				wm(filepath,watermark);
			}
		}
	}
	return 1;
}

/**
Method that takes two string values, one for the pathname to the image and the pathname
 to the watermark. Applies the watermark and overwrites the original image. 
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
