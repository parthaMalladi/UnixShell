#include <bits/stdc++.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE 80 /* The maximum length command */

using namespace std;

int main(void)
{
    char *args[MAX_LINE/2 + 1]; /* command line arguments */
    int should_run = 1; /* flag to determine when to exit program */
    string input;
    string lastCommand;
    int longest = 0;

    while (should_run) {
        printf("osh> ");
        fflush(stdout);
        int argsIndex = 0;

        getline(cin, input);
        
        // check if nothing got inputted
        if (input.empty()) {
            continue;
        }
        
        // exit if user types 'exit'
        if (input == "exit") {
            should_run = 0;
            break;
        }

        // check to see if calling previous command
        if (input == "!!") {
            if (lastCommand.empty()) {
                cout << "No commands in history." << endl;
                continue;
            } 
            input = lastCommand;
        } else {
            lastCommand = input;
        }
            
        // split into each word and store in args array
        istringstream iss(input);
        string word;

        while (iss >> word && argsIndex < (MAX_LINE/2 + 1)) {
            args[argsIndex] = new char[word.size() + 1];
            strcpy(args[argsIndex], word.c_str());
            argsIndex += 1;
        }

        // final entry is null for exec command
        args[argsIndex] = nullptr;
        longest = max(longest, argsIndex);

        // check for & at the end
        bool waitCheck = true;
        if (strcmp(args[argsIndex - 1], "&") == 0) {
            args[argsIndex - 1] = nullptr;
            argsIndex -= 1;
            waitCheck = false;
        }
        
        // fork and start running commands
        int rc = fork();

        if (rc == 0) {
            int fd;
            int pipeIndex = -1;

            // check for redirection (< or >)
            for (int i = 0; i < argsIndex; i++) {
                if (*args[i] == '>') {
                    fd = open(args[i + 1], O_WRONLY | O_CREAT | O_TRUNC, 0644);

                    // redirect to fd
                    dup2(fd, STDOUT_FILENO);

                    // remove the redirection and file
                    args[i] = nullptr;
                    args[i + 1] = nullptr;
                    break;
                } else if (*args[i] == '<') {
                    fd = open(args[i + 1], O_RDONLY);

                    // redirect to console
                    dup2(fd, STDIN_FILENO);

                    // remove the redirection and file
                    args[i] = nullptr;
                    args[i + 1] = nullptr;
                    break;
                } else if (*args[i] == '|') {
                    pipeIndex = i;
                    break;
                }
            }

            // exec command
            if (pipeIndex != -1) {
                char *leftCmd[MAX_LINE/2 + 1];
                char *rightCmd[MAX_LINE/2 + 1];
                
                // get left side command
                for (int i = 0; i < pipeIndex; i++) {
                    leftCmd[i] = args[i];
                }
                leftCmd[pipeIndex] = nullptr; 

                // get right side command
                int temp = 0;
                for (int i = pipeIndex + 1; i < argsIndex; i++) {
                    rightCmd[temp] = args[i];
                    temp++;
                }
                rightCmd[temp] = nullptr;

                // two ends of the pipe
                int pipeFd[2];
                pipe(pipeFd);

                // fork and left command
                int left = fork();
                if (left == 0) {
                    close(pipeFd[0]);
                    dup2(pipeFd[1], STDOUT_FILENO);
                    close(pipeFd[1]);
                    execvp(leftCmd[0], leftCmd);
                    exit(1);
                }

                // fork and right command
                int right = fork();
                if (right == 0) {
                    close(pipeFd[1]);
                    dup2(pipeFd[0], STDIN_FILENO);
                    close(pipeFd[0]);
                    execvp(rightCmd[0], rightCmd);
                    exit(1);
                }
                
                // close the pipe in the parent process
                close(pipeFd[0]);
                close(pipeFd[1]);

                // wait for children to finish
                waitpid(left, NULL, 0);
                waitpid(right, NULL, 0);
            } else {
                execvp(args[0], args);
            }
            
            // so that child doesnt interfere with the parent process
            exit(0);
        } else {
            // wait for child to finish
            if (waitCheck) {
                wait(NULL);
            }
        }
    }

    // free allocated memory
    for (int i = 0; i < longest; i++) {
        delete[] args[i];
    }

    return 0;
}
