void mysh_loop (int, char **);

char * mysh_read(void);

char ** mysh_parse(char *);

int mysh_run(char **);

int mysh_cd(char **);

int mysh_exit(void);

int redirect_out (char **);

void check_python (char *);

int background (char *);

void print_error(void);
