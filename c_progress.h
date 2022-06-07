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

enum progress_status{QUEUED, INPROGRESS, CANCELLED, COMPLETED, FAILED};


struct progress_bar {
  enum progress_status status;
  int labellen;
  int index;
  char* label;
  double percentage;
  uint64_t start;
  uint64_t last_updated;
};


struct progress_info {
  int initialized;
  int bars_count;
  int statuslen;
  int status_count;
  char **status;
  struct progress_bar **bars;
};


/* Helper function to get the current time in usecs past the epoch. */
uint64_t get_timestamp(void);

/* Helper function to print a usecs value as a duration. */
void print_timedelta(uint64_t delta);

void print_bar(struct progress_bar* pb);

void print_multiple_progress();

void init_progress_bars(int num, int status_num, int label_len, int status_len);

void start_progress_bar(int index, char* label);

void update_progress_bar(int index, double percentage);

void update_status(int index, char *status);

void free_progress_bars();

void print_multiple_progress();

void print_line(char *line);
