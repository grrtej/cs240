#include "line.h"

#include <stdio.h>
#include <stdlib.h>

// Returns a line read from stdin.
// Returns NULL on EOF.
char *line_get(void)
{
    // Allocate an empty string.
    size_t line_size = 1;
    char *line = malloc(line_size * sizeof *line);
    if (!line)
    {
        fprintf(stderr, "err: line malloc failed\n");
        exit(EXIT_FAILURE);
    }
    line[line_size - 1] = '\0';

    // Get an input character from stdin until EOF or newline.
    for (int input; (input = getc(stdin)) != EOF && input != '\n';)
    {
        // Resize the string.
        char *line_resized = realloc(line, ++line_size * sizeof *line);
        if (!line_resized)
        {
            fprintf(stderr, "err: line realloc failed\n");
            exit(EXIT_FAILURE);
        }
        line = line_resized;

        // Append the input character and terminate the string.
        line[line_size - 2] = input;
        line[line_size - 1] = '\0';
    }

    // On ^D/EOF, discard the input and return NULL.
    if (feof(stdin))
    {
        line_free(line);
        return NULL;
    }

    // Otherwise, return what was read until the newline.
    return line;
}

// Frees the line.
void line_free(char *line)
{
    free(line);
}
