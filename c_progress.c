#include "c_progress.h"
#include <stdio.h>

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

struct progress_info global_pi;

/* Helper function to get the current time in usecs past the epoch. */
uint64_t get_timestamp(void) {
    struct timeval tv;
    uint64_t stamp = 0;
    gettimeofday(&tv, NULL);
    stamp = tv.tv_sec * 1000000 + tv.tv_usec;
    return stamp;
}

/* Helper function to print a usecs value as a duration. */
void print_timedelta(uint64_t delta) {

    uint64_t delta_secs = delta / 1000000;
    uint64_t hours    = delta_secs / 3600;
    uint64_t minutes  = (delta_secs - hours * 3600) / 60;
    uint64_t seconds  = (delta_secs - hours * 3600 - minutes * 60);
    uint64_t mseconds = (delta / 100000) % 10;

    if (hours) {
        printf("%luh %lum %lus    ", hours, minutes, seconds);
    }
    else if (minutes) {
        printf("%lum %02lus        ", minutes, seconds);
    }
    else {
        printf("%lu.%lus           ", seconds, mseconds);
    }
}

/*
 * Main interface function for updating the progress bar. This
 * function doesn't print a newline, so you can cal it iteratively
 * and have the progress bar grow across the screen. The caler can
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

    printf("\r\x1b[K  %s: %6.2f%% \t%s", *(global_pi.label + num), *(global_pi.percentage + num), BAR_START);

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
        printf("\x1b[K"); 	/* delete til end of line */
    }
    fflush(stdout);
}


void init_progress_bars(int bar_num, int status_num, int label_len, int status_len){
  int i;
  global_pi.initialized = 0;
  global_pi.bars_count = bar_num;
  global_pi.statuslen = status_len;
  global_pi.labellen = label_len;
  global_pi.status = (char**)malloc(sizeof(char*)*bar_num);
  global_pi.status_count = status_num;
  for (i=0; i< status_num; i++){
    *(global_pi.status+i) = (char*)malloc(sizeof(char)*status_len);
    **(global_pi.status+i) = '\0';
  }
  global_pi.label = (char**)malloc(sizeof(char*)*bar_num);
  for (i=0; i< bar_num; i++){
    *(global_pi.label+i) = (char*)malloc(sizeof(char)*label_len);
  }
  global_pi.percentage = (double*)malloc(sizeof(double)*bar_num);
  global_pi.start = (uint64_t*)malloc(sizeof(uint64_t)*bar_num);
}

void start_progress_bar(int index, char* label){
  strcpy(*(global_pi.label + index), label);
  *(global_pi.percentage + index) = 0.0;
  *(global_pi.start + index) = get_timestamp();
}

void update_progress_bar(int index, double percentage){
  *(global_pi.percentage + index) = percentage;
}

void update_status(int index, char *status){
  int i, len;
  len = strlen(status);
  for (i=0; i < len && i < global_pi.statuslen; i++){
    *(*(global_pi.status + index)+i) = *(status+i);
  }
  if (len > global_pi.statuslen){
    /* the numbers here are temporary. For now I want to see the filename and extension. */
    strcpy(*(global_pi.status + index) + global_pi.statuslen - 7, "...");
    for (i=0; i < 5; i++){
      *(*(global_pi.status + index) + global_pi.statuslen - i) = *(status + len - i);
    }
    *(*(global_pi.status + index) + global_pi.statuslen) = '\0';
  }else{
    *(*(global_pi.status + index) + len) = '\0';
  }
}

void free_progress_bars(int num){
  int i;
  global_pi.bars_count = 0;
  for (i=0; i< num; i++){
    free(*(global_pi.label+i));
  }
  free(global_pi.label);
  free(global_pi.percentage);
  free(global_pi.start);
}

void print_multiple_progress(){
  int i;
  if (global_pi.initialized){
    /* goto previous lines to reach start line*/
    printf("\x1b[%dA", global_pi.bars_count +
	   global_pi.status_count);
  }else{
    global_pi.initialized = 1;
  }
  for (i=0; i< global_pi.bars_count; i++){
    print_single_progress(i);
    printf("\n");
  }
  for (i=0; i< global_pi.status_count; i++){
    printf("\x1b[K%s\n", *(global_pi.status+i));
  }
}

void print_line(char *line){
  if (global_pi.initialized){
    /* goto previous lines to reach start line*/
    printf("\x1b[%dA", global_pi.bars_count +
	   global_pi.status_count);
    global_pi.initialized = 0;
  }
  printf("%s\x1b[K\n", line);
  print_multiple_progress();
}


int main(int argc, char **argv) {
  int i, j;

  init_progress_bars(2, 2, 20, 100);
  char status1[100], status2[100];
  start_progress_bar(0, "\e[33mTest\e[0m");
  start_progress_bar(1, "Test2");
  for (i = 0; i < 100; i++) {
    update_progress_bar(1, i*1.0);
    sprintf(status2, "Process %d of 1000", i+1);
    update_status(1, status2);
    if (i % 25 == 0){
      print_line("LOG: Some text log here.");
    }

    for (j=0; j< 1000; j++){
      update_progress_bar(0, j/10.0);
      sprintf(status1, "Subprocess %d of 1000", j+1);
      update_status(0, status1);
      print_multiple_progress();
      usleep(50000 / (i + j + 1));
    }
  }
}
