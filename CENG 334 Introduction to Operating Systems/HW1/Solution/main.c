// Burak Yildiz
// 2449049
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "parser.h" 

// Forward declarations
void execute_command(command cmd);
void execute_pipeline(parsed_input *input);
void exec_command(char **args);
void execute_sequential(parsed_input *input);
void execute_parallel(parsed_input *input);
void execute_subshell(char *subshell_content);
void execute_pipeline2(char ***cmds, int num_cmds);
void execute_repeater(int num_cmds, int pipes[][2]);

// Main function
int main() {
    char line[1024]; 
    parsed_input input;

    while (1) {
        printf("/> "); // Shell prompt
        fflush(stdout); // Ensure the prompt is displayed immediately

        if (!fgets(line, sizeof(line), stdin)) {
            printf("\n");
            break; // Handle EOF (Ctrl+D)
        }

        line[strcspn(line, "\n")] = 0; // Remove the trailing newline

        if (strcmp(line, "quit") == 0) {
            break; // Exit the shell
        }

        if (parse_line(line, &input)) {
            // Handle single commands, pipelines, and subshells directly
            if (input.num_inputs == 1) {
                if (input.inputs[0].type == INPUT_TYPE_COMMAND) {
                    execute_command(input.inputs[0].data.cmd);
                } else if (input.inputs[0].type == INPUT_TYPE_SUBSHELL) {
                    execute_subshell(input.inputs[0].data.subshell);
                }
            } else if (input.num_inputs > 1 && input.separator == SEPARATOR_PIPE) {
                // Handle pipeline
                execute_pipeline(&input);
            } else if (input.separator == SEPARATOR_SEQ) {
                // Handle sequential execution
                execute_sequential(&input);
            } else if (input.separator == SEPARATOR_PARA) {
                // Handle parallel execution
                execute_parallel(&input);
            }
            free_parsed_input(&input);
        } 
    }

    return 0;
}

