#include <stdio.h>

void next_buffer(int buffer[], int *buffer_current, int buffer_pos_max) {
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

int main (void) {
	int buffer_size = 6;
	int buffer_pos_max = 255;

	int buffer[buffer_size]; int i; for (i = 0; i < buffer_size; i++) { buffer[i] = 0; }
	int buffer_current = 0;

	int j;

	printf("Starting\n buffer_size: %d, buffer_current: %d\n", buffer_size, buffer_current);
	while (buffer_current <= buffer_size) {
		printf("  buffer_size: %d, buffer_current: %d\n", buffer_size, buffer_current);
		for (j = 0; j <= buffer_current; j++) {
			printf("%2x ", buffer[j]);
		}
		next_buffer(buffer, &buffer_current, buffer_pos_max);
	}
}