#include "os.h"

extern void print_proc_inodes();

void alloc_test() {
  Log("alloc_test begin...");
  Log("intr_status: %d", _intr_read());
  int *arr = (int*)pmm->alloc(20 * sizeof(int));
  Log("intr_status: %d", _intr_read());
  for (int i = 0; i < 20; i++){
    arr[i] = i;
  }
  for (int i = 0; i < 20; i++){
    Log("arr[%d]: %d", i, arr[i]);
  }
  pmm->free(arr);
  Log("intr_status: %d", _intr_read());
  pmm->alloc(16);
  pmm->alloc(20);
  pmm->alloc(24);
  pmm->alloc(120);
  pmm->alloc(120);
  pmm->alloc(120);
  pmm->alloc(120);
  pmm->alloc(120);
  pmm->alloc(120);
  pmm->alloc(120);
  pmm->alloc(120);
  pmm->alloc(120);
  pmm->alloc(120);
  pmm->alloc(120);
  pmm->alloc(120);
  pmm->alloc(120);
  Log("intr_status: %d", _intr_read());
  Log("_heap.end: 0x%x", _heap.end);
  Log("alloc_test end.\n============================");
}

static thread_t test_thread[5];

static void thread_test_func(void *tid){
    TRACE_ENTRY;
    int cnt;
    for (cnt = 0; cnt < 10; cnt++){
        Log("The %d thread prints: %d", (int)tid, cnt);
    }
    Log("The %d thread is finished.", (int)tid);
    while (1);
}

void thread_test() {
  Log("thread_test begin...");   
  int i;
  for (i = 0; i < 5; i++){
    kmt->create(&test_thread[i], thread_test_func, (void *)i);
    Log("Thread %d created.", i);
  }
/*  kmt->teardown(&test_thread[2]);
  Log("Thread 2 has been teardowned.");
  kmt->teardown(&test_thread[4]);
  Log("THread 4 has been teardowned.");*/
  Log("thread_test end\n============================");
}

static sem_t empty;
static sem_t fill;
static thread_t sem_test_thread[2];

static void producer(void *arg) {
  while (1){
    kmt->sem_wait(&empty);
    printf("(");
    kmt->sem_signal(&fill);
  }
}

static void consumer(void *arg) {
  while (1){
    kmt->sem_wait(&fill);
    printf(")");
    kmt->sem_signal(&empty);
  }
}

void sem_test() {
  Log("sem_test begin");
  kmt->sem_init(&empty, "empty", 20);
  kmt->sem_init(&fill, "fill", 0);
  kmt->create(&sem_test_thread[0], producer, NULL);
  kmt->create(&sem_test_thread[1], consumer, NULL);
  Log("sem_test end.");
//  printf("sem_test end\n============================");
}

static thread_t fs_test_thread[5];

static void fs_test_func(void *tid){
    while (1);
}

void fs_init_test() {
  Log("fs_init_test begin");
  int i;
  for (i = 0; i < 5; i++){
    kmt->create(&fs_test_thread[i], fs_test_func, (void *)i);
    Log("Thread %d created.", i);
  }
  kmt->teardown(&fs_test_thread[2]);
  Log("Thread 2 deleted.");
  print_proc_inodes();
  int fd = vfs->open("/proc/1/status", O_RDWR);
  if (fd == -1){
    panic("open failed.\n");
  }
  char buf[200];
  if (vfs->read(fd, buf, sizeof(buf) - 1) == -1)
    panic("read failed");
  printf("buf------\n%s\n", buf);
  if (vfs->write(fd, "1234", 4) == -1)
    panic("write failed");
  if (vfs->read(fd, buf, sizeof(buf) - 1) == -1)
    panic("read failed");
  printf("buf------\n%s\n", buf);
  vfs->close(fd);
}