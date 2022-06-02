/*
 * Simple progress bar implementation in C. 
 *
 * Originally From: https://github.com/c-progress-bar/c-progress-bar
 */
#include <stddef.h>
#include <stdio.h>
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

#ifdef VERTICAL
static const char * subprogress_blocks[] = { " ",
                                             "\u2581",
                                             "\u2582",
                                             "\u2583",
                                             "\u2584",
                                             "\u2585",
                                             "\u2586",
                                             "\u2587" 
};
#else
static const char * subprogress_blocks[] = { " ",
                                             "\u258F",
                                             "\u258E",
                                             "\u258D",
                                             "\u258C",
                                             "\u258B",
                                             "\u258A",
                                             "\u2589"
};
#endif

#define NUM_SUBBLOCKS (sizeof(subprogress_blocks) / sizeof(subprogress_blocks[0]))


struct progress_info {
  int initialized;
  int numprocess;
  char** label;
  double* percentage;
  uint64_t* start;
} global_pi;


/* Helper function to get the current time in usecs past the epoch. */
static uint64_t get_timestamp(void) {
    struct timeval tv;
    uint64_t stamp = 0;
    gettimeofday(&tv, NULL);
    stamp = tv.tv_sec * 1000000 + tv.tv_usec;
    return stamp;
}

/* Helper function to print a usecs value as a duration. */
static void print_timedelta(uint64_t delta) {

    uint64_t delta_secs = delta / 1000000;
    uint64_t hours    = delta_secs / 3600;
    uint64_t minutes  = (delta_secs - hours * 3600) / 60;
    uint64_t seconds  = (delta_secs - hours * 3600 - minutes * 60);
    uint64_t mseconds = (delta / 100000) % 10;

    if (hours) {
        printf("%lluh %llum %llus    ", hours, minutes, seconds);
    }
    else if (minutes) {
        printf("%llum %02llus        ", minutes, seconds);
    }
    else {
        printf("%llu.%llus           ", seconds, mseconds);
    }
}

/*
 * Main interface function for updating the progress bar. This
 * function doesn't print a newline, so you can call it iteratively
 * and have the progress bar grow across the screen. The caller can
 * print a newline when the're ready to go to a new line.
 *
 * percentage: a double between 0.0 and 100.0 indicating the progress.

 * start: usecs timestamp for when the task started, for calculating
 *        remaining time.
 */
void print_single_progress(int num) {
    size_t i;
    size_t total_blocks = PROGRESS_BAR_WIDTH * NUM_SUBBLOCKS;
    size_t done = *(global_pi.percentage + num) / 100 * total_blocks;
    size_t num_blocks = done / NUM_SUBBLOCKS;
    size_t num_subblocks = done % NUM_SUBBLOCKS;

    uint64_t now = get_timestamp();
    uint64_t elapsed = now - *(global_pi.start + num);
    uint64_t estimated_total = elapsed / (*(global_pi.percentage + num) / 100.0);
    uint64_t remaining = estimated_total - elapsed;

    printf("\r  %s: %6.2f%% \t%s", *(global_pi.label + num), *(global_pi.percentage + num), BAR_START);

    for (i = 0; i < num_blocks; i++) {
        printf("%s", PROGRESS_BLOCK);
    }

    if (num_subblocks) {
        printf("%s", subprogress_blocks[num_subblocks]);
        i++;
    }

    for (; i < PROGRESS_BAR_WIDTH; i++) {
        printf(" ");
    }

    
    printf("%s\t", BAR_STOP);

    if (*(global_pi.percentage + num) < 100.0) {
        printf("ETA: ");
        print_timedelta(remaining);
    }
    else {
        printf("\x1b[K"); 	/* delete till end of line */
    }
    fflush(stdout);
}


void print_multiple_progress();

void init_progress_bars(int num){
  global_pi.initialized = 0;
  global_pi.numprocess = num;
  global_pi.label = malloc(sizeof(char*)*num);
  global_pi.percentage = malloc(sizeof(double*)*num);
  global_pi.start = malloc(sizeof(uint64_t*)*num);
}

void set_progress_bar(int index, char* label, double percentage, uint64_t start){
  *(global_pi.label + index) = label;
  *(global_pi.percentage + index) = percentage;
  *(global_pi.start + index) = start;
}

void update_progress_bar(int index, double percentage){
  *(global_pi.percentage + index) = percentage;
}

void free_progress_bars(int num){
  global_pi.numprocess = 0;
  free(global_pi.label);
  free(global_pi.percentage);
  free(global_pi.start);
}

void print_multiple_progress(){
  int i;
  if (global_pi.initialized){
    /* goto previous lines */
    printf("\x1b[%dA", global_pi.numprocess);
  }else{
    global_pi.initialized = 1;
  }
  for (i=0; i< global_pi.numprocess; i++){
    print_single_progress(i);
    printf("\n");
  }
}


int main(int argc, char **argv) {
    int i;
    double amount = 0.0;
    uint64_t start = get_timestamp();

    init_progress_bars(2);
    set_progress_bar(0, "\e[33mTest\e[0m", 0.0, start);
    set_progress_bar(1, "Test2", 0.0, start);
    for (i = 0; i < 10000; i++) {
        amount += 0.01;
	update_progress_bar(0, amount);
        print_multiple_progress();
        usleep(20000 / (i + 1));
    }

    printf("\n");
}
