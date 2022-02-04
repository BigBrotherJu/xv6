#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int p1[2], p2[2];
  if(pipe(p1) < 0){
    fprintf(2, "pingpong: pipe failure\n");
  }
  if(pipe(p2) < 0){
    fprintf(2, "pingpong: pipe failure\n");
  }

  char buf[1];
  int pid;
  if(fork() == 0){
    // child read from parent
    if(read(p1[0], buf, 1) != 1){
      fprintf(2, "pingpong: read failure in child\n");
    }
    // child print info
    pid = getpid();
    fprintf(1, "%d: received ping\n", pid);
    // child write to parent
    if(write(p2[1], buf, 1) != 1){
      fprintf(2, "pingpong: write failure in child\n");
    }

    exit(0);
  } else {
    // parent write to child
    if(write(p1[1], buf, 1) != 1){
      fprintf(2, "pingpong: write failure in parent\n");
    }
    // parent read from child
    if(read(p2[0], buf, 1) != 1){
      fprintf(2, "pingpong: read failure in child\n");
    }
    // parent print info
    pid = getpid();
    fprintf(1, "%d: received pong\n", pid);

    exit(0);
  }
}
