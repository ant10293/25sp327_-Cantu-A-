#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>

// Global flags for handling signal reception
static volatile sig_atomic_t received_tstp = 0;     // Flag for SIGTSTP (suspend) signal
static volatile sig_atomic_t received_sigterm = 0;  // Flag for SIGTERM (termination) signal
static volatile sig_atomic_t received_sigusr2 = 0;  // Flag for SIGUSR2
static int sum = 0;                                 // Summation variable to send to parent
static pid_t parent_pid = 0;                        // Parent process ID

// Signal handler for SIGTSTP
void handle_sigtstp(int sig) {
    (void)sig; 
    received_tstp = 1; // Set flag indicating that SIGTSTP was received
}

// Signal handler for SIGUSR2 using siginfo to confirm sender
void handle_sigusr2(int sig, siginfo_t *info, void *ucontext) {
    (void)sig;         
    (void)ucontext;     
    if (info->si_pid == parent_pid) {
        received_sigusr2 = 1; // Set flag indicating reception from parent
    }
}

// Signal handler for SIGTERM
void handle_sigterm(int sig) {
    (void)sig; 
    received_sigterm = 1; // Set flag for termination
}

int main(int argc, char *argv[]) {
    // Validate command line argument for parent PID
    if (argc < 2) {
        fprintf(stderr, "Child: missing parent PID argument.\n");
        exit(EXIT_FAILURE);
    }
    parent_pid = (pid_t)atoi(argv[1]); // Convert argument to integer for PID
    if (parent_pid <= 1) {
        fprintf(stderr, "Child: invalid parent PID.\n");
        exit(EXIT_FAILURE);
    }

    // Setup signal handlers for SIGTSTP, SIGUSR2, and SIGTERM
    struct sigaction sa_tstp, sa_usr2, sa_term;
    memset(&sa_tstp, 0, sizeof(sa_tstp));
    memset(&sa_usr2, 0, sizeof(sa_usr2));
    memset(&sa_term, 0, sizeof(sa_term));

    // Configuration for SIGTSTP
    sa_tstp.sa_flags = SA_RESTART; 
    sa_tstp.sa_handler = handle_sigtstp;
    if (sigaction(SIGTSTP, &sa_tstp, NULL) == -1) {
        perror("sigaction - SIGTSTP");
        exit(EXIT_FAILURE);
    }

    // Configuration for SIGUSR2
    sa_usr2.sa_flags = SA_SIGINFO;
    sa_usr2.sa_sigaction = handle_sigusr2;
    if (sigaction(SIGUSR2, &sa_usr2, NULL) == -1) {
        perror("sigaction - SIGUSR2");
        exit(EXIT_FAILURE);
    }

    // Configuration for SIGTERM
    sa_term.sa_flags = 0;
    sa_term.sa_handler = handle_sigterm;
    if (sigaction(SIGTERM, &sa_term, NULL) == -1) {
        perror("sigaction - SIGTERM");
        exit(EXIT_FAILURE);
    }

    // Child process loop
    printf("Child: Running... Use 'kill -SIGTSTP %d' to interrupt me.\n", getpid());
    int counter = 0; // Counter to control display message frequency

    while (1) {
        if (received_sigterm) {
            union sigval val; // Union to send value with sigqueue
            val.sival_int = sum;
            if (sigqueue(parent_pid, SIGUSR1, val) == -1) {
                perror("sigqueue - final");
            }
            printf("Child: Received SIGTERM, exiting...\n");
            break; // Exit loop on SIGTERM
        }

        // Simulate some ongoing task by incrementing sum
        sum += 10;
        sleep(2); // Delay to simulate process time
        counter++;

        // Periodically print status
        if (counter % 3 == 0) {
            printf("Child: Running... Use 'kill -SIGTSTP %d' to interrupt me.\n", getpid());
        }

        // Check if SIGTSTP was received and send data to parent
        if (received_tstp) {
            received_tstp = 0;
            printf("Child: SIGTSTP received. Sending sum to parent.\n");
            union sigval val;
            val.sival_int = sum;
            if (sigqueue(parent_pid, SIGUSR1, val) == -1) {
                perror("sigqueue");
            }
        }

        // Check for SIGUSR2 and log its reception
        if (received_sigusr2) {
            received_sigusr2 = 0;
            printf("Child: Received SIGUSR2 from Parent (PID: %d). Current sum = %d\n",
                   parent_pid, sum);
        }
    }

    return 0;
}