// Executes a single command
void execute_command(command cmd) {
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) { // Child process
        exec_command(cmd.args);
    } else if (pid < 0) { // Error forking
        perror("eshell");
    } else { // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

// Function to execute a single command within a pipeline segment

void exec_command(char **args) {
    if (execvp(args[0], args) == -1) {
        perror("eshell");
        exit(EXIT_FAILURE);
    }
}
    
// Commands or subshells can be in a pipeline. They will be called CS in short for this section. The steps
// necessary to implement a pipeline are given below:
// 1. Assuming there are N number of CS in a pipeline, create N − 1 pipes (see pipe)
// 2. Fork the CS processes (see fork)
// 3. Redirect their output and input to the correct pipes (see dup2)
// 4. Execute the CS (for commands: see exec family of functions/ see 3.1.4 for subshells)
// 5. Reap all of the CS that have finished executing(see wait family of functions) to prevent zombie
// processes. This also assumes subshell does the same for all its children (see Subshell Execution).
// A basic pseudocode example is given below:
// A | B | C
// for (comm in A,B,C) {
// pipe_i = pipe()
// fork
// dup(pipe_i-1, 0) (if i>0)
// dup(pipe_i, 1) (if i<n)
// exec(comm)
// }
// for (process in A,B,C)
// wait (after forks complete)

void execute_pipeline(parsed_input *input) {

    int pipe_fds[2], in_fd = 0;
    for (int i = 0; i < input->num_inputs; i++) {

        // Last command does not need to create a new pipe for output redirection
        if (i < input->num_inputs - 1) {
            if (pipe(pipe_fds) < 0) {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }

        pid_t pid = fork();
        if (pid == 0) { // Child process
            dup2(in_fd, 0); // Redirect input from the previous pipe or stdin
            if (in_fd != 0) {
                close(in_fd);
            }

            if (i < input->num_inputs - 1) {
                dup2(pipe_fds[1], 1); // Redirect output to the next command
                close(pipe_fds[1]); // No longer needed after dup2
            }

            // Close the read end of the current pipe, if not the last command
            if (i < input->num_inputs - 1) {
                close(pipe_fds[0]);
            }

            // Check if the current command is a subshell command
            if (input->inputs[i].type == INPUT_TYPE_SUBSHELL) {
                execute_subshell(input->inputs[i].data.subshell);
            } else { // Regular command
                exec_command(input->inputs[i].data.cmd.args);
            }
            exit(EXIT_FAILURE); // Should not reach here if exec_command succeeds
        } else if (pid < 0) {
            perror("fork failed");
            exit(EXIT_FAILURE);
        } else { // Parent process
            if (in_fd != 0) {
                close(in_fd);
            }
            if (i < input->num_inputs - 1) {
                close(pipe_fds[1]); // Close the write end of the pipe
                in_fd = pipe_fds[0]; // Next command reads from this pipe
            }
            wait(NULL); // Optionally, wait for the child process to finish
        }
    }

    // Final cleanup, if necessary
    if (in_fd != 0) {
        close(in_fd);
    }
}

// Pipelines or commands can be executed in sequential order. They will be called PC in short for this
// section. The steps necessary to implement a sequential execution are given below:
// 1. For each PC in the list:
// (a) If it is a pipeline, execute the pipeline (see Pipeline Execution)
// (b) Else: Fork a process (see fork)
// (c) Execute the command (for commands: see exec family of functions)
// (d) Reap the finished process (see wait family of functions).
// A basic pseudocode example is given below:
// A ; B ; C
// for (comm in A,B,C) {
// fork (child)
// exec(comm)
// wait() (Parent) (synchronously, wait inside the loop)
// }

// Function to handle sequential execution
void execute_sequential(parsed_input *input) {
    for (int i = 0; i < input->num_inputs; ++i) {
        if (input->inputs[i].type == INPUT_TYPE_COMMAND) {
            execute_command(input->inputs[i].data.cmd);
        } else if (input->inputs[i].type == INPUT_TYPE_PIPELINE) {
            // This case handles a pipeline within a sequence
            char ***cmds = malloc(input->inputs[i].data.pline.num_commands * sizeof(char **));
            for (int j = 0; j < input->inputs[i].data.pline.num_commands; ++j) {
                cmds[j] = input->inputs[i].data.pline.commands[j].args;
            }
            execute_pipeline2(cmds, input->inputs[i].data.pline.num_commands);
            free(cmds);
        }
    }
}

// 1. For each PC in the list:
// (a) If it is a pipeline, execute the pipeline (see Pipeline Execution). Do not wait for the pipeline
// to finish.
// (b) Else: Fork a process (see fork)
// (c) Execute the command (for commands: see exec family of functions)
// 2. Reap all of the finished processes (see wait family of functions). This also includes the pipelines.
// A basic pseudocode example is given below:
// A , B , C
// for (comm in A,B,C) {
// fork (child)
// exec(comm)}
// wait() (Parent) (All of the children, wait outside of the loop)

void execute_parallel(parsed_input *input) {
    int num_cmds = input->num_inputs;
    int pipes[num_cmds][2];
    pid_t pids[num_cmds + 1]; // Plus one for the repeater

    // Create pipes for each command
    for (int i = 0; i < num_cmds; i++) {
        pipe(pipes[i]);
    }

    // Fork repeater process if needed (for stdin replication)
    pids[num_cmds] = fork();
    if (pids[num_cmds] == 0) {
        execute_repeater(num_cmds, pipes); 
        exit(EXIT_SUCCESS);
    }

    // Fork and execute each command
    for (int i = 0; i < num_cmds; i++) {
        pids[i] = fork();
        if (pids[i] == 0) { // Child process
            // Redirect stdin for the command from the repeater
            dup2(pipes[i][0], STDIN_FILENO);
            for (int j = 0; j < num_cmds; j++) { // Close all pipe ends in child
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            // Execute command, subshell, or pipeline
            if (input->inputs[i].type == INPUT_TYPE_COMMAND) {
                exec_command(input->inputs[i].data.cmd.args);
            } else if (input->inputs[i].type == INPUT_TYPE_PIPELINE) {
                // Execute pipeline
                char ***cmds = malloc(input->inputs[i].data.pline.num_commands * sizeof(char **));
                for (int j = 0; j < input->inputs[i].data.pline.num_commands; ++j) {
                    cmds[j] = input->inputs[i].data.pline.commands[j].args;
                }
                execute_pipeline2(cmds, input->inputs[i].data.pline.num_commands);
                free(cmds);
            } else if (input->inputs[i].type == INPUT_TYPE_SUBSHELL) {
                execute_subshell(input->inputs[i].data.subshell);
            }
            exit(EXIT_FAILURE);
        }
    }

    // Close all unused pipe ends in the parent
    for (int i = 0; i < num_cmds; i++) {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    // Wait for all child processes to finish, including the repeater
    for (int i = 0; i < num_cmds + 1; i++) {
        waitpid(pids[i], NULL, 0);
    }
}

// (A ; B; C)
// fork()
// call A;B;C handling code
// wait (in subshell parent)
// (A, B, C)
// fork()
// for (comm in A,B,C)
// pipe_i = pipe()
// fork()
// dup(pipe_i, 0)
// exec(comm)
// fork()
// repeater code,
// for each line of stdin
// replicate to pipe_*[1]
// for (process in A,B,C,repeater):
// wait (after all forks finish, subshell parent)


void execute_subshell(char *subshell_content) {
    //printf("Executing subshell with content: %s\n", subshell_content);
    pid_t pid, wpid;
    int status;

    pid = fork();
    if (pid == 0) {
        // Child process
        parsed_input subshell_input;
        if (parse_line(subshell_content, &subshell_input)) {
            if (subshell_input.num_inputs >= 1) {
                //printf("num imputs 1: \n");
                if (subshell_input.inputs[0].type == INPUT_TYPE_COMMAND && subshell_input.num_inputs == 1) {
                    //printf("imput type command: \n");
                    execute_command(subshell_input.inputs[0].data.cmd);
                } else if (subshell_input.separator == SEPARATOR_PIPE){
                    // Handle pipeline within subshell
                    // printf("pipeline girdi: \n");
                    
                    execute_pipeline(&subshell_input);
                  
                }
                else if (subshell_input.separator == SEPARATOR_SEQ) {
                    execute_sequential(&subshell_input);
            }   else if (subshell_input.separator == SEPARATOR_PARA) {
                      execute_parallel(&subshell_input);
                }
            }
            free_parsed_input(&subshell_input);
        } else {
            printf("Failed to parse subshell command.\n");
        }
        exit(EXIT_SUCCESS);
    } else if (pid < 0) {
        // Error forking
        perror("eshell");
    } else {
        // Parent process
        do {
            wpid = waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
}

// Execute pipeline for sequential and parallell 
void execute_pipeline2(char ***cmds, int num_cmds) {
    //for (int i = 0; i < num_cmds; ++i) {
      //printf("Command %d: %s\n", i, cmds[i][0]); 
    //}
    int pipe_fds[2], in_fd = 0;
    for (int i = 0; i < num_cmds; i++) {
        pipe(pipe_fds);
        if (fork() == 0) {
            dup2(in_fd, 0); // Redirect input
            if (i < num_cmds - 1) {
                dup2(pipe_fds[1], 1); // Redirect output to the next command
            }
            close(pipe_fds[0]);
            exec_command(cmds[i]);
        } else {
            wait(NULL); // Wait for the child process to finish
            close(pipe_fds[1]);
            in_fd = pipe_fds[0];
        }
    }
}

void execute_repeater(int num_cmds, int pipes[][2]) {
    pid_t pid = fork();
    if (pid == 0) { // Repeater process
        char buffer[1024];
        ssize_t nread;
        
        // Close writing ends and stdin, stdout, stderr of the repeater
        for (int i = 0; i < num_cmds; i++) {
            close(pipes[i][0]); // Close reading end, not used by repeater
        }

        while ((nread = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0) {
            for (int i = 0; i < num_cmds; i++) {
                write(pipes[i][1], buffer, nread); // Write to each command's stdin
            }
        }

        // Close pipes after done writing
        for (int i = 0; i < num_cmds; i++) {
            close(pipes[i][1]);
        }

        exit(0);
    } else if (pid < 0) {
        perror("fork failed for repeater");
        exit(EXIT_FAILURE);
    }
    // Parent process might wait for the repeater or continue based on design.
}


// 1. Single Command: 5 pts   ++++++
// /> ls
// 2. Single Pipeline: 10 pts +++++
// /> ls | grep .c
// 3. Sequential Execution with commands: 5 pts +++++
// /> echo sa ; sleep 3 ; sleep 4
// 4. Sequential Execution with mixed commands and pipelines: 15 pts +++++
// /> ls | grep .c ; ls | grep .c | tr a-z A-Z ; cat  needs ctrl d 
// 5. Parallel Execution with commands: 5 pts +++++
// /> echo sa , sleep 3 , sleep 5
// 6. Parallel Execution with mixed commands and pipelines: 15 pts +++++
// /> ls | grep .c , ls | grep .c | tr a-z A-Z , sleep 5
// 7. Subshell with a single pipeline or sequential or parallel execution: 5 pts ++++++
// /> (ls | grep .c)
// /> (echo sa ; sleep 3 ; sleep 4)
// /> (echo sa , sleep 3 , sleep 5)
// 8. Subshell with mixed types(sequential-pipeline or parallel pipeline): 10 pts +++++
// /> ( ls | grep .c ; ls | grep .c | tr a-z A-Z  ; sleep 3 ; ls | grep main )
// /> ( ls | grep .c , ls | grep .c | tr a-z A-Z  , sleep 3 , ls | grep main )
// 9. Subshell pipeline with mixed types(sequential-pipeline or parallel pipeline) without the need    ???
// for repeaters: 15 pts
// /> ( ls | grep .c , ls ) | ( ls | grep .c ) | ( echo sa ; sleep 3 ; echo as | cat )
// 10. Subshell pipeline with mixed types(sequential-pipeline or parallel pipeline) with repeaters    ????
// necessary: 15 pts
// /> ( A | B , C ) | ( D | E , F ) | ( G , H | I ) | J


