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
    // semid = semget(SEM_KEY, 2, IPC_CREAT | OBJ_PERMS);
    // if (semid == -1)
    //     errExit("semget");

    // if (initSemAvailable(semid, WRITE_SEM) == -1)
    //     errExit("initSemAvailable");
    // if (initSemInUse(semid, READ_SEM) == -1)
    //     errExit("initSemInUse");

    // sem = sem_open(SEM_NAME, O_CREAT, 0644, 1);
    // if(sem == SEM_FAILED) {
    //     perror("sem_open");
    //     return 1;
    // }

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

    int value;
    sem_getvalue(sem, &value);
    printf("Semaphore value: %d\n", value);

    // create shared memory
    // shmid = shmget(SHM_KEY, sizeof(struct shmseg), IPC_CREAT | OBJ_PERMS);
    // if (shmid == -1)
    //     errExit("shmget");
    fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if(fd == -1) {
        perror("shm_open");
        return 1;
    }

    // shmaat = shared memory attach
    // shmp = shared memory pointer
    // shmp = shmat(shmid, NULL, 0);
    // if (shmp == (void *) -1)
    //     errExit("shmat");

    // map shared memory object
    shmp = mmap(NULL, sizeof(struct shmseg), PROT_READ, MAP_SHARED, fd, 0);
    if (shmp == MAP_FAILED) {
        perror("mmap");
        return 1;
    }

    /* Transfer blocks of data from stdin to shared memory */

    // xfrs = transfers (the number of times the transfer happened?)
    for (xfrs = 0, bytes = 0; ; xfrs++, bytes += shmp->cnt) {
        /* Wait for our turn */
        // if (reserveSem(semid, WRITE_SEM) == -1)
        //     errExit("reserveSem");
        if (sem_wait(sem) == -1) {
            perror("sem_wait");
            return 1;
        }
        printf("writer pass sem\n");

        // shmp->cnt = read(STDIN_FILENO, shmp->buf, 1024);
        // if (shmp->cnt == -1) {
        //     perror("read");
        //     return 1;
        // }
        if(fgets(shmp->buf, sizeof(shmp->buf), stdin) == NULL) {
            perror("fgets");
            return 1;
        }
        printf("%s\n", shmp->buf);
        shmp->cnt = strlen(shmp->buf);

        // if (releaseSem(semid, READ_SEM) == -1)          /* Give reader a turn */
        //     errExit("releaseSem");
        if(sem_post(sem) == -1) {
            perror("sem_post");
            return 1;
        }

        /* Have we reached EOF? We test this after giving the reader
           a turn so that it can see the 0 value in shmp->cnt. */

        if (strcmp(shmp->buf, "quit") == 0)
            break;
    }

    /* Wait until reader has let us have one more turn. We then know
       reader has finished, and so we can delete the IPC objects. */

    // if (reserveSem(semid, WRITE_SEM) == -1)
    //     errExit("reserveSem");
    if (sem_wait(sem) == -1) {
        perror("sem_wait");
        return 1;
    }

    // if (semctl(semid, 0, IPC_RMID, dummy) == -1)
    //     errExit("semctl");
    if(sem_close(sem) == -1) {
        perror("sem_close");
        return 1;
    }
    if(sem_unlink(SEM_NAME) == -1) {
        perror("sem_unlink");
        return 1;
    }
    // if (shmdt(shmp) == -1)
    //     errExit("shmdt");
    if( munmap(shmp, sizeof(struct shmseg)) == -1) {
        perror("munmap");
        return 1;
    }
    // if (shmctl(shmid, IPC_RMID, 0) == -1)
    //     errExit("shmctl");
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
