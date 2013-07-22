/**
 * Purpose: Search a binary file for a specified hexadecimal pattern
 * Programmer: Benjamin Bunk
 * Input: File location, Hexadecimal set to search for
 * Output: Beginning offset of where it is found, the filename, and a check of the data comparison.
 */

// Headers
#include <stdio.h>    // printf
#include <string.h>   // memcmp
#include <dirent.h>   // struct dirent, type DIR
#include <stdbool.h>  // bool
#include <stdlib.h>   // exit

#include <sys/types.h>
#include <sys/stat.h>
#include <err.h>      // perror
#include <fts.h>      // File Tree

#include <pthread.h> // pthread create, destroy, mutext.


// Function Prototypes
int convert(char *, int);
int find_in_buffer(unsigned char *, int, unsigned char *, size_t);
int search(const char *, unsigned char *, int);
void *r_search(void *);
void next_buffer(unsigned char *, int *, int);
void copy_buffer(unsigned char *, unsigned char *, int);
void printHexBuffer(unsigned char *, int);

#define NUMTHRDS 10
pthread_mutex_t mutex;
pthread_t pth[NUMTHRDS];  // this is our thread identifier

#define buffer_size 5
int buffer_pos_max = 255;
int buffer_current = 0;
unsigned char buffer[buffer_size];

/**
 * Main entry point.
 */
