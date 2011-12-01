#include <dirent.h>
#include <stdio.h>
#include <string.h>

int r_search(char *);

int main(int argc, char *argv[])
{
  r_search(argv[1]);
 return(0);
}

//Recursively search the directories
int r_search(char *dstart) {
//printf("===========\nEntering r_search\ndstart: %s\n", dstart);
  DIR *d = NULL;
  struct dirent *dir = NULL;
  d = opendir(dstart);
//printf("After opening: dstart = %s\n", dstart);
  if(d) {
//printf("d exists: dstart = %s\n", dstart);
    // Get next dir
    while ( (dir = readdir(d)) != NULL) {
//printf("Inside loop: dstart = %s\n", dstart);
      // File
      if(dir->d_type == DT_REG) {
//printf("%s | File | %i\n", dir->d_name, dir->d_reclen);
      // Directory
      } else if (dir->d_type == DT_DIR) { 
//printf("%s | Directory | %i\n", dir->d_name, dir->d_reclen);
        //Dont create runtime loops
        if( (strcmp(dir->d_name, "..")==0) || (strcmp(dir->d_name, ".")==0) ) { // Fail on loops
//printf("Directory is blacklisted: dstart = %s\n", dstart);
        } else {  
//printf("Directory is OK\n");
//printf("dstart before path: %s\n", dstart);
          char path[strlen(dstart) + strlen("/") + strlen(dir->d_name)];
	  strcpy(path, "");
//printf("Path=%s | ", path);
          strcat(path, dstart);
//printf("Path=%s | ", path);
          if(strcmp(dstart, "/") != 0) {
            strcat(path, "/");
          }
//printf("Path=%s | ", path);
          strcat(path, dir->d_name);
//printf("dstart after path: %s\n", dstart);
//printf("path: %s\n", path);
//printf("Calling r_search: dstart = %s | path = %s\n===========\n", dstart, path);
          int ret_val = r_search(path);
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
 return(0);
}
