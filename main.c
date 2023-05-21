#include "shell.h"

/**
 * propmt - print shell prompt
 *
 * Return: void
 */
void prompt(void)
{
	char *prompt = "($) ";
	ssize_t write_c;

	if (isatty(STDIN_FILENO))
	{
		write_c = _puts(prompt);

		if (write_c == -1)
			exit(0);
	}
}

/**
 * 
 */
 int check_empty_line(const char *line)
 {
	 int i = 0;
	 while (line[i] != '\0')
	 {
		 if (line[i] != ' ' && line[i] != '\t' && line[i] != '\n')
			 return (0);
		 i++;
	 }
	 return (1);
 }

int main(int argc , char **argv)
{
	char *line = NULL;
	char **tokens = NULL;
	int exe_st, wstatus;
	size_t line_len = 0;
	ssize_t getline_ret;
	pid_t cpid;
	char *cmd_path = NULL;
	int (*builtin_func)(char *);

	(void)argc;
	/**char *cmd[] = {"/bin/ls", "-l", NULL};*/
	
	/* intializing enviroment variables */
	environ = create_env();
	
	/*
	printf("Path =%s\n", _getenv("PATH"));
	_setenv("new var", "new value", 1);
	printf("Path =%s\n", _getenv("new var"));
	print_env();
	_unsetenv("LS_COLORS");
	print_env();
	*/	
	
	while (1)
	{
		line = NULL;
		tokens = NULL;
		line_len = 0;
		cmd_path = NULL;
		
		prompt();
		
		getline_ret = _getline(&line, &line_len, stdin);
		if (getline_ret == -1)
		{
			free(line);
			free_env();
			_puts("\n");
			exit(EXIT_SUCCESS);
		}
		else if (check_empty_line(line))
		{
			free(line);
			continue;
		}
		tokens = tokenizing(line, " \n");
	
		if(_strncmp(tokens[0], "/", 1) != 0 && _strncmp(tokens[0], "./", 2) != 0 &&
		  _strncmp(tokens[0], "../", 3) != 0)
		{
			builtin_func = check_builtin(tokens[0]);
			if (builtin_func != NULL)
			{
				if(builtin_func(tokens[1]) == -1)
				{
					perror(tokens[0]);
				}
				free(tokens);
				free(line);
				free(cmd_path);
				break;
			}
			else
			{
				cmd_path = check_paths(tokens[0]);
				if (cmd_path != NULL)
					tokens[0] = cmd_path;
			}
		}
		if (access(tokens[0], F_OK | X_OK) == 0)
		{
			/*printf("start\n");*/
			cpid = fork();
			if (cpid == 0)
			{
				/*printf("exexuting... (cpid =%i)\n", cpid);*/
				exe_st = execve(tokens[0], tokens, environ);
				if (exe_st == -1)
				{
					perror(argv[0]);
					exit(0);
				}
			}
			else
			{
				wait(&wstatus);
				/*printf("Done %i : %i (cpid =%i)\n", exe_st, wstatus, cpid);*/
			}
		}
		else
		{
			/*printf("else: \n");*/
			/*errno = ENOENT;*/
			perror(argv[0]);
		}

		free(tokens);
		free(line);
		free(cmd_path);
		
		if (!isatty(STDIN_FILENO))
			break;
	}

	free_env();
	return (0);	
}
