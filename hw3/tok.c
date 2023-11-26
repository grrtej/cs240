#include "tok.h"

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

// Returns a NULL-terminated array of tokens in str.
char **tok_get(char *str)
{
    // Allocate an empty array (contains only NULL).
    size_t tok_size = 1;
    char **tok = malloc(tok_size * sizeof *tok);
    if (!tok)
    {
        fprintf(stderr, "err: tok malloc failed\n");
        exit(EXIT_FAILURE);
    }
    tok[tok_size - 1] = NULL;

    // Search for tokens in str delimited by whitespace.
    for (size_t start = 0, end = 0; str[start] != '\0';)
    {
        // Not in a token:
        // Skip all whitespace.
        if (isspace(str[start]) && isspace(str[end]))
        {
            end++;
            start = end;
        }
        // In a token:
        // Find the end until a whitespace or end of string is encountered.
        else if (!isspace(str[start]) && !isspace(str[end]) && str[end] != '\0')
        {
            end++;
        }
        // Append the recognized token to the array.
        else
        {
            // Resize the array.
            char **tok_resized = realloc(tok, ++tok_size * sizeof *tok);
            if (!tok_resized)
            {
                fprintf(stderr, "err: tok realloc failed\n");
                exit(EXIT_FAILURE);
            }
            tok = tok_resized;

            const size_t tok_cur = tok_size - 2;
            const size_t tok_cur_size = end - start + 1;

            // Allocate a string for current token.
            tok[tok_cur] = malloc(tok_cur_size * sizeof *tok[tok_cur]);
            if (!tok[tok_cur])
            {
                fprintf(stderr, "err: tok_cur malloc failed\n");
                exit(EXIT_FAILURE);
            }

            // Copy the current token into the array.
            for (size_t i = 0; i < tok_cur_size; i++)
            {
                tok[tok_cur][i] = str[start + i];
            }
            tok[tok_cur][tok_cur_size - 1] = '\0';

            // NULL-terminate the array.
            tok[tok_size - 1] = NULL;

            // Move out as this token has been appended.
            start = end;
        }
    }

    return tok;
}

// Frees the array of tokens.
void tok_free(char **tok)
{
    for (size_t i = 0; tok[i] != NULL; i++)
    {
        free(tok[i]);
    }

    free(tok);
}
