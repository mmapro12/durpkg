#ifndef UTILS_H
#define UTILS_H

#define COLOR_RED "\033[31m"
#define COLOR_GREEN "\033[32m"
#define COLOR_YELLOW "\033[33m"
#define COLOR_BLUE "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN "\033[36m"
#define COLOR_RESET "\033[0m"

#define VERSION "0.0.1"

// ask to continue and get the answer y or n 
int asknget(char*);

// Clears the "'s that are in string
void clear_quotes(char*);

void print_help(void);

void print_version(void);

int validate_url(char*);

int check_url_exists(const char*);

#endif // !UTILS_H
