#include <errno.h>      /* Errors */
#include <stdio.h>      /* Input/Output */
#include <sys/wait.h>   /* Wait for Process Termination */
#include <stdlib.h>     /* General Utilities */
#include <iostream>
#include <string>
using namespace std;

int wm(string, string);

int main(int argc, char* argv[])
{  
	string imagePath = argv[argc-2];
	string wmPath = argv[argc-1];
	system("mkdir temp");
	//loop(./temp,);

	cout << imagePath << " " << wmPath << endl;

	wm (imagePath, wmPath);
	system ("rmdir temp");
}
/**
int loop(DIR dir, int max, int current, String watermark){
	
	while(loop through dir){
		if(is dir && current + 1 < max){
			loop(dir name,max,current++,watermark);
		}else if(is image){
			watermark(image,watermark);
		}
	}

}**/

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
           exit(0);  /* parent exits with code 0 since everything completed successfully */       
        
   

    return 0;
}
