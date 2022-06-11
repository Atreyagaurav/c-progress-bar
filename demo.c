#include "c_progress.h"

int main(int argc, char **argv) {
  int i, j;
  double prog0, prog1, prog2;

  init_progress_bars(3, 2, 20, 100, 1);
  char status1[100], status2[100];
  
  start_progress_bar(0, "Test-0");
  for (j=0; j < 100; j++){
    prog0 = (j+1)*2.0;
    if (prog0 < 100.0){
      update_progress_bar(0, prog0);
    }else{
      update_progress_bar(0, 100.0);
      mark_bar_completed(0);
    }
    sprintf(status1, "Subprocess %d of 100", j+1);
    update_status(0, status1);
    print_all_progress();
    usleep(50000 / (j + 1));
  }
  mark_bar_completed(0);

  start_progress_bar(2, "Test-2");
  for (i=0; i < 5; i++){
    if (i == 1){
      start_progress_bar(1, "Test-1");
    }else if (i == 4){
      mark_bar_completed(2);
    }
    for (j=0; j < 100; j++){
      prog2 = (i*100+j+1)/4.0;
      prog1 = ((i-1)*100+j+1)/4.0;
      if (i > 0){
	update_progress_bar(1, prog1);
	sprintf(status2, "Subprocess %d of %d", (i-1)*100+j+1, 400);
	update_status(1, status2);
      }
      if (i < 4){
	update_progress_bar(2, prog2);
      }
      sprintf(status1, "Subprocess %d of %d", i*100+j+1, 400);
      update_status(0, status1);
      print_all_progress();
      usleep(200000 / (j + 1));
    }
  }
  mark_bar_completed(1);
}
