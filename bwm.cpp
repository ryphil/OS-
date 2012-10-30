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
		execl ("/bin/mkdir", "mkdir", "temp", (char*)0);
	} else {
		wait (&status);
	}

	//Loop through the arguments and see if -r or --recursive=NUM was specefied,
	//otherwise
	bool recursive = false;
	string rnum, str2;
	string URL = argv[argc-2];
	
	//If the URL is a directory, then see if -r or --recursive=NUM was specefied
	if (URL.at(URL.length()-1) == '/') {
		
		for ( int i = 1; i < argc-1; i++) {
			str2 = argv[i];
			//check to see if -r is called
			if (str2.at(1) == 'r' ) {
				recursive=true;
				rnum = argv[i+1];
				break;
			//check to see if --recursive is called, then set rnum to whatever is after the =
			} else if (str2.at(2) == 'r') {
				recursive =true;
				rnum=argv[i];
				rnum = rnum.substr(12, rnum.length()-1);
				break;
			}
		}
	} else if (recursive) {
		//Need to exit because the user tried to call -r or --recursive=NUM on 
		// a single image.
		cout << "bwm will now exit because -r or --recursive=NUM was specefied without specifying a directory" << endl;
		exit(0);
	}
	

	//If -s or --save was not specefied, remove the directory
	if (recursive) {
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
	} else {
		//Need to do something if the recursive call is not called, need to also exit if the -r is
		//called for a single image. 
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
	
	//loop(./temp,);
	string name = "./temp";
	string watermark = "/watermark";
	//loop(name,3,0,watermark);
	//Call wm for a single image
	wm (argv[argc-2], argv[argc-1]);

	//Loop through the arguments and see if -s or --save was specefied
	//if so, make sure we save the original file.
	bool save = false;
	string str;
	for ( int i = 1; i < argc-1; i++) {
		str = argv[i];
		if (str.at(1) == 's' || str.at(2) == 's') {
			save=true;
		}	
	}

	//If -s or --save was not specefied, remove the directory
	if (!save) {
		pid_t rmdirPID = fork();
		if (rmdirPID == 0) {	
			cout << "remove the directory" << endl;
			execl("/bin/rm","rm","-r","temp",(char*)0);
		}
		else {
			wait(&status);
		}
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

cout << filePath << " " << wmPath << endl;
/*Create child pid_t so we can wait fork off children and wait on them to finish*/
    pid_t childpid2;
    pid_t childpid3;
    pid_t childpid4;
    int status;     /* parent process: child's exit status */
    //use this to set the type of image, (1=jpg, 2=png, 3=tif) for saving
    //the image as well as when viewing the final image on the web.
    int imageType; 
    string imageURL = file; 
    switch (imageURL[imageURL.size() - 2]) {
	case'i':  {
		imageType = 3;
		break;
        }
	case'p': {
		
		imageType = 1;
		break;
        }
	case'n': {
		imageType = 2;
		break;
	}
    }

    childpid2 = fork();		/* Create new child to apply watermark*/
    if ( childpid2 >= 0 )
    {
	 if ( childpid2 == 0 )
	 { 
		//call the exec in child2 to apply the watermark on the image.
		cout << "CHILD2: Begin Apply Watermark." << endl;
  
	if ( imageType == 1) {
	    	int err = execl ("/usr/bin/composite", "composite", "-compose","bumpmap", "-tile", wmPath.c_str(),filePath.c_str(), "./temp/jpg_image.jpg", (char*)0);
      		cout << err << endl;
    	} else if (imageType == 2 ) {
		int err = execl ("/usr/bin/composite", "composite", "-compose","bumpmap", "-tile",wmPath.c_str(),filePath.c_str(), "./temp/png_image.png", (char*)0);
       		cout << err << endl;
	} else {
		int err = execl ("/usr/bin/composite", "composite", "-compose","bumpmap", "-tile",wmPath.c_str(),filePath.c_str(), "./temp/tif_image.tif", (char*)0);
       		cout << err << endl;
 	}
		cout << "CHILD2: Watermark Failed." << endl;
	}
	else {
		//wait for child2 to finish applying the watermark.
		wait (&status);

		//Create the 3rd child to upload the image.	
		childpid3 = fork();		
   		if ( childpid3 >= 0 )
  		{
		    if ( childpid3 == 0 )
	 	    { 
			//In the child Process, we must exec a new process to
			//upload the image to the server.
			cout << "CHILD3: Begin upload." << endl;
		
			if ( imageType == 1) {
				execl ("/usr/bin/sftp", "sftp","-o", "batchmode no", "-b", "jpg.bat", "phil7017@gpel1.cs.ou.edu", (char*)0);

    			} else if (imageType == 2 ) {
				execl ("/usr/bin/sftp", "sftp","-o", "batchmode no", "-b", "png.bat", "phil7017@gpel1.cs.ou.edu", (char*)0);
			} else {
				execl ("/usr/bin/sftp", "sftp","-o", "batchmode no", "-b", "tif.bat", "phil7017@gpel1.cs.ou.edu", (char*)0);

			}
			//execl ("/usr/bin/sftp", "sftp","-o", "batchmode no", "-b", "jpg.bat", "phil7017@gpel1.cs.ou.edu", (char*)0);
			cout << "CHILD3: Exec failed." << endl;
		    }	
	  	    else {
			//wait for the child to finish uploading
			wait (&status);
			childpid4 = fork();
			if ( childpid4 >= 0 )
			{
				if ( childpid4 == 0 ) {
					//Launch browser here
					cout << "CHILD4: child 4 launches web browser." << endl;
					char* browser = getenv("BROWSER");	
					cout << browser << endl;
					if(browser == NULL){
						cout<< "BROWSER not set, will launch Firefox." << endl;
						exit(3);
					} else{
						if ( imageType == 1 ){ 							
							execl("/usr/bin/firefox",browser,"accounts.cs.ou.edu/~phil7017/jpg_image.jpg",(char*)0);
						} else if ( imageType == 2 ) {
							execl("/usr/bin/firefox",browser,"accounts.cs.ou.edu/~phil7017/png_image.png",(char*)0);
						} else {
							execl("/usr/bin/firefox",browser,"accounts.cs.ou.edu/~phil7017/tif_image.tif",(char*)0);
						}
					}
					
				}
				else {
					wait (&status);
				}
			  }
		     }
		}	
   	  }	
    }     
        
    return 0;
}
