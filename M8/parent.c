#define _XOPEN_SOURCE 700
#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>

// Global variables to handle signal behavior and status
static volatile sig_atomic_t received_sigusr1 = 0; // Flag for SIGUSR1 reception
static volatile sig_atomic_t child_sum = 0;        // Data from child via SIGUSR1
static volatile sig_atomic_t child_terminated = 0; // Flag for child termination
static pid_t child_pid = 0;                        // PID of the child process

// Signal handler for SIGUSR1
void sigusr1_handler(int signo, siginfo_t *info, void *ucontext) {
    (void)signo;     
    (void)ucontext; 
    // Check if the signal comes from the expected child
    if (info->si_pid == child_pid) {
        child_sum = info->si_value.sival_int; // Retrieve value sent from child
        received_sigusr1 = 1; // Set flag to indicate signal reception
    }
}

// Signal handler for SIGCHLD
void sigchld_handler(int signo, siginfo_t *info, void *ucontext) {
    (void)signo;
    (void)ucontext;
    // Check if the termination signal comes from the expected child
    if (info->si_pid == child_pid) {
        child_terminated = 1; // Set flag to indicate child has terminated
    }
}

int main(void) {
    struct sigaction sa_usr1, sa_chld;
    memset(&sa_usr1, 0, sizeof(sa_usr1)); // Zero out the structure
    memset(&sa_chld, 0, sizeof(sa_chld)); // Zero out the structure

    // Configure the SIGUSR1 signal handler
    sa_usr1.sa_flags = SA_SIGINFO;
    sa_usr1.sa_sigaction = sigusr1_handler;
    if (sigaction(SIGUSR1, &sa_usr1, NULL) == -1) {
        perror("sigaction - SIGUSR1");
        exit(EXIT_FAILURE);
    }

    // Configure the SIGCHLD signal handler
    sa_chld.sa_flags = SA_SIGINFO | SA_NOCLDSTOP;
    sa_chld.sa_sigaction = sigchld_handler;
    if (sigaction(SIGCHLD, &sa_chld, NULL) == -1) {
        perror("sigaction - SIGCHLD");
        exit(EXIT_FAILURE);
    }

    // Fork to create a child process
    child_pid = fork();
    if (child_pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    // Child process execution
    if (child_pid == 0) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%d", getppid()); // Pass parent PID to child
        execl("./child", "./child", buf, (char *)NULL);
        perror("execl");
        exit(EXIT_FAILURE);
    }

    // Parent process execution
    printf("Parent: fork-exec successful. Child pid (%d)\n", child_pid);

    // Main loop to handle child process and signals
    while (!child_terminated) {
        sleep(3); // Sleep to simulate work
        if (child_terminated) {
            break; // Exit if child has terminated
        }
        printf("Parent: Working...\n");

        // Handle reception of SIGUSR1
        if (received_sigusr1) {
            received_sigusr1 = 0;
            printf("Parent: Received SIGUSR1 from Child (PID: %d). Sum = %d\n",
                   child_pid, child_sum);

            // Send SIGUSR2 to child to acknowledge
            if (kill(child_pid, SIGUSR2) == -1) {
                perror("kill - SIGUSR2");
            }
        }
    }

    // Wait for child process to terminate and handle its exit status
    if (child_terminated) {
        int status;
        pid_t w = waitpid(child_pid, &status, 0);
        if (w < 0) {
            perror("waitpid");
            exit(EXIT_FAILURE);
        }
        printf("Parent: Child (PID: %d) has terminated with status %d. Exiting.\n",
               child_pid, WEXITSTATUS(status));
    }

    return EXIT_SUCCESS;
}
