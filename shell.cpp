#include <bits/stdc++.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_LINE 80 /* The maximum length command */

using namespace std;

int main(void)
{
    char *args[MAX_LINE/2 + 1]; /* command line arguments */
    int should_run = 1; /* flag to determine when to exit program */
    string input;

    while (should_run) {
        printf("osh>");
        fflush(stdout);
        int argsIndex = 0;

        getline(cin, input);
        
        // exit if user types 'exit'
        if (input == "exit") {
            should_run = 0;
            break;
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

        // free allocated memory
        for (int i = 0; i < argsIndex; i++) {
            delete[] args[i];
        }
    }
    
    return 0;
}
