#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "stdbool.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
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
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
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

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
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

/* Syscall de los semaforos */

struct sema_phore {
  int value;
  struct spinlock lock;
  bool open;
  
};

#define MAX_SEMAPHORES 100
struct sema_phore semaphores[MAX_SEMAPHORES];  // Array de semáforos

uint64
sys_sem_open (void)
{
  int sem, value;

  argint(0, &sem);
  argint(1, &value);
  /* Como se puede ver en otras system calls la funcion argint se utiliza
  para extraer los argumentos enteros que son pasados por un programa a
  nivel de usuario.
  En este caso entonces el primer argumento es sem y el segundo value*/

  if (sem < 0 || sem > MAX_SEMAPHORES) {
    return 0;
  }

  initlock(&semaphores[sem].lock, "sem");
  /* Inicializa el lock con el nombre que le pasamos, sin estar bloqueado
  y con cpu en 0 lo que probablemente significa que ninguna cpu lo tiene actualmente*/

  acquire(&(semaphores[sem].lock));//cierro el lock para que no entren dos o mas procesos

  if (semaphores[sem].open == false){ // El semaforo no estaba abierto
    semaphores[sem].open = true;
    semaphores[sem].value=value;//hago un semaforo que puede soportar hasta value procesos,este valor se modifica a medida que agrego o quito procesos
    release(&(semaphores[sem].lock));//ahora abro el lock para que otros procesos puedan entrar al semaforo o no dependiendo del limite del semaforo
  }
  else {
    release(&(semaphores[sem].lock));
    return 0;
  }

  return 1;
}

uint64
sys_sem_close (void)
{
  int sem;
  argint(0, &sem);

  acquire(&semaphores[sem].lock);
  if(semaphores[sem].open) {
    semaphores[sem].open = false;
    semaphores[sem].value = 0;
    release(&(semaphores[sem].lock));
    return 1;
  }
  release(&(semaphores[sem].lock));
  return 0;
}

uint64
sys_sem_up (void)
{
  int sem;
  argint(0, &sem);

  acquire(&(semaphores[sem].lock));

  int value = semaphores[sem].value;

  if (value == 0){
    wakeup(&semaphores[sem]);
  }

  semaphores[sem].value = semaphores[sem].value+1;

  release(&(semaphores[sem].lock));
  return 1;
}

uint64
sys_sem_down (void)
{
  int sem;
  argint(0, &sem);
  
  acquire(&(semaphores[sem].lock));

  while (semaphores[sem].value == 0) { 
    /* Es necesario el while ya que puede suceder algo que se llama
    "Spurious wakeup". Puede suceder cuando una thread se despierta 
    cuando no deberia (spurious viene de que aparentemente se desperto sin razon) */

    sleep(&(semaphores[sem]), &(semaphores[sem].lock));
  }

  // A este punto, el valor del lock esta garantizada a ser positiva
  semaphores[sem].value = semaphores[sem].value - 1;
    
  release(&(semaphores[sem].lock));
  return 1;
}
