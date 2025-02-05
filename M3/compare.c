#include <stdlib.h>
#include <stdio.h>

int main() {
    void* ptr = malloc(1024); // Allocate 1024 bytes
    if (ptr == NULL) {
        printf("Memory allocation failed\n");
        return 1; // Non-zero return value indicates error
    }

    printf("Memory allocation succeeded\n");
    free(ptr); // Free the allocated memory
    return 0; // Successful execution
}
