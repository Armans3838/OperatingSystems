#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>

#define MAX_LINE 80 /* 80 chars per line, per command */

using namespace std;

int main(void)
{
    char *args[MAX_LINE/2 + 1]; /* max of 40 arguments */
    int should_run = 1;

    int i, upper;

    while (should_run) {
        printf("osh>");
        fflush(stdout);

        /** fork a child to execute command (using one of the exec calls)
            Break out of loop if user types 'quit' or 'exit' then exit
            program **/
        string userInput;
        cin >> userInput; // grabbing our user input here

        // Stop case for exit or quit
        if (userInput == "exit" || userInput == "quit") {
            break;
        }

        //Fork and run inputted command
        if (fork() == 0) {
            execvp("/usr/bin/", reinterpret_cast<char *const *>(&userInput));
            std::cerr << "Error: " << strerror(errno) << '\n';
        }
        else {
            for (char c: userInput) {
                    wait(NULL);
                    break;
            }

        }
    }

    return 0;
}