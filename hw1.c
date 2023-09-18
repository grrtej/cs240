/*********************************************
 * Id: Gurtej Saini <sain3777@vandals.uidaho.edu>
 *
 * Compile: gcc -std=c99 -Wall -Wextra -Wpedantic -Werror hw1.c
 * Run: ./a.out
 *
 * Split input into words/tokens
 *********************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_LENGTH 500

int makearg(char *s, char ***args)
{
    const char *whitespace = " \f\n\r\t\v"; // from https://en.cppreference.com/w/c/string/byte/isspace

    int count = 0;
    char **tokens = malloc(sizeof *tokens); // string array (length = 1)
    if (!tokens) // allocation failed
    {
        return -1;
    }

    tokens[count] = strtok(s, whitespace);
    while (tokens[count])
    {
        count++;

        char **tokens_resized = realloc(tokens, (count + 1) * sizeof *tokens); // length++
        if (!tokens_resized) // reallocation failed
        {
            free(tokens);
            return -1;
        }
        tokens = tokens_resized;

        tokens[count] = strtok(NULL, whitespace);
    }

    *args = tokens;
    return count;
}

int main()
{
    printf("Gimme a line of input: ");
    char input[INPUT_LENGTH];
    fgets(input, INPUT_LENGTH, stdin);

    char **argv;
    int argc = makearg(input, &argv);

    printf("That line contains %d words. Here they are:\n", argc);
    for (int i = 0; i < argc; i++)
    {
        printf("%s\n", argv[i]);
    }

    return 0;
}
