// Must do the following for the dynamic loader to work.
// For Mac
// sudo ln -s /usr/local/mysql/lib/libmysqlclient.18.dylib /usr/lib/libmysqlclient.18.dylib

// Compile - 
//gcc sql_test.c -stf=c99 -I/usr/local/mysql-5.5.29-osx10.6-x86_64/include/ `mysql_config --cflags --libs`

// http://zetcode.com/db/mysqlc/


#include <my_global.h>
#include <mysql.h>

int insert_data(MYSQL * mysql, char * pattern, char * os, int total_matches, int files_scanned, 
	int num_files_with_match, char * file_most_matches, int time_start, int time_first_match, 
	int time_last_match, int time_end, int len)
{
    char statement[4095];
    //pattern, OS, total_matches, files_scanned, num_files_with_match, 
    // file_most_matches, time_start, time_first_match, time_last_match, time_end
    // '00 01 02 03', 'Mac', 1, 2, 3,
    // '/etc/hosts', 1234567891, 1234567892, 1234567893, 1234567894
    snprintf(statement, sizeof statement, 
    	"INSERT INTO `binary_stats` (`pattern`, `OS`, `total_matches`, `files_scanned`, `num_files_with_match`, `file_most_matches`, `time_start`, `time_first_match`, `time_last_match`, `time_end`) VALUES ('%s', '%s', %d, %d, %d, '%s', %d, %d, %d, %d)",
    	pattern, os, total_matches, files_scanned, num_files_with_match,
  		file_most_matches, time_start, time_first_match, time_last_match, time_end
  	);
  	printf("%s\n", statement);
    return mysql_query(mysql, statement);
}


int main(int argc, char **argv)
{
  //printf("MySQL client version: %s\n", mysql_get_client_info());
  //exit(0);

  MYSQL *con = mysql_init(NULL);

  if (con == NULL) 
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      exit(1);
  }

  if (mysql_real_connect(con, "localhost", "drupal", "password", "binary_count", 0, NULL, 0) == NULL) 
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      mysql_close(con);
      exit(1);
  }

  char *pattern = "00 01 02 03";
  char *os = "Ubuntu";
  int total_matches = 1;
  int files_scanned = 2; 
	int num_files_with_match = 3;
	char *file_most_matches = "/etc/hosts";
	int time_start = 1234567891;
	int time_first_match = 1234567892;
	int time_last_match = 1234567893;
	int time_end = 1234567894;

  if (insert_data(
  		con,
  		pattern, os, total_matches, files_scanned, num_files_with_match,
  		file_most_matches, time_start, time_first_match, time_last_match, time_end,
  		-1
  	)) 
  {
      fprintf(stderr, "%s\n", mysql_error(con));
      mysql_close(con);
      exit(1);
  }

  mysql_close(con);
  exit(0);
}