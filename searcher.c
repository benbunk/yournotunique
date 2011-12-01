//Purpose: Search a binary file for a specified hexadecimal set
//Programmer: Benjamin Bunk
//Input: File location, Hexadecimal set to search for
//Output: Found (true/false), Beginning offset of where it is found

//Headers
#include <stdio.h> //cin, cout
#include <string.h> //memcmp
#include <dirent.h> //struct dirent, type DIR


//Function Prototypes
int Convert(char *, int);
int r_search(char *, unsigned char *, int);
int search(char *, unsigned char *, int);

//Begin main
int main(int argc, char *argv[])
{
  //Process arguments
  if(argc < 3) {
    //Program heading
    printf("\n\n%s\n", "Searcher v0.1");
    printf("%s\n", "Author: Ben Bunk");
    printf("%s\n\n", "Purpose: Search a file for a hexadecimal string");
    printf("%s\n", "Format: searcher [filelocation+filename] hex data");
    printf("%s\n", "Example: searcher 01 - All We Know.mp3 00 32 30 30 35 20 20 20 20 20 20 20 20 20 20");
    printf("%s\n", "Example (Recursive Search): searcher r 00 32 30 30 35 20 20 20 20 20 20 20 20 20 20");
  }
  else {
    //Process arguments
    
    //printf("\n%s\n", "Begin processing");
    
    char *filename = argv[1];


////////////////////////////////////////////////////CONVERT INPUT STRING//////////////////////////////////////////
      //Convert input string to hex
      unsigned char converted[argc-2];
//Debugging
      //printf("Size of argv - 2 (Used to create the converted array): %d\n", argc-2);
      //printf("%s", "Converted input: ");
      int i;
      i = 2;
      while(argv[i]) {
        //debugging printf("Arg %d as string %s\n", i, argv[i]);
        converted[i-2]=(unsigned char)Convert(argv[i], 16);
        //debugging printf("Arg %d as hex %.2X\n", i, (unsigned int)converted[i-2]);
        //printf("%.2X ", (unsigned int)converted[i - 2]);
        i++;
      }
//printf("\n");
      int numItems = i - 2;
//Debugging
      //printf("Number of items: %d\n", numItems);
/////////////////////////////////////////////////////END CONVERT INPUT STRING////////////////////////////////

////////////////////////////////////////////////////SWITCH based on filename/////////////////////////////////
      if ( strcmp(filename, "r") == 0 ) {
        r_search("/", converted, numItems);
      } else {
        //Call search
        search(filename, converted, numItems);
      }
//////////////////////////////////////////////////END SWITCH BASED ON FILENAME///////////////////////////////
  }

  printf("\n\n\n");
 return 0;
}

int Convert(char *buffer, int cbase) {
    int i;
    int bin;
    i = bin = 0;
    
    while(buffer[i]) {
        buffer[i] -= 48;
        
        if(buffer[i] > 16)
            buffer[i] -= 7;
        if(buffer[i] >= cbase)
            buffer[i] -= 32;
        if(buffer[i] >= cbase || buffer[i] < 0)
            break;
        
        bin *= cbase;
        bin += buffer[i];
        i++;
    }
    
    return bin;
}

int r_search(char *dstart, unsigned char *converted, int numItems) {
//printf("===========\nEntering r_search\ndstart: %s\n", dstart);
  int ret_val=0;
  DIR *d = NULL;
  struct dirent *dir = NULL;
//printf("Path=%s | ", path);
  d = opendir(dstart);
//printf("After opening: dstart = %s\n", dstart);
  if(d) {
//printf("d exists: dstart = %s\n", dstart);
    // Get next dir
    while ( (dir = readdir(d)) != NULL) {
      //Setup the path variable
      char path[strlen(dstart) + strlen("/") + strlen(dir->d_name)];
      strcpy(path, "");
//printf("dstart before path: %s\n", dstart);
      strcat(path, dstart);
//printf("Path=%s | ", path);
      if(strcmp(dstart, "/") != 0) {
        strcat(path, "/");
      }
//printf("Path=%s | ", path);
      strcat(path, dir->d_name);
//printf("dstart after path: %s\n", dstart);
//printf("path: %s\n", path);

//printf("Inside loop: dstart = %s\n", dstart);
      // File
      if(dir->d_type == DT_REG) {
//printf("%s | File | %i\n", dir->d_name, dir->d_reclen);
        //scan the file found
        ret_val = search(path, converted, numItems);
        if (ret_val == 1) {
          break;
        }
      // Directory
      } else if (dir->d_type == DT_DIR) { 
//printf("%s | Directory | %i\n", dir->d_name, dir->d_reclen);
        //Dont create runtime loops
        if( (strcmp(dir->d_name, "..")==0) || (strcmp(dir->d_name, ".")==0) ) { // Fail on endless loops
//printf("Directory is blacklisted: dstart = %s\n", dstart);
        } else {  
//printf("Directory is OK\n");
printf("Calling r_search: dstart = %s | path = %s\n===========\n", dstart, path);
          ret_val = r_search(path, converted, numItems);
///////////////////////////////////////////////ENABLE FOR NORMAL OPERATION///////////////////////////////////
//          if (ret_val == 1) { break; }
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//printf("Returned to r_search caller: dstart = %s | ret_val = %i\n", dstart, ret_val);
        }
      // Other possibilities but we don't care
      } else {
//printf("%s | Unknown\n", dir->d_name);
//printf("Type is blacklisted: dstart = %s\n", dstart);
      }
    }
//printf("Exited loop: dstart = %s\n", dstart);
    closedir(d);
//printf("Closed d: dstart = %s\n", dstart);
  }
 return(ret_val);
}

int search(char *filename, unsigned char *converted, int numItems) {
    FILE *file;

//If filename == r then process recursively otherwise do it like normal
  int ret_val=0;
    if((file = fopen(filename, "rb")) == NULL) {
      printf("Failed: %s\n", filename); 
      return ret_val=1;
    } 
    else {
//Debugging
      printf("Searching: %s ", filename);
      int i=0;
      //Process the file for matches
      size_t bytes_read;
      unsigned char buffer[numItems];
//Debugging
      //printf("Size of converted after being populated: %d\n", sizeof(converted));
      int err=0;
      int j=0;
      int total_bytes_read=0;
      do {
//Debugging
        //printf("\n\nChunk Number: %d\n", j);
        if(j>0) {

///////////////////////////////////////if numItems == numItemsCurrentPass || numItemsCurrentPass > lowerLimit
///////////////////////////////////////Reset pointer back to beginning 
///////////////////////////////////////Set numItemsCurrentPass 1 less than last time
///////////////////////////////////////Search again
///////////////////////////////////////else 
///////////////////////////////////////reset back to beginning
///////////////////////////////////////search original buffer based on original size for another occurence of first char
///////////////////////////////////////Try it over again. 
//Debugging
	  //printf("Searching last run buffer for a match\n");
          //Search the current array for another occurance of the first character
          for(i=0; i<numItems; i++) {
//Debugging
            //printf("Match to find: %.2X == %.2X\n", converted[0], buffer[i]);         
            if(converted[0] == buffer[i]) {
              //If it exists seek from the beginning multiply the number of passes by the numItems to get total bytes read 
              // so far. 
	      // Than add the array index to set the pointer to the current location of the found byte.
              total_bytes_read-=numItems;
              total_bytes_read+=i+1;
              fseek(

                file,
                total_bytes_read, //Num bytes to rewind -- we are actually fast forwarding from the beginning, thus total
                SEEK_SET//Where to start from (SEEK_SET, SEEK_CUR, SEEK_END)
              );
//Debugging
	    //printf("Match found breaking: seek bytes == %d\n", total_bytes_read );
              break;
            }
          }
        }

        bytes_read = fread(
          buffer, //Fill this with the data 
          1, //How big is an item
          numItems, //How many items to read
          file); // The file to read from
	if(bytes_read < numItems) { //Was numItems
          err=1;
          break; //Breaking here eliminates the need for a condition in the do{}while();
        }
        j++;
        total_bytes_read+=numItems;
//Debugging
        /*printf("Total Bytes Read: %d\n", total_bytes_read);
        printf("Size of numItems: %d\n", numItems);
        printf("Size of buffer after being populated: %d\n", sizeof(buffer));

        printf("Bytes Read: %d\n", bytes_read);
        printf("%s", "Printing file buffer contents: \n");        
	for(i=0; i < bytes_read; i++) {
          printf("%.2X ", buffer[i]);

        }*/
      }
      while(memcmp(converted, buffer, bytes_read) != 0);
//Debugging      
      //while( j < 7); 


      if(err==1){
//Debugging
        printf("\nEOF\n");
      }
      else {
        ret_val = 1;
        printf("\n%s\nSuccess:\nInput:  ", filename);
        for(i=0; i < numItems; i++) {
          printf("%.2X ", converted[i]);
        }
        printf("\nOutput: ");        
	for(i=0; i < bytes_read; i++) {
          printf("%.2X ", buffer[i]);
        }
      }
     fclose(file);
   }

      //Print if converted and buffer are equivalent by memcompare and by the loop
      /*printf("\nConverted is to Buffer: %d (0 is equal, +-N where n is the difference)", memcmp(converted, buffer, bytes_read));*/
 return ret_val;
}




/*

int findInBuffer (searchBuffer, searchValue) { 
 
 return positionSearchValue || -1;
}

int rewindReadHead (file, totalBytes, numBytesToRewind) {
  
  return file || null;
}


*/
