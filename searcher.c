/**
 * Purpose: Search a binary file for a specified hexadecimal set
 * Programmer: Benjamin Bunk
 * Input: File location, Hexadecimal set to search for
 * Output: Found (true/false), Beginning offset of where it is found
 */

/**
 * @todo check if we need malloc.
 * @todo check what our security position is.
 */

// Headers
#include <stdio.h> // cin, cout
#include <string.h> // memcmp
#include <dirent.h> // struct dirent, type DIR


// Function Prototypes
int convert(char *, int);
int r_search(char *, unsigned char *, int);
int search(char *, unsigned char *, int);

/**
 * Main entry point.
 */
int main(int argc, char *argv[])
{
  // Process arguments.
  if(argc < 3) {
    // Program heading.
    printf("\n\n%s\n", "Searcher v0.1");
    printf("%s\n", "Author: Ben Bunk");
    printf("%s\n\n", "Purpose: Search a file for a hexadecimal string");
    printf("%s\n", "Format: searcher [filelocation+filename] hex data");
    printf("%s\n", "Example: searcher 01 - All We Know.mp3 00 32 30 30 35 20 20 20 20 20 20 20 20 20 20");
    printf("%s\n", "Example (Recursive Search): searcher r 00 32 30 30 35 20 20 20 20 20 20 20 20 20 20");
  }
  else {
    char *filename = argv[1];
    unsigned char input_buffer[argc-2];
    int i;
    i = 2;

    // Convert input hex string to a hex array.
    while(argv[i]) {
      input_buffer[i-2] = (unsigned char)convert(argv[i], 16);
      i++;
    }
    
    int buffer_length = i - 2;
    
    // Call the appropriate search method.
    if ( strcmp(filename, "r") == 0 ) {
      r_search("/", input_buffer, buffer_length);
    } else {
      search(filename, input_buffer, buffer_length);
    }
  }

  printf("\n\n\n");
  return 0;
}

/**
 * Convert a string hex characters into a char (hex) value.
 */
int convert(char *buffer, int cbase) {
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

/**
 * Run a search recursively against the speicifed folder instad of on a single file.
 * @see search().
 *
 * @todo - File and directory handling is off...maybe im not doing it posix style?
 */
int r_search(char *starting_directory, unsigned char *input_buffer, int buffer_length) {
  int ret_val = 0;
  DIR *directory = NULL;
  struct dirent *dir = NULL;

  // Load the directory and do some work.
  directory = opendir(starting_directory);

  // @todo - Switch this to a pesimistic if statement and avoid the 40 line wrapper.
  if (directory) {
    // Get next dir
    while ((dir = readdir(directory)) != NULL) {
      //Setup the path variable
      char path[strlen(starting_directory) + strlen("/") + strlen(dir->d_name)];
      strcpy(path, "");
      strcat(path, starting_directory);
      if(strcmp(starting_directory, "/") != 0) {
        strcat(path, "/");
      }
      strcat(path, dir->d_name);

      // @todo change this to a switch so it's more readable.
      // Switch logic based on the type of file we loaded.
      // File
      if (dir->d_type == DT_REG) {
        //scan the file found
        ret_val = search(path, input_buffer, buffer_length);
        if (ret_val == 1) {
          break;
        }
      }
      // Directory
      else if (dir->d_type == DT_DIR) {
        // Avoid a loop by skipping file handles that reference the current/previous directory.
        if( (strcmp(dir->d_name, "..")==0) || (strcmp(dir->d_name, ".")==0) ) { // Fail on endless loops
          // No Op.
        } 
        else {
          printf("Calling r_search: starting_directory = %s | path = %s\n===========\n", starting_directory, path);
          ret_val = r_search(path, input_buffer, buffer_length);
          
          // ENABLE THIS TO STOP ON THE FIRST FOUND MATCH.
          // CURRENTLY WE ARE TESTING TO SEE HOW MANY MATCHES WE CAN FIND.
          // if (ret_val == 1) { break; }
        }

      // Other possibilities but we don't care
      } else {
        // No Op.
      }
    }

    // Close the file handle.
    closedir(directory);
  }

  return ret_val;
}

/**
 * Search for a hex pattern in a given file.
 */
int search(char *filename, unsigned char *input_buffer, int buffer_length) {
  FILE *file;
  int ret_val = 0;
  
  if ((file = fopen(filename, "rb")) == NULL) {
    printf("Failed: %s\n", filename); 
    // @todo - Why is this a 1?
    return ret_val = 1;
  }
  else {
    int i = 0;
    size_t bytes_read;
    unsigned char buffer[buffer_length];
    int err = 0;
    int j = 0;
    int total_bytes_read = 0;

    printf("Searching: %s ", filename);

    //Process the file for matches
    do {
      if (j > 0) {
      // if buffer_length == numItemsCurrentPass || numItemsCurrentPass > lowerLimit
      // Reset pointer back to beginning 
      // Set numItemsCurrentPass 1 less than last time
      // Search again
      // else 
      // reset back to beginning
      // search original buffer based on original size for another occurence of first char
      // Try it over again. 

        //Search the current array for another occurance of the first character
        for (i = 0; i < buffer_length; i++) {
          if (input_buffer[0] == buffer[i]) {
            // If it exists seek from the beginning multiply the number of passes by the buffer_length to get total bytes read.
            // so far. 
            // Than add the array index to set the pointer to the current location of the found byte.
            total_bytes_read -= buffer_length;
            total_bytes_read += i + 1;

            fseek(
              file,
              total_bytes_read, // Num bytes to rewind -- we are actually fast forwarding from the beginning, thus total.
              SEEK_SET          // Where to start from (SEEK_SET, SEEK_CUR, SEEK_END).
            );

            break;
          }
        }
      }

      bytes_read = fread(
        buffer,   // Fill this with the data.
        1,        // How big is an item.
        buffer_length, // How many items to read.
        file);    // The file to read from.

      // Breaking here eliminates the need for a condition in the do{}while();
      if(bytes_read < buffer_length) {
        err = 1;
        break;
      }

      j++;
      total_bytes_read += buffer_length;
    }
    while(memcmp(input_buffer, buffer, bytes_read) != 0);

    if (err == 1) {
      // No Op.
    }
    else {
      ret_val = 1;
      
      printf("\n%s\nSuccess:\nInput:  ", filename);
      
      for (i = 0; i < buffer_length; i++) {
        printf("%.2X ", input_buffer[i]);
      }
      
      printf("\nOutput: ");        

      for(i = 0; i < bytes_read; i++) {
        printf("%.2X ", buffer[i]);
      }

      printf("\nPosition: %d", total_bytes_read);
    }
    
    fclose(file);
  }

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