int main(int argc, char *argv[])
{
  // Process arguments.
  if(argc < 3) {
    // Program heading.
    printf("\n\n%s\n", "Searcher v0.2");
    printf("%s\n", "Author: Ben Bunk");
    printf("%s\n\n", "Purpose: Search a file for a hexadecimal pattern.");
    printf("%s\n", "Format: searcher [filename] [hex data]");
    printf("%s\n", "Example: searcher AllWeKnow.mp3 00 32 30 30 35 20 20 20 20 20 20 20 20 20 20");
    printf("%s\n", "Example (Recursive Search from root (/)): searcher r 00 32 30 30 35 20 20 20 20 20 20 20 20 20 20");
    printf("%s\n", "Recursive is restricted to the first 20 levels of directory stucture.");
  }
  else {
    const char *filename = argv[1];
    unsigned char input_buffer[argc-2];

    // Convert input hex string to a hex array.
    int i = 2;
    while(argv[i]) {
      input_buffer[i-2] = (unsigned char)convert(argv[i], 16);
      i++;
    }

    // Set the global reference to the input.
    unsigned char *input = input_buffer;
    size_t input_length = i - 2;

    // Call the appropriate search method.
    if (strcmp(filename, "r") == 0) {
      // @todo pass in a path at some point.
      printf("Initiating Mutex\n");
      pthread_mutex_init(&mutex, NULL);

      int j, c;
      for (j = 0; j < NUMTHRDS; j++) {
        printf("Starting Thread: %d\n", j);
        pthread_create(&pth[j],NULL,r_search, &j);
      }

      // Wait for threads to finish.
      for (j = 0; j < 4; j++) {
        pthread_join(pth[j],NULL);
      }
    } else {
      search(filename, input, input_length);
    }
  }

  pthread_exit(NULL);
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
 * Find the starting position of one buffer inside of another.
 * 
 * Partial matches are allowed if it is consecutive from it's start
 * to the end of the window buffer. Meaning that if the first 3 values
 * of the search buffer are found in the last 3 spaces of the window buffer
 * it is considered a match because as much of the search buffer was found
 * as possible.
 * However if the first 3 values of the search buffer are found in the last 4 
 * positions of the window buffer but the 4 and final window buffer position 
 * doesn't match the 4th value in the search buffer then it would not be 
 * considered a match.
 *
 * Information from this function can be used to adjust the window size and 
 * read head position so that a faster function like @see memcmp can be used. 
 * 
 * unsigned char *search_buffer
 *   The buffer we are looking for.
 * size_t search_len
 *   Size of the search_buffer.
 * unsigned char *window_buffer
 *   The buffer we are looking in.
 * size_t window_len
 *   Size of the window_buffer.
 * 
 * Return int
 *   -1 if search buffer isn't found at all, N position in the window buffer if it is found.
 */
int find_in_buffer(unsigned char *search_buffer, int search_len, unsigned char *window_buffer, size_t window_len) {
  int i;
  int found_position = 0;
  int search_position = 0;
  bool found = false;

  for (i = 0; i < window_len && search_position < search_len; i++) {
  // If we find the match
    if (window_buffer[i] == search_buffer[search_position]) {
      if (found == false) {
        found = true;
        found_position = i;
      }
      search_position++;
    }
    else {
      if (found == true) {
        found = false;
        search_position--;
        i = found_position;
      }
    }
  }

  if (found == true) {
    return found_position;
  }
  else {
    return -1;
  }
}

/**
 * Search for a hex pattern in a given file.
 */
int search(const char *filename, unsigned char *input_buffer, int buffer_length) {
//  unsigned char *input_buffer = (unsigned char *)input;
//  size_t buffer_length = input_length;

  int i = 0;
  int err = 0;
  int ret_val = 0;
  int found;

  FILE *file;
  size_t bytes_read;
  int total_bytes_read = 0;
  unsigned char buffer[buffer_length];
  //printHexBuffer(input_buffer, buffer_length);

  //printf("\nSearching: %s ", filename);

  // Get the file handle.
  if ((file = fopen(filename, "rb")) == NULL) {
    //printf("Failed: %s\n", filename); 
    return ret_val = 1;
  }

  //Process the file for matches
  do {
    // Check if we have an occurence of our search embedded in the middle of this buffer.
    if (total_bytes_read > 0) {
      found = find_in_buffer(input_buffer, buffer_length, buffer, bytes_read);
      if ( found != -1) {
        total_bytes_read -= buffer_length;
        total_bytes_read += found;

        // Move the read head to the position we found.
        // We are actually fast forwarding from the beginning, thus total bytes here.
        fseek(file, total_bytes_read, SEEK_SET);
      }
    }

    // Pull in a new buffer based on where the read head is. @see fseek().
    bytes_read = fread(buffer, 1, buffer_length, file);

    // Break at the end of the file.
    if(bytes_read < buffer_length) {
      err = 1;
      break;
    }

    // Increment the Byte counter.
    total_bytes_read += buffer_length;
  }
  while(memcmp(input_buffer, buffer, bytes_read) != 0);
  
  // Close the file handle.
  fclose(file);

  // If Success.
  if (err != 1) {
    ret_val = 2;

    /*printf("\nSuccess!\n  Input:  ");
    printHexBuffer(input_buffer, buffer_length);
    printf("\n  Output: ");
    printHexBuffer(buffer, bytes_read);
    printf("\n  Position: %d", total_bytes_read);*/
  }

  return ret_val;
}

/**
 * Recursively trace a file structure specified in path.
 *
 * This function only contains the directory scanning logic and any
 * aggreagate statistics at the file level.
 * @see search() for information on byte level matching.
 *
 * @see fts()
 */
// Next_buffer support stuff
void *r_search(void *arg) {

  int* str = (int *)arg;
  int local = *str;

  printf("Thread %d: Started.\n", local);

  int ret_val = 0;
  char * const filepaths[] = { 
      "/", // Default directory.
      NULL 
  };
  FTS *ftsp;
  FTSENT *p, *chp;
  int fts_options = FTS_COMFOLLOW | FTS_LOGICAL | FTS_NOCHDIR;

  int i;

  // @todo - Buffer current is not thread safe, copy it to buffer_current_local, or do work inside mutex.
  while (buffer_current <= buffer_size) {
    pthread_mutex_lock(&mutex);
      unsigned char local_buffer[buffer_current];
      int local_buffer_current = buffer_current;

      next_buffer(buffer, &buffer_current, buffer_pos_max);
      copy_buffer(buffer, local_buffer, buffer_current);
      
      printf("Thread %d: buffer=", local);
      printHexBuffer(buffer, buffer_current);
      printf("\nThread %d: local_buffer=", local);
      printHexBuffer(local_buffer, local_buffer_current);
      printf("\n");
    pthread_mutex_unlock(&mutex);

    if ((ftsp = fts_open(filepaths, fts_options, NULL)) == NULL) {
      warn("fts_open");
      return ;
    }
    chp = fts_children(ftsp, 0);
    if (chp == NULL) {
      return 0;  // no files to traverse
    }

    while ((p = fts_read(ftsp)) != NULL) {
      switch (p->fts_info) {
        case FTS_F:
          //printf("Thread %d: File=%s\n", local, p->fts_path);
          ret_val = search(p->fts_path, local_buffer, local_buffer_current);
          break;
        default:
          break;
      }

      if (ret_val > 1) {
        //g_hit_counter++;
        //break; // Stop on a match.
      }
    }
    fts_close(ftsp);

    printf("\n\n\n Matches: %d\n",);
  }

  return;
}

/**
 *  Automatically generate input buffers for r_search.
 */
void next_buffer(unsigned char *buffer, int *buffer_current, int buffer_pos_max) {
  int i;
  for (i = *buffer_current; i >= 0; i--) {
    if (buffer[i] < buffer_pos_max) {
      buffer[i]++;
      break;
    }
    if (buffer[i] == buffer_pos_max) {
      buffer[i] = 0;
    }

    if (i == 0 && buffer[i] == 0) {
      (*buffer_current)++;
    }
  }
}

void copy_buffer(unsigned char *src, unsigned char *dst, int size) {
  int i;
  for (i = 0; i <= size; i++) {
    dst[i] = src[i];
  }
}

/**
 * Print Hex Buffer helper method.
 */
void printHexBuffer (unsigned char *buffer, int len) {
  //printf("\nPrint Buffer: size=%d, first=%X\n", len, buffer[0]);
  int i;
  for (i = 0; i <= len; i++) {
    printf("%.2X ", buffer[i]);
  }
  return;
}

/**
 * Takes an uninitialized char * and initializes it to the correct length given an arbitrary buffer.
 *   ** = pointer to pointer - this is for immutables like strings.
 *  CALL
 *    char * buf;
 *    convert_hex_pattern_to_string(&buf, hex_pattern);
 */
convert_hex_pattern_to_string (char **storage_buffer, unsigned char * hex_pattern) {
  *storage_buffer = (char *)malloc(sizeof(hex_pattern) + sizeof(hex_pattern)-1);
  int i = 0;
  for (; i < sizeof(hex_pattern) - 2; i++) {
    sprintf(*storage_buffer + strlen(*storage_buffer), "%2X ", hex_pattern[i]);
  }
}
