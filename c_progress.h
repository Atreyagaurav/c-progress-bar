/*
 * Simple progress bar implementation in C. 
 *
 * Originally From: https://github.com/c-progress-bar/c-progress-bar
 */
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

/* Specify how wide the progress bar should be. */
#define PROGRESS_BAR_WIDTH 50

/*
 * Alternative progress bar where each block grows
 * vertically instead of horizontally.
 */
/* #define VERTICAL */

/* Various unicode character definitions. */
#define BAR_START "\u2595"
#define BAR_STOP  "\u258F"
#define PROGRESS_BLOCK     "\u2588"


#define NUM_SUBBLOCKS (sizeof(subprogress_blocks) / sizeof(subprogress_blocks[0]))


struct progress_info {
  int initialized;
  int numprocess;
  int statuslen;
  char *status;
  int labellen;
  char** label;
  double* percentage;
  uint64_t* start;
};


/* Helper function to get the current time in usecs past the epoch. */
uint64_t get_timestamp(void);

/* Helper function to print a usecs value as a duration. */
void print_timedelta(uint64_t delta);

void print_single_progress(int num);

void print_multiple_progress();

void init_progress_bars(int num, int status_len, int label_len);

void start_progress_bar(int index, char* label);

void update_progress_bar(int index, double percentage);

void update_status(char *status);

void free_progress_bars(int num);

void print_multiple_progress();
