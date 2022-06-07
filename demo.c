#include "c_progress.h"

int main(int argc, char **argv) {
  int i, j;
  double prog1, prog2;

  init_progress_bars(3, 2, 20, 100, 1);
  char status1[100], status2[100];
  start_progress_bar(0, "\e[33mTest1\e[0m");
  start_progress_bar(2, "\e[33mTest3\e[0m");
  for (j=0; j < 100; j++){
    prog1= (j+1)*2.0;
    if (prog1 < 100.0){
      update_progress_bar(0, prog1);
    }else{
      update_progress_bar(0, 100.0);
    }
    sprintf(status1, "Subprocess %d of 10", j+1);
    update_status(0, status1);
    print_all_progress();
    usleep(50000 / (j + 1));
  }
  for (i=0; i < 5; i++){
    if (i == 3){
      start_progress_bar(1, "\e[33mTest2\e[0m");
    }
    for (j=0; j < 100; j++){
      prog1= (i*100+j+1)/5.0;
      if (prog1 < 100.0){
	update_progress_bar(2, prog1);
      }else{
	update_progress_bar(2, 100.0);
      }
      if (i > 2){
	update_progress_bar(1, ((i-3)*100+j+1)/2.0);
	sprintf(status2, "Subprocess %d of %d", i*100+j+1, 500);
	update_status(1, status2);
      }
      sprintf(status1, "Subprocess %d of %d", i*100+j+1, 500);
      update_status(0, status1);
      print_all_progress();
      usleep(50000 / (j + 1));
    }
  }
}
