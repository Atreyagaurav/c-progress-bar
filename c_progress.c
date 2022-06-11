#include "c_progress.h"
#include <stdio.h>
#include <time.h>

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

#define PERC_MAX 100.0

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
        printf("%luh %lum %lus", hours, minutes, seconds);
    }
    else if (minutes) {
        printf("%lum %02lus", minutes, seconds);
    }
    else {
        printf("%lu.%lus", seconds, mseconds);
    }
}

void print_bar(struct progress_bar* pb) {
    size_t i;
    size_t total_blocks = PROGRESS_BAR_WIDTH * NUM_SUBBLOCKS;
    size_t done = pb->percentage / PERC_MAX * total_blocks;
    size_t num_blocks = done / NUM_SUBBLOCKS;
    size_t num_subblocks = done % NUM_SUBBLOCKS;

    uint64_t now = get_timestamp();
    uint64_t elapsed = now - pb->start;
    uint64_t estimated_total = elapsed / pb->percentage * PERC_MAX;
    uint64_t remaining = estimated_total - elapsed;

    printf("\r\x1b[K  %s: %6.2f%% \t%s",
	   pb->label, pb->percentage * 100.0 / PERC_MAX, BAR_START);

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

    if (pb->status == INPROGRESS) {
      printf("ETA: ");
      print_timedelta(remaining);
    }
    else if(pb->status == COMPLETED) {
      const time_t s = (int)(now/1e6);
      struct tm * current_time;
      current_time = localtime(&s);
 
      printf("Cmpl: ");
      printf("%02d:%02d:%02d",
	     current_time->tm_hour,
	     current_time->tm_min,
	     current_time->tm_sec);
    }
    printf("\x1b[K"); 	/* delete til end of line */
    fflush(stdout);
}

struct progress_bar* init_progress_bar(int index, int labellen){
  struct progress_bar *pb;
  pb = (struct progress_bar*)malloc(sizeof(struct progress_bar));
  pb->label = (char*)malloc(sizeof(char)*labellen);
  *(pb->label) = '\0';
  pb->labellen = labellen;
  pb->index = index;
  pb->start = get_timestamp();
  pb->last_updated = get_timestamp();
  pb->status = QUEUED;
  pb->percentage = 0.0;
  return pb;
}


void free_progress_bar(struct progress_bar* pb){
  free(pb->label);
  free(pb);
}


void init_progress_bars(int bar_num, int status_num, int label_len, int status_len, int show_active_only){
  int i;
  global_pi.initialized = 0;
  global_pi.show_active_only = show_active_only;
  global_pi.bars_count = bar_num;
  global_pi.statuslen = status_len;
  global_pi.bars = (struct progress_bar**)malloc(sizeof(struct progress_bar*)*bar_num);
  for (i=0; i < bar_num; i++){
    *(global_pi.bars+i) = init_progress_bar(i, label_len);
  }
  global_pi.status = (char**)malloc(sizeof(char*)*bar_num);
  global_pi.status_count = status_num;
  for (i=0; i< status_num; i++){
    *(global_pi.status+i) = (char*)malloc(sizeof(char)*status_len);
    **(global_pi.status+i) = '\0';
  }
}

struct progress_bar* get_progress_bar(int index){
  struct progress_bar *pb = (*(global_pi.bars + index));
  return pb;
}

void start_progress_bar(int index, char* label){
  struct progress_bar *pb = (*(global_pi.bars + index));
  strcpy(pb->label, label);
  pb->percentage = 0.0;
  pb->start = get_timestamp();
  pb->status = INPROGRESS;
}

void update_progress_bar(int index, double percentage){
  struct progress_bar *pb = *(global_pi.bars + index);
  pb->percentage = percentage;
  pb->last_updated = get_timestamp();
}

void mark_bar_completed(int index){
  struct progress_bar *pb = *(global_pi.bars + index);
  if (pb->status != COMPLETED){
    pb->status = COMPLETED;
    goto_top_bar();
    print_bar(pb);
    printf("\n");
    global_pi.initialized = 0;
    print_all_progress();
  }
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

void free_progress_bars(){
  int i;
  for (i=0; i< global_pi.bars_count; i++){
    free_progress_bar(*(global_pi.bars + i));
  }
  global_pi.bars_count = 0;

  for (i=0; i< global_pi.status_count; i++){
    free(global_pi.status + i);
  }
  free(global_pi.status);
  global_pi.status_count = 0;
}

void goto_top_bar(){
  int i, lines;
  lines = (global_pi.show_active_only ?
	       global_pi.active_count : global_pi.bars_count)
    + global_pi.status_count;

  printf("\x1b[K");
  for (i=0; i<lines; i++){
    printf("\x1b[1A\x1b[K"); 	/* move up and clear line. */
  }
}

void print_all_progress(){
  int i;
  if (global_pi.initialized){
    goto_top_bar();
  }else{
    global_pi.initialized = 1;
  }
  global_pi.active_count = 0;
  for (i=0; i< global_pi.bars_count; i++){
    if (global_pi.show_active_only &&
	(*(global_pi.bars+i))->status != INPROGRESS){
      continue;
    }
    print_bar(*(global_pi.bars+i));
    /* fprintf(stderr, "%.2f\n", (*(global_pi.bars+i))->percentage); */
    global_pi.active_count += 1;
    printf("\n");
  }
  for (i=0; i< global_pi.status_count; i++){
    printf("\x1b[K%s\n", *(global_pi.status+i));
  }
}


void print_line(char *line){
  if (global_pi.initialized){
    goto_top_bar();
    global_pi.initialized = 0;
  }
  printf("%s\x1b[K\n", line);
  print_all_progress();
}

