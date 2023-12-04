#ifndef LINE_H
#define LINE_H

// Returns a line read from stdin.
// Returns NULL on EOF.
char *line_get(void);

// Frees the line.
void line_free(char *line);

#endif // LINE_H
