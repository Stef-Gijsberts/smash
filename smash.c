#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 100
#define MAX_NUM_ARGS 20

static void eval_loop(void);
static void eval(char *const *args);

static void cmd_cd(char *const *args);
static void cmd_exit(char *const *args);
static void launch(char *const *args);

static void split_line(char *line, char *args[], size_t max_num_args);


int main(int argc, const char *argv[])
{
	signal(SIGINT, SIG_IGN);

	eval_loop();

	return EXIT_SUCCESS;
}


static void eval_loop(void)
{
	static char line[MAX_LINE_LENGTH];
	static char *args[MAX_NUM_ARGS];

	for(;;) {
		fputs("> ", stdout);

		if (fgets(line, MAX_LINE_LENGTH, stdin) == NULL) {
			fputs("smash: Could not read a line. Exiting..\n", stderr);
			exit(EXIT_FAILURE);
		}

		split_line(line, args, MAX_NUM_ARGS);
		eval(args);
	}
}


static void eval(char *const *args)
{
	int i;

	if (args[0] == NULL) {
		return;
	}

	if (strcmp(args[0], "cd") == 0)
		cmd_cd(args);

	else if (strcmp(args[0], "exit") == 0)
		cmd_exit(args);

	else
		launch(args);
}


static void cmd_cd(char *const *args)
{
	if (args[1] == NULL) {
		fprintf(stderr, "smash: expected argument to \"cd\"\n");
		return;
	}

	if (chdir(args[1]) != 0) {
		perror("smash");
	}
}


static void cmd_exit(char *const *args)
{
	exit(EXIT_SUCCESS);
}


static void launch(char *const *args)
{
	pid_t pid;
	int status;

	pid = fork();

	if (pid < 0) {
		perror("smash");
		exit(EXIT_FAILURE);
	}

	/* if we are the child */
	if (pid == 0) {
		if (execvp(args[0], args) == -1) {
			perror("smash");
		}
		exit(EXIT_FAILURE);
	}

	/* we are the parent */
	do {
		waitpid(pid, &status, WUNTRACED);
	} while (!WIFEXITED(status) && !WIFSIGNALED(status));
}


static void split_line(char *str, char *args[], size_t argssize)
{
	bool in_string = false;
	size_t i = 0;
	char *begin = str;

	while (*++str != '\0' && i < argssize - 2) {
		if (*str == ' ' || *str == '\n') {
			*str = '\0';
			args[i++] = begin;
			begin = str + 1;
		}
	}

	args[i++] = NULL;
}
