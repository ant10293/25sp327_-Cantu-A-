#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/fsuid.h>

// Macro to print RUID, EUID, and SUID
#define PRINT_IDS(stage) do { \
    uid_t r, e, s; \
    if (getresuid(&r, &e, &s) == -1) { \
        perror("getresuid"); \
        _exit(1); \
    } \
    printf("%s: RUID=%u, EUID=%u, SUID=%u\n", stage, r, e, s); \
} while (0)

int main() {
    PRINT_IDS("t0: Running with root privileges");

    // Temporarily drop privileges
    if (seteuid(getuid()) != 0) {
        perror("seteuid");
        _exit(1);
    }
    PRINT_IDS("t1: Dropped privileges temporarily");

    // Restore privileges
    if (seteuid(0) != 0) {
        perror("seteuid");
        _exit(1);
    }
    PRINT_IDS("t2: Restored privileges");

    // Drop privileges again
    if (seteuid(getuid()) != 0) {
        perror("seteuid");
        _exit(1);
    }
    PRINT_IDS("t3: Dropped privileges again");

    // Permanently drop privileges
    if (setresuid(getuid(), getuid(), getuid()) != 0) {
        perror("setresuid");
        _exit(1);
    }
    printf("t4: Permanently dropping privileges...\n");
    PRINT_IDS("t4: After permanent drop");

    // Attempt to restore privileges but expect failure
    if (seteuid(0) != 0) {
        printf("t5: Attempting to restore privileges...\n");
        printf("t5: Failed to restore privileges: Operation not permitted\n");
    } else {
        // This block should never execute since privileges cannot be restored
        PRINT_IDS("t5: Unexpectedly restored privileges");
    }

    return 0;
}
