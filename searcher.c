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
#include <stdio.h> // printf
#include <string.h> // memcmp
#include <dirent.h> // struct dirent, type DIR
#include <stdbool.h> // bool


// Function Prototypes
// @todo - Switch the size int to size_t for passing array size around.
int convert(char *, int);
int findInBuffer(unsigned char *, size_t, unsigned char *, size_t);
int search(char *, unsigned char *, size_t);
int r_search(char *, unsigned char *, size_t);
void printHexBuffer(unsigned char *, size_t);

int g_hit_counter = 0;

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
    
    size_t buffer_length = i - 2;
    
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
 * Find a value from one buffer in the other.
 */
int findInBuffer(unsigned char *search_buffer, size_t search_len, unsigned char *window_buffer, size_t window_len) {
  int i;
  int foundAt = 0;
  int search = 0;
  bool found = false;

  for (i = 0; i < window_len && search < search_len; i++) {
  // If we find the match
    if (window_buffer[i] == search_buffer[search]) {
      if (found == false) {
        found = true;
        foundAt = i;
      }
      search++;
    }
    else {
      if (found == true) {
        found = false;
        search--;
        i = foundAt;
      }
    }
  }

  if (found == true) {
    return foundAt;
  }
  else {
    return -1;
  }
}

/**
 * Search for a hex pattern in a given file.
 */
int search(char *filename, unsigned char *input_buffer, size_t buffer_length) {
  int i = 0;
  int err = 0;
  int ret_val = 0;
  
  FILE *file;
  size_t bytes_read;
  int total_bytes_read = 0; // @todo make this size_t
  unsigned char buffer[buffer_length];

  printf("\nSearching: %s ", filename);

  // Get the file handle.
  if ((file = fopen(filename, "rb")) == NULL) {
    printf("Failed: %s\n", filename); 
    // @todo - Why is this a 1? ... probably a particular error code. No convention set though. ... Possibly for the recursive version.
    return ret_val = 1;
  }

  //Process the file for matches
  do {
    // If we didn't find a match check the current buffer for a place to start searching.
    // @todo - Make this it's own function.
    // @todo - Change to search from first occurrence to the end of the buffer.
      // @todo - Peak ahead could be a bit of a compromise.
    // @todo - Check if memchr is sufficient for this. @see http://www.cplusplus.com/reference/cstring/memchr/
    if (total_bytes_read > 0) {

      //Search the current array for another occurance of the first character
      // @todo - bytes_read is probably better.
      bool match = false;
      for (i = 0; i < buffer_length; i++) {
        if (input_buffer[0] == buffer[i]) {
          total_bytes_read -= buffer_length;
          total_bytes_read += i + 1;

          // Move the read head to the position we found.
          fseek(
            file,
            total_bytes_read, // Num bytes to rewind -- we are actually fast forwarding from the beginning, thus total.
            SEEK_SET          // Where to start from (SEEK_SET, SEEK_CUR, SEEK_END).
          );

          break;
        }
      }
    }

    // Pull in a new buffer based on where the read head is.
    // @see fseek().
    bytes_read = fread(
      buffer,        // Fill this with the data being read.
      1,             // How big is an item.
      buffer_length, // How many items to read.
      file           // The file to read from.
    );

    // Break at the end of the file.
    if(bytes_read < buffer_length) {
      err = 1;
      break;
    }

    // Increment the Byte counter.
    // @todo - This is sending a wrong total bytes if the EOF is reached. ... Try bytes_read instead.
    total_bytes_read += buffer_length;
  }
  while(memcmp(input_buffer, buffer, bytes_read) != 0);
  
  // Close the file handle.
  fclose(file);

  // If Success.
  if (err != 1) {
    ret_val = 1; // @todo this is the same code as a bad file handle.

    printf("\nSuccess!\n  Input:  ");
    printHexBuffer(input_buffer, buffer_length);
    printf("\n  Output: ");
    printHexBuffer(buffer, bytes_read);
    printf("\n  Position: %d", total_bytes_read);
  }

  return ret_val;
}

/**
 * Run a search recursively against the speicifed folder instad of on a single file.
 * @see search().
 *
 * @todo - Handle file permissions.
 * @todo - Skip locked files.
 */
int r_search(char *starting_directory, unsigned char *input_buffer, size_t buffer_length) {
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
          printf("\nCalling r_search: starting_directory = %s | path = %s\n===========\n", starting_directory, path);
          ret_val = r_search(path, input_buffer, buffer_length);
          
          // ENABLE THIS TO STOP ON THE FIRST FOUND MATCH.
          // CURRENTLY WE ARE TESTING TO SEE HOW MANY MATCHES WE CAN FIND.
          if (ret_val == 1) {
            //break;
            printf("\n\n\n Number of matches: %d", ++g_hit_counter);
          }
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
 * Print Hex Buffer helper method.
 */
void printHexBuffer (unsigned char *buffer, size_t len) {
  int i;
  for (i = 0; i < len; i++) {
    printf("%.2X ", buffer[i]);
  }
  return;
}

/*

int rewindReadHead (file, totalBytes, numBytesToRewind) {
  
  return file || null;
}


// */
