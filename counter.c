#include <stdio.h>

// @note - Given a particular buffer and a set of limits you can always determine the next buffer
// @todo - Create a next buffer method that only does one iteration.

int r_reset_buffer(int *buffer, int current_rewind_position, int current_buffer_position, int max_buffer_position_size) {
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
      return r_reset_buffer(buffer, current_rewind_position - 1, current_buffer_position, max_buffer_position_size);
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
    for (j = 0; j <= max_buffer_position_size; j++) {
      buffer[current_buffer_position] = j;

      for (k = 0; k < current_buffer_position; k++) {
        printf("%2x ", buffer[k]);  
      }

      printf("%2x\n", buffer[current_buffer_position]);

      // if current is max we need to recursively move back
      if(j == max_buffer_position_size) {
        if (r_reset_buffer(buffer, current_rewind_position, current_buffer_position, max_buffer_position_size) > 0) {
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
