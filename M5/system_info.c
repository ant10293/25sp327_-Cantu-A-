#include <stdio.h>
#include <string.h>
#include <sys/sysinfo.h>

#define COLUMN_FORMAT "%-29s"

void getHostname() {
    FILE *fp = fopen("/proc/sys/kernel/hostname", "r");
    char hostname[256];
    if (fp) {
        if (fgets(hostname, sizeof(hostname), fp)) {
            hostname[strcspn(hostname, "\n")] = 0; // Remove newline character if present
            printf(COLUMN_FORMAT "%s\n", "Current Host Name:", hostname);
        }
        fclose(fp);
    } else {
        perror("Error opening /proc/sys/kernel/hostname");
    }
}

void getKernelVersion() {
    FILE *fp = fopen("/proc/version", "r");
    char version[256];
    if (fp) {
        if (fgets(version, sizeof(version), fp)) {
            char *parenthesis = strchr(version, '(');
            if (parenthesis) {
                *parenthesis = '\0'; // Truncate string at the first '('
            }
            version[strcspn(version, "\n")] = 0; // Remove any trailing newline characters
            printf(COLUMN_FORMAT "%s\n", "Kernel Version:", version);
        }
        fclose(fp);
    } else {
        perror("Error opening /proc/version");
    }
}

void getCpuInfo() {
    FILE *fp = fopen("/proc/cpuinfo", "r");
    if (!fp) {
        perror("Failed to open /proc/cpuinfo");
        return;
    }

    char line[512];
    char cpuType[256] = "Unavailable";
    char cpuModel[256] = "Unavailable";
    int cores = 0;
    int typeFound = 0;
    int modelFound = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (!typeFound && strstr(line, "model name")) {
            char *start = strchr(line, ':');
            if (start) {
                strncpy(cpuType, start + 2, sizeof(cpuType) - 1);
                cpuType[strlen(cpuType) - 1] = '\0'; // Remove newline
                typeFound = 1;
            }
        }
        if (!modelFound && (strstr(line, "model\t:") || strstr(line, "model :"))) {
            char *start = strchr(line, ':');
            if (start) {
                strncpy(cpuModel, start + 2, sizeof(cpuModel) - 1);
                cpuModel[strlen(cpuModel) - 1] = '\0'; // Remove newline
                modelFound = 1;
            }
        }
        if (strncmp(line, "cpu cores", 9) == 0) {
            sscanf(line, "cpu cores : %d", &cores);
        }
    }

    fclose(fp);
    printf(COLUMN_FORMAT "%s\n", "CPU Type:", cpuType);
    printf(COLUMN_FORMAT "%s\n", "CPU Model:", cpuModel);
    if (cores == 0) {
        printf(COLUMN_FORMAT "%s\n", "CPU Cores:", "Unavailable");
    }
    else {
        printf(COLUMN_FORMAT "%d\n", "CPU Cores:", cores);
    }
}

void getMemoryInfo() {
    FILE *fp = fopen("/proc/meminfo", "r");
    char line[256];
    long totalMem = 0, freeMem = 0;
    if (fp) {
        while (fgets(line, sizeof(line), fp)) {
            if (strncmp(line, "MemTotal", 8) == 0) {
                sscanf(line, "MemTotal: %ld kB", &totalMem);
            }
            if (strncmp(line, "MemAvailable", 12) == 0) {
                sscanf(line, "MemAvailable: %ld kB", &freeMem);
            }
        }
        printf(COLUMN_FORMAT "%ld kB\n", "Memory Configured:", totalMem);
        printf(COLUMN_FORMAT "%ld kB\n", "Memory Available:", freeMem);
        fclose(fp);
    } else {
        perror("Error opening /proc/meminfo");
    }
}

void getUptime() {
    struct sysinfo info;
    if (sysinfo(&info) == 0) {
        long days = info.uptime / 86400;
        long hours = (info.uptime % 86400) / 3600;
        long minutes = (info.uptime % 3600) / 60;
        long seconds = info.uptime % 60;
        printf(COLUMN_FORMAT "%ld:%02ld:%02ld:%02ld\n", "Uptime:", days, hours, minutes, seconds);
    } else {
        perror("Error getting system uptime");
    }
}

void getCpuTimes() {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp) {
        perror("Failed to open /proc/stat");
        return;
    }

    char line[1024];
    long user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
    
    if (fgets(line, sizeof(line), fp)) {
        sscanf(line, "cpu  %ld %ld %ld %ld %ld %ld %ld %ld %ld %ld",
               &user, &nice, &system, &idle, &iowait, &irq, &softirq, &steal, &guest, &guest_nice);
        printf(COLUMN_FORMAT "%ld ms\n", "Time Spent in User Mode:", (user + nice) * 10);
        printf(COLUMN_FORMAT "%ld ms\n", "Time Spent in System Mode:", system * 10);
        printf(COLUMN_FORMAT "%ld ms\n", "Time Spent in Idle Mode:", idle * 10);
    }

    fclose(fp);
}

int main() {
    printf("----------------------------------------------------------------------------\n");
    printf("                             System Information\n");
    printf("----------------------------------------------------------------------------\n");
    getHostname();
    getCpuInfo();
    getKernelVersion();
    getMemoryInfo();
    getUptime();
    getCpuTimes();
    printf("----------------------------------------------------------------------------\n");
    return 0;
}
