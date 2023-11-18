#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

#define SEM_NAME "/my_sem"
#define SHM_NAME "/my_shm"

struct shmseg {                 /* Defines structure of shared memory segment */
    int cnt;                    /* Number of bytes used in 'buf' */
    char buf[1024];         /* Data being transferred */
};

int main(int argc, char *argv[]) {
    int bytes, xfrs;
    struct shmseg *shmp;
    // union semun dummy;
    sem_t *sem;
    int fd;

    // create semaphore
    // If semaphore exists, delete it and create a new one to reset value.
    sem = sem_open(SEM_NAME, 0);
    if (sem != SEM_FAILED) {
        sem_close(sem);
        sem_unlink(SEM_NAME);
    }
    sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    if(sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    // create shared memory
    fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if(fd == -1) {
        perror("shm_open");
        return 1;
    }


    // set memory size
    ftruncate(fd, sizeof(struct shmseg));

    // mapping
    shmp = mmap(NULL, sizeof(struct shmseg), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    // int value;
    // sem_getvalue(sem, &value);
    // printf("Semaphore value: %d\n", value);

    /* Transfer blocks of data from stdin to shared memory */
    // xfrs = transfers (the number of times the transfer happened?)
    printf("type in messages, and type \"quit\" to quit.\n");
    for (xfrs = 0, bytes = 0; ; xfrs++, bytes += shmp->cnt) {
        /* Wait for our turn */
        if (sem_wait(sem) == -1) {
            perror("sem_wait");
            return 1;
        }

        if(fgets(shmp->buf, sizeof(shmp->buf), stdin) == NULL) {
            perror("fgets");
            return 1;
        }
        if(strcmp(shmp->buf, "quit\n") == 0) {
            shmp->cnt = 0;
        } else {
            shmp->cnt = strlen(shmp->buf);
        }

        if(sem_post(sem) == -1) {
            perror("sem_post");
            return 1;
        }

        usleep(1);

        if(shmp->cnt == 0) {
            break;
        }
    }

    /* Wait until reader has let us have one more turn. We then know reader has finished, and so we can delete the IPC objects. */

    if (sem_wait(sem) == -1) {
        perror("sem_wait");
        return 1;
    }

    if(sem_close(sem) == -1) {
        perror("sem_close");
        return 1;
    }
    if(sem_unlink(SEM_NAME) == -1) {
        perror("sem_unlink");
        return 1;
    }

    if( munmap(shmp, sizeof(struct shmseg)) == -1) {
        perror("munmap");
        return 1;
    }

    if(close(fd) == -1) {
        perror("close");
        return 1;
    }
    if(shm_unlink(SHM_NAME) == -1) {
        perror("shm_unlink");
        return 1;
    }

    fprintf(stderr, "Sent %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}
