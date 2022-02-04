#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

void
func(int left)
{
  int first_num;
  if(read(left, &first_num, 4) != 4){
    fprintf(2, "primes: read failure\n");
  }
  printf("prime %d\n", first_num);

  int p[2];
  pipe(p);

  int num = 0;
  while (1) {
    int status = read(left, &num, 4);
    if (status == 0) {
      break;
    } else if (status != 4) {
      fprintf(2, "primes: read failure\n");
    } else if (status == 4) {
      if (num % first_num != 0) {
        if (write(p[1], &num, 4) != 4){
          fprintf(2, "primes: write failure\n");
        }
      }
    }
  }

  if (num == 0) {
    close(p[0]);
    close(p[1]);
    exit(0);
  } else {
    if (fork() == 0) {
      close(p[1]);
      func(p[0]);
    } else {
      close(p[0]);
      close(p[1]);
      wait(0);
      exit(0);
    }
  }
}

int
main(int argc, char *argv[])
{
  int p[2];
  pipe(p);

  for(int num = 2; num <= 35; num++)
    if(write(p[1], &num, 4) != 4){
      fprintf(2, "primes: write failure\n");
    }

  if(fork() == 0){
    close(p[1]);
    func(p[0]);
    exit(0);
  } else {
    close(p[0]);
    close(p[1]);
    wait(0);
    exit(0);
  }
}
