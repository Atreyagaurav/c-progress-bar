#include "c_progress.h"

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
