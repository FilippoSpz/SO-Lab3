#include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <unistd.h>
    #include <sys/wait.h>

    #define MAXPROCESSES 256
    #define MAXCMD 256

    void close_pipes(int pipes[MAXPROCESSES][2], int n, int close_write) {
        for (int i = 0; i < n; i++) {
            close(pipes[i][close_write]);
        }
    }

    void replace_placeholder(char *dest, const char *template, const char *param) {
        const char *placeholder = strstr(template, "%");
        if (placeholder) {
            size_t prefix_len = placeholder - template;
            strncpy(dest, template, prefix_len);
            dest[prefix_len] = '\0';
            strcat(dest, param);
            strcat(dest, placeholder + 1);
        } else {
            strcpy(dest, template);
        }
    }

    int main(int argc, char *argv[]) {
        if (argc != 4) {
            fprintf(stderr, "Usage: %s <file> <num_processes> <command>\n", argv[0]);
            return 1;
        }

        char *file_name = argv[1];
        int num_processes = atoi(argv[2]);
        char *command_template = argv[3];

        if (num_processes <= 0 || num_processes > MAXPROCESSES) {
            fprintf(stderr, "Invalid number of processes. Must be between 1 and %d.\n", MAXPROCESSES);
            return 1;
        }

        FILE *file = fopen(file_name, "r");
        if (!file) {
            perror("Error opening file");
            return 1;
        }

        int pipes[MAXPROCESSES][2];
        for (int i = 0; i < num_processes; i++) {
            if (pipe(pipes[i]) == -1) {
                perror("Error creating pipe");
                return 1;
            }
        }

        for (int i = 0; i < num_processes; i++) {
            pid_t pid = fork();
            if (pid == -1) {
                perror("Error during fork");
                return 1;
            }

            if (pid == 0) { // Child process
                close_pipes(pipes, num_processes, 1); // Close write ends
                char cmd[MAXCMD];
                while (read(pipes[i][0], cmd, MAXCMD) > 0) {
                    system(cmd);
                }
                close(pipes[i][0]);
                exit(0);
            }
        }

        close_pipes(pipes, num_processes, 0); // Close read ends in parent

        char line[MAXCMD];
        int current_process = 0;
        while (fgets(line, sizeof(line), file)) {
            line[strcspn(line, "\n")] = '\0'; // Remove newline
            char cmd[MAXCMD];
            replace_placeholder(cmd, command_template, line);
            write(pipes[current_process][1], cmd, strlen(cmd) + 1); // Include null terminator
            current_process = (current_process + 1) % num_processes;
        }

        fclose(file);

        for (int i = 0; i < num_processes; i++) {
            close(pipes[i][1]); // Close write ends
        }

        for (int i = 0; i < num_processes; i++) {
            wait(NULL); // Wait for all children to finish
        }

        return 0;
    }