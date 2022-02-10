#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;


  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}


#ifdef LAB_PGTBL
// BASE is the starting virtual address of the first
// user page to check, LEN is the number of pages to
// check, MASK is a user address to a buffer storing
// the result
uint64
sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  uint64 base;
  int len;
  uint64 mask;

  unsigned int result = 0;
  pte_t *pte;
  struct proc *p = myproc();

  if (argaddr(0, &base) < 0)
    return -1;
  if (argint(1, &len) < 0)
    return -1;
  if (argaddr(2, &mask) < 0)
    return -1;

  if (len > 32)
    return -1;

  for (int i = 0; i < len; i += 1) {
    if ((pte = walk(p->pagetable, base + i * PGSIZE, 0)) == 0)
      return -1;
    if((*pte & PTE_V) && (*pte & (PTE_R|PTE_W|PTE_X))
       && (*pte & PTE_A)){
      result += (1 << i);
      // printf("%d: %d\n", i, result);
      *pte = (*pte)&(~PTE_A);
    }
  }

  if (copyout(p->pagetable, mask, (char*)&result, sizeof(result)) < 0)
    return -1;
  return 0;
}

uint64
sys_printpgtbl(void)
{
  struct proc *p = myproc();
  vmprint(p->pagetable);
  return 0;
}

#endif

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
