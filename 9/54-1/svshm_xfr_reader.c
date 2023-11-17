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
    int xfrs, bytes;
    struct shmseg *shmp;
    sem_t *sem;
    int fd;

    /* Get IDs for semaphore set and shared memory created by writer */
    // semid = semget(SEM_KEY, 0, 0);
    // if (semid == -1)
    //     errExit("semget");
    sem = sem_open(SEM_NAME, 0); // ignored if semaphore already exists
    if(sem == SEM_FAILED) {
        perror("sem_open");
        return 1;
    }

    int value;
    sem_getvalue(sem, &value);
    printf("Semaphore value: %d\n", value);

    // shmid  = shmget(SHM_KEY, 0, 0);
    // if (shmid == -1)
    //     errExit("shmget");
    fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if(fd == -1) {
        perror("shm_open");
        return 1;
    }
    ftruncate(fd, sizeof(struct shmseg));

    // shmp = shmat(shmid, NULL, SHM_RDONLY);
    // if (shmp == (void *) -1)
    //     errExit("shmat");
    shmp = mmap(NULL, sizeof(struct shmseg), PROT_READ, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    /* Transfer blocks of data from shared memory to stdout */

    for (xfrs = 0, bytes = 0; ; xfrs++) {
        // if (reserveSem(semid, READ_SEM) == -1)          /* Wait for our turn */
        //     errExit("reserveSem");
        if (sem_wait(sem) == -1) {
            perror("sem_wait");
            return 1;
        }

        if (shmp->cnt == 0)                     /* Writer encountered EOF */
            break;
        bytes += shmp->cnt;

        if (write(STDOUT_FILENO, shmp->buf, shmp->cnt) != shmp->cnt) {
            perror("write");
            return 1;
        }

        // if (releaseSem(semid, WRITE_SEM) == -1)         /* Give writer a turn */
        //     errExit("releaseSem");
        if(sem_post(sem) == -1) {
            perror("sem_post");
            return 1;
        }
    }

    // if (shmdt(shmp) == -1)
    //     errExit("shmdt");
    if( munmap(shmp, sizeof(struct shmseg)) == -1) {
        perror("munmap");
        return 1;
    }

    /* Give writer one more turn, so it can clean up */

    // if (releaseSem(semid, WRITE_SEM) == -1)
    //     errExit("releaseSem");
    if(sem_post(sem) == -1) {
        perror("sem_post");
        return 1;
    }

    if(sem_close(sem) == -1) {
        perror("sem_close");
        return 1;
    }

    fprintf(stderr, "Received %d bytes (%d xfrs)\n", bytes, xfrs);
    exit(EXIT_SUCCESS);
}
