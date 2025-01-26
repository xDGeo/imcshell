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

pid_t background_processes[MAX_ARGS]; 
int bg_count = 0; 

/**
 * Executes a command entered by the user.
 * Supports foreground execution, background execution (&),
 * and output redirection (>). Handles built-in command `globalusage`.
 */
void execute_command(char *input) {
    char *args[MAX_ARGS]; 
    char *token = strtok(input, " ");
    int background = 0, redirect = 0;
    char *outfile = NULL;
    int i = 0;

    if (!token) return;

    if (strcmp(token, "exec") == 0) {
        token = strtok(NULL, " ");
    }

    while (token != NULL) {
        if (strcmp(token, "&") == 0) {
            background = 1;
        } else if (strcmp(token, ">") == 0) {
            redirect = 1; 
            token = strtok(NULL, " "); 
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
    args[i] = NULL;

    if (args[0] == NULL) return; 

    // Handle built-in globalusage command
    if (strcmp(args[0], "globalusage") == 0) {
        int fd = -1, saved_stdout = -1;

        if (redirect && outfile) {
            fd = open(outfile, O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd < 0) {
                perror("open");
                return;
            }
            saved_stdout = dup(STDOUT_FILENO);
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        printf("IMCSH Version 1.1 created by George and Mihai\n");

        if (redirect && saved_stdout != -1) {
            dup2(saved_stdout, STDOUT_FILENO);
            close(saved_stdout);
        }

        fflush(stdout); 
        return;
    }

    pid_t pid = fork();

    if (pid < 0) { 
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
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        execvp(args[0], args);
        perror("execvp");
        exit(EXIT_FAILURE);
    } else { // Parent process
        if (background) {
            printf("Started background process with PID %d\n", pid);
            background_processes[bg_count++] = pid;
        } else {
            waitpid(pid, NULL, 0);
            printf("Process %d finished.\n", pid);
        }

        if (redirect) {
            printf("Process %d finished. Output saved to %s\n", pid, outfile);
        }
    }
}

/**
 * Handles quitting the shell.
 * If there are running background processes, prompts the user for confirmation.
 * Terminates all background processes before exiting if confirmed.
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
        getchar();

        if (response == 'Y' || response == 'y') {
            printf("Terminating background processes...\n");
            for (int i = 0; i < bg_count; i++) {
                kill(background_processes[i], SIGKILL); 
            }
            exit(0);
        }
    } else {
        printf("Exiting IMCSH...\n");
        exit(0);
    }
}

/**
 * Entry point for the IMCSH shell.
 *
 * Continuously reads user input and executes commands.
 * Detects and handles background process termination.
 *
 * Exit status (0 for normal termination).
 */
int main(void) {
    char input[MAX_INPUT];

    while (1) {
        printf("user@host> ");
        if (fgets(input, MAX_INPUT, stdin) == NULL) break;
        input[strcspn(input, "\n")] = '\0'; 

        if (strcmp(input, "quit") == 0) {
            quit_shell();
        } else {
            execute_command(input);
        }

        for (int i = 0; i < bg_count; i++) {
            if (waitpid(background_processes[i], NULL, WNOHANG) > 0) {
                printf("Background process %d finished.\n", background_processes[i]);
                for (int j = i; j < bg_count - 1; j++) {
                    background_processes[j] = background_processes[j + 1];
                }
                bg_count--;
                i--;
            }
        }
    }
    return 0;
}