#include <stdio.h>
#include <stdlib.h>
// for strerror, errno
#include <string.h>
#include <errno.h>
// for fork, exec, wait
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "line.h"
#include "tok.h"

int main(void)
{
    // Prompt for commands until EOF.
    for (char *cmd; (cmd = line_get(">: ")); line_free(cmd))
    {
        // Tokenize the command.
        // Reprompt if the command had no tokens (empty/only whitespace).
        char **args = tok_get(cmd);
        if (!args[0])
        {
            tok_free(args);
            continue;
        }

        // Handle the exit command.
        // Usage: exit [n]
        // If n is not specified or 0, exit will indicate successful completion.
        if (strcmp(args[0], "exit") == 0)
        {
            exit(args[1] ? atoi(args[1]) : EXIT_SUCCESS);
        }

        // Create a child process to execute the command.
        // If not forked, the command replaces the shell.
        pid_t pid = fork();
        switch (pid)
        {
        case -1:
            fprintf(stderr, "err: fork failed: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        case 0:
            // Execute the command and replace the child process.
            execvp(args[0], args);

            // If this is reached, then execvp was unable to replace the child process.
            // In most cases, the command was not found in PATH.
            fprintf(stderr, "'%s' failed: %s\n", args[0], strerror(errno));
            exit(EXIT_FAILURE);
        }

        // The shell (parent) pauses until the command (child) returns.
        int status;
        if (wait(&status) != pid)
        {
            fprintf(stderr, "err: wait failed: %s\n", strerror(errno));
        }

        tok_free(args);
    }

    return EXIT_SUCCESS;
}
