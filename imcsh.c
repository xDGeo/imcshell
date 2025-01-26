#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#define MAX_INPUT 1024
#define MAX_ARGS 100

pid_t background_processes[MAX_ARGS]; // Store background process PIDs
int bg_count = 0; // Count of background processes

/**
 * Function to execute a given command.
 * Supports:
 * - Foreground execution (`exec <command>`)
 * - Background execution (`exec <command> &`)
 * - Output redirection (`exec <command> > <file>`)
 */
void execute_command(char *input) {
    char *args[MAX_ARGS]; 
    char *token = strtok(input, " ");
    int background = 0, redirect = 0;
    char *outfile = NULL;
    int i = 0;

    // Ignore empty input
    if (!token) return;

    // Check for "exec" and skip it
    if (strcmp(token, "exec") == 0) {
        token = strtok(NULL, " "); // Skip "exec"
    }

    // Tokenize input
    while (token != NULL) {
        if (strcmp(token, "&") == 0) {
            background = 1;  // Set background flag
        } else if (strcmp(token, ">") == 0) {
            redirect = 1; 
            token = strtok(NULL, " "); // Get filename
            if (token) {
                outfile = token;
            } else {
                fprintf(stderr, "Error: No file specified for redirection\n");
                return;
            }
        } else {
            args[i++] = token;
        }
        token = strtok(NULL, " ");
    }
    args[i] = NULL; // Null-terminate the argument list

    if (args[0] == NULL) return; // If no command, return

    // Built-in command: "globalusage"
    if (strcmp(args[0], "globalusage") == 0) {
    int fd = -1, saved_stdout = -1;

    if (redirect && outfile) {
        fd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd < 0) {
            perror("open");
            return;
        }
        
        // Save the original stdout file descriptor
        saved_stdout = dup(STDOUT_FILENO);
        
        // Redirect stdout to file
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    printf("IMCSH Version 1.1 created by George and Mihai\n");

    if (redirect && saved_stdout != -1) {
        // Restore the original stdout
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
    }

    fflush(stdout);  // Ensure output is written
    return;
}

    // Create a child process to execute the command
    pid_t pid = fork();

    if (pid < 0) { // Fork error
        perror("fork");
        return;
    } 
    
    if (pid == 0) { // Child process
        if (redirect && outfile) {
            int fd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd < 0) {
                perror("open");
                exit(EXIT_FAILURE);
            }
            dup2(fd, STDOUT_FILENO); // Redirect stdout to file
            close(fd);
        }

        execvp(args[0], args); // Execute the command
        perror("execvp"); // Only runs if exec fails
        exit(EXIT_FAILURE);
    } else { // Parent process
        if (background) {
            printf("Started background process with PID %d\n", pid);
            background_processes[bg_count++] = pid;
        } else {
            waitpid(pid, NULL, 0); // Wait for foreground process
            printf("Process %d finished.\n", pid);
        }

        if (redirect) {
            printf("Process %d finished. Output saved to %s\n", pid, outfile);
        }
    }
}

/**
 * Function to handle quitting the shell.
 * - If background processes exist, prompts the user before quitting.
 * - Terminates all background processes if the user confirms.
 */
void quit_shell(void) {
    if (bg_count > 0) {
        printf("The following background processes are still running:\n");
        for (int i = 0; i < bg_count; i++) {
            printf("- PID %d\n", background_processes[i]);
        }
        printf("Are you sure you want to quit? [Y/n]: ");
        char response;
        scanf(" %c", &response);
        getchar(); // Consume newline

        if (response == 'Y' || response == 'y') {
            printf("Terminating background processes...\n");
            for (int i = 0; i < bg_count; i++) {
                kill(background_processes[i], SIGKILL); // Kill background processes
            }
            exit(0);
        }
    } else {
        printf("Exiting IMCSH...\n");
        exit(0);
    }
}

int main(void) {
    char input[MAX_INPUT];

    while (1) {
        printf("user@host> ");
        if (fgets(input, MAX_INPUT, stdin) == NULL) break;
        input[strcspn(input, "\n")] = '\0';  // Remove newline

        if (strcmp(input, "quit") == 0) {
            quit_shell();
        } else {
            execute_command(input);
        }

        // ✅ Detect if any background processes have finished
        for (int i = 0; i < bg_count; i++) {
            if (waitpid(background_processes[i], NULL, WNOHANG) > 0) {
                printf("Background process %d finished.\n", background_processes[i]);
                // Remove the finished process from the list
                for (int j = i; j < bg_count - 1; j++) {
                    background_processes[j] = background_processes[j + 1];
                }
                bg_count--; // Reduce count
                i--; // Adjust loop index
            }
        }
    }
    return 0;
}
