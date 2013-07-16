#include <stdio.h>

int reset_buffer(int *buffer, int current_rewind_position, int current_buffer_position, int max_buffer_position_size) {
  int i = 0;

  if (current_rewind_position >= 0) {    
    if (buffer[current_rewind_position] < max_buffer_position_size) {
      buffer[current_rewind_position]++;

      for (i = current_rewind_position + 1; i <= current_buffer_position; i++) {
        buffer[i] = 0;
      }

      return 0;
    }
    else if (current_rewind_position - 1 >= 0) {
      return reset_buffer(buffer, current_rewind_position - 1, current_buffer_position, max_buffer_position_size);
    }
    else {
    }
  }
  
  return 1;
}

int main(void) {

  int buffer[6] = { 0 };
  int current_buffer_position = 0;
  int current_rewind_position = -1;

  int max_buffer_positions = 6;
  int max_buffer_position_size = 255;

  // @todo - min buffer position size
  // @todo - min buffer position size
 
  int i, j, k, l;
  for (i = 0; i < max_buffer_positions; i++) {
    //printf("current_buffer_position=%d, current_rewind_position=%d, current_buffer=%d, rewind_buffer=%d\n", current_buffer_position, current_rewind_position, buffer[current_buffer_position], buffer[current_rewind_position]);

    for (j = 0; j <= max_buffer_position_size; j++) {
      buffer[current_buffer_position] = j;

      for (k = 0; k < current_buffer_position; k++) {
        printf("%2x ", buffer[k]);  
      }

      printf("%2x\n", buffer[current_buffer_position]);
      
      // if current is max we need to recursively move back
      if(j == max_buffer_position_size) {
        if (reset_buffer(buffer, current_rewind_position, current_buffer_position, max_buffer_position_size) > 0) {
          break;
        }
        else {
          j = 0;  
        }
      }
    }

    // Zero the buffer before the next run.
    for (l = 0; l <= current_buffer_position; l++) {
      buffer[l] = 0;
    }

    // Reset position counters.
    current_buffer_position++;
    current_rewind_position = current_buffer_position - 1;
  }

  return 0;
}

/*

current_buffer_position=2, current_rewind_position=1, current_buffer=0, rewind_buffer=00
00 00 01
00 ff ff
current_buffer_position=3, current_rewind_position=2, current_buffer=0, rewind_buffer=00
00 00 00 01
00 00 ff ff
current_buffer_position=4, current_rewind_position=3, current_buffer=0, rewind_buffer=00
00 00 00 00 01

go forward up to 6 positions

first position
  {
    count 0-255 for the current buffer position
    print entire buffer after each increment

    if current positiion in buffer is greater then 0 && itteration is 255
      check rewind buffer position value < 255
      increment by 1
    else
      move rewind position back 1 position
      increment by 1
    else
      exit
  }

1
2
3

1 1
1 2
1 3

2 1
2 2
2 3

3 1
3 2
3 3

0 0 0
0 0 1
0 0 2
0 0 3

0 1 0
0 1 1
0 1 2
0 1 3

0 2 0
0 2 1
0 2 2
0 2 3

0 3 0
0 3 1
0 3 2
0 3 3

1 0 0
1 0 1
1 0 2
1 0 3

1 1 0
1 1 1
1 1 2
1 1 3

1 2 0
1 2 1
1 2 2
1 2 3

1 3 0
1 3 1
1 3 2
1 3 3

2 0 0
2 0 1
2 0 2
2 0 3

2 1 0
2 1 1
2 1 2
2 1 3

3 2 1
3 2 2
3 2 3

3 3 1
3 3 2
3 3 3

1 1 1 1
1 1 1 2
1 1 1 3

1 1 2 1
1 1 2 2
1 1 2 3

1 1 3 1
1 1 3 2
1 1 3 3

1 2 1 1
1 2 1 2
1 2 1 3

1 2 2 1
1 2 2 2
1 2 2 3

1 2 3 1
1 2 3 2
1 2 3 3

1 3 1 1
1 3 1 2
1 3 1 3

1 3 2 1
1 3 2 2
1 3 2 3

1 3 3 1
1 3 3 2
1 3 3 3

2 1 1 1
2 1 1 2
2 1 1 3

2 1 2 1
2 1 2 2
2 1 2 3

2 1 3 1
2 1 3 2
2 1 3 3

2 2 1 1
2 2 1 2
2 2 1 3

2 2 2 1
2 2 2 2
2 2 2 3

2 2 3 1
2 2 3 2
2 2 3 3

2 3 1 1
2 3 1 2
2 3 1 3

2 3 2 1
2 3 2 2
2 3 2 3

2 3 3 1
2 3 3 2
2 3 3 3

3 1 1 1
etc

*/