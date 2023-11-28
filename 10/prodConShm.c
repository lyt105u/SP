// producer, consumer, shared memory
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/types.h>

// Global variable for signal handling
volatile sig_atomic_t data_ready = 0;
volatile int received_parameter = 0;

// Signal handler function
void signal_handler(int sig, siginfo_t *siginfo, void *context) {
    data_ready = 1;
    received_parameter = siginfo->si_value.sival_int; // Retrieve the parameter
}

int main(int argc, char **argv) {
    if(argc != 5) {
        printf("Usage: %s <data_count> <transmission_frequency> <consumer_count> <buffer_size>\n", argv[0]);
        return 1;
    } else if(atoi(argv[1])==0 || atoi(argv[2])==0 || atoi(argv[3])==0 || atoi(argv[4])==0){
        printf("Error: all the arguments should be integers\n");
        return 1;
    }

    int M = atoi(argv[1]);  // data cnt
    int R = atoi(argv[2]);  // transmission frequency
    int N = atoi(argv[3]);  // consumer cnt
    int B = atoi(argv[4]);  // buffer size

    typedef struct {
        int rows;
        char data[B][80];
    } SharedMemory;

    size_t shm_size = sizeof(int) + B * 80; // size for rows + character data
    const char *shm_name = "/my_shared_memory";
    int shm_fd;
    SharedMemory *shared_mem;
    pid_t pids[N]; // Array to store child process IDs

    // create shared memory
    shm_fd = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        return 1;
    }

    // configure shared memory size
    ftruncate(shm_fd, shm_size);

    // map shared memory
    shared_mem = (SharedMemory *)mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shared_mem == MAP_FAILED) {
        perror("mmap");
        close(shm_fd);
        return 1;
    }
    shared_mem->rows = 0;

    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = signal_handler;
    sa.sa_flags = SA_SIGINFO;

    // Set the signal handler for SIGUSR1
    if(sigaction(SIGUSR1, &sa, NULL) < 0) {
        perror("sigaction");
        return 1;
    }

    for(int i=0; i<N; i++) {
        pids[i] = fork();
        if(pids[i] < 0) {
            perror("fork");
            return 1;
        } else if (pids[i] == 0) {
            int recv_cnt = 0;
            while(1) {
                while(!data_ready) {
                    // Waiting for signal from parent
                    pause();
                }
                data_ready = 0;
                int number;
                sscanf(shared_mem->data[received_parameter % B], "This is message %d", &number);
                if (number == received_parameter) {
                    // printf("%s\n", shared_mem->data[0]);
                    printf("%d receives: %s\n", getpid(), shared_mem->data[received_parameter % B]);
                    recv_cnt++;
                } else if (received_parameter == -1) {
                    printf("%d got %d message in total\n", getpid(), recv_cnt);
                    __sync_add_and_fetch(&shared_mem->rows, recv_cnt);
                    return 0;
                }
            }   
        }
    }

    // Send signal to each child process with a parameter
    union sigval value;
    int send_cnt = 0;
    while(send_cnt < M) {
        for (int i = 0; i < N; i++) {
            value.sival_int = send_cnt; // Different parameter for each child
            sprintf(shared_mem->data[send_cnt % B], "This is message %d", send_cnt);
            // strcpy(shared_mem->data[send_cnt % B], "This is message 0");
            sigqueue(pids[i], SIGUSR1, value);
        }
        send_cnt++;
        usleep(R);
    }

    sleep(5);
    for (int i = 0; i < N; i++) {
        value.sival_int = -1; // Different parameter for each child
        sigqueue(pids[i], SIGUSR1, value);
    }

    // Wait for all child processes to finish
    for (int i = 0; i < N; i++) {
        waitpid(pids[i], NULL, 0);
    }

    printf("\nM = %d, R = %d μs, N = %d, B = %d\n", M, R, N, B);
    printf("----------------------------------------------------------------\n");
    printf("Total messages: %d\n", M *N);
    printf("Sum of received messages by all consumers: %d\n", shared_mem->rows);
    float result = 1 - (float)shared_mem->rows / (M*N);
    printf("Loss rate: %f\n", result);

    // clean up
    munmap(shared_mem, shm_size);
    close(shm_fd);
    shm_unlink(shm_name);

    return 0;
}
