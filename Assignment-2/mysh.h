static int in_fd, out_fd;

void mysh_loop (void);

char * mysh_read(void);

char ** mysh_parse(char *);

int mysh_run(char **);

int mysh_cd(char **);

int mysh_exit(void);

void print_error(void);
