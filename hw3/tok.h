#ifndef TOK_H
#define TOK_H

// Returns a NULL-terminated array of tokens in str.
char **tok_get(char *str);

// Frees the array of tokens.
void tok_free(char **tok);

#endif // TOK_H
