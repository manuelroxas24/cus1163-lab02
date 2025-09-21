#include "proc_reader.h"

int list_process_directories(void) 
{
    DIR *dir = opendir("/proc");
    if (!dir)
    {
        perror("opendir");
        return -1;
    }

    struct dirent *entry;
    int count = 0;

    

    printf("Process directories in /proc:\n");
    printf("%-8s %-20s\n", "PID", "Type");
    printf("%-8s %-20s\n", "---", "----");

    while ((entry = readdir(dir)) != NULL) 
    {
        if (is_number(entry->d_name))
        {
            printf("%-8s %-20s\n", entry->d_name, "Process");
            count++;
        }
    }
   
    if (closedir(dir) == -1) 
    {
        perror("closedir");
        return -1;
    }

    printf("\nTotal processes found: %d\n", count);
    return 0;

}

int read_process_info(const char* pid) 
{
    char filepath[256];

    printf("\n--- Process Information for PID %s ---\n", pid);

    snprintf(filepath, sizeof(filepath), "/proc/%s/status", pid);
    if (read_file_with_syscalls(filepath) != 0)
    {
        fprintf(stderr, "Failed to read %s\n", filepath);
        return -1;
    }

    printf("\n--- Command Line ---\n");

    snprintf(filepath, sizeof(filepath), "/proc/%s/cmdline", pid);
    if (read_file_with_syscalls(filepath) != 0)
    {
        fprintf(stderr, "Failed to read %s\n", filepath);
        return -1;
    }

    printf("\n");
    return 0; 
}

int show_system_info(void) 
{
    FILE *fp;
    char buffer[256];
    int line_count = 0;
    const int MAX_LINES = 10;

    printf("\n--- CPU Information (first %d lines) ---\n", MAX_LINES);

    fp = fopen("/proc/cpuinfo", "r");
    if (!fp)
    {
        perror("fopen");
        return -1;
    }
    while (fgets(buffer, sizeof(buffer), fp) && line_count < MAX_LINES)
    {
        printf("%s", buffer);
        line_count++;
    }
    fclose(fp);

    printf("\n--- Memory Information (first %d lines) ---\n", MAX_LINES);
    line_count = 0;
    fp = fopen("/proc/meminfo", "r");
    if (!fp)
    {
        perror("fopen");
        return -1;
    }
    while (fgets(buffer, sizeof(buffer), fp) && line_count < MAX_LINES)
    {
        printf("%s", buffer);
        line_count++;
    }
    fclose(fp);
    return 0;
}

void compare_file_methods(void)
{
    const char* test_file = "/proc/version";

    printf("Comparing file reading methods for: %s\n\n", test_file);

    printf("=== Method 1: Using System Calls ===\n");
    read_file_with_syscalls(test_file);

    printf("\n=== Method 2: Using Library Functions ===\n");
    read_file_with_library(test_file);

    printf("\nNOTE: Run this program with strace to see the difference!\n");
    printf("Example: strace -e trace=openat,read,write,close ./lab2\n");
}

int read_file_with_syscalls(const char* filename) 
{
    
    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        return -1;
    }

    char buffer[256];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer) -1)) > 0)
    {
        buffer[bytes_read] = '\0';
        printf("%s", buffer);
    }

    if (bytes_read == -1) 
    {
        perror("read");
        close(fd);
        return -1;
    }

    if (close(fd) == -1)
    {
        perror("close");
        return -1;
    }
    
    return 0; 
}

int read_file_with_library(const char* filename) 
{
    FILE *fp = fopen(filename, "r");
    if (!fp)
    {
        perror("fopen");
        return -1;
    }

    char buffer[256];
    while (fgets(buffer, sizeof(buffer), fp))
    {
        printf("%s", buffer);
    
    }

    if (fclose(fp) == EOF)
    {
        perror("fclose");
        return -1;
    }
    return 0; 
}

int is_number(const char* str)
 {
    if (!str || *str == '\0')
    return 0; 
    
    while (*str)
    {
        if (!isdigit((unsigned char)*str))
        return 0;
        str++;
    }
    return 1;
}