#ifndef VERROR_H
#define VERROR_H

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

void verror(const char* format, ...);

int close_file(FILE *file, const char *file_name);

#define RED           "\x1b[31;1m"
#define END_OF_RED "\x1b[39;49m\n" 

#define VERROR(...) do                                              \
    {                                                               \
        fprintf(stderr, RED "ERROR:%s (%d)\n", __FILE__, __LINE__); \
        verror(__VA_ARGS__);                                        \
        fprintf(stderr, END_OF_RED);                                \
    } while (0)

#define VERROR_FILL_FILE(file_name) do                                              \
    {                                                                               \
        VERROR("faild to fill the %s %s %s", (file_name), "file:", strerror(errno));\
    } while (0)  

#define VERROR_FOPEN(file_name) do                                                  \
    {                                                                               \
       VERROR("file %s %s %s", (file_name), "cannot be opened:", strerror(errno));  \
    } while (0)  
    
#define VERROR_FCLOSE(file_name) do                                                 \
    {                                                                               \
       VERROR("file %s %s %s", (file_name), "cannot be closed:", strerror(errno));  \
    } while (0)  

#define VERROR_FWRITE(file_name) do                         \
    {                                                       \
       VERROR("troubles filling the file %s", (file_name)); \
    } while (0)
#define VERROR_MEM VERROR("memory allocation failure")

#define VERROR_SSCANF(line) VERROR("troubles reading the line %s", line)
                                                    
#endif
