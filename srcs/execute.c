/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2025/09/12 19:24:31 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "minishell.h"


// // TESTING
// void    print_char_array(char **arr)
// {
//     // A counter for the index of the array
//     int i = 0;

//     // Check if the array itself is NULL before attempting to access it
//     if (!arr)
//         return;

//     // Loop until a NULL pointer is encountered.
//     // In C, string arrays are often terminated by a NULL pointer.
//     while (arr[i] != NULL)
//     {
//         // Use printf for formatted output. The `%s` specifier is for strings.
//         // A newline `\n` is added for clean output.
//         printf("%d:%s\n", i, arr[i]);
//         i++;
//     }
// }



// // for testing, delete later
// void print_envp(char *const *envp) {
//     if (envp == NULL) {
//         printf("(NULL envp)\n");
//         return;
//     }

//     // Iterate through the array of pointers until the null terminator is reached.
//     for (int i = 0; envp[i] != NULL; i++) {
//         printf("%s\n", envp[i]);
//     }
// }

// REMOVE AND FIX ENV EXITS
void	error_exit(char *msg)
{
	perror(msg);
	exit (errno);
}

void	write_bulitin_error(char *str1, char *str2, char *str3, char *str4)
{
	if (str1)
		ft_putstr_fd(str1, STDERR_FILENO);
	if (str2)
		ft_putstr_fd(str2, STDERR_FILENO);
	if (str3)
		ft_putstr_fd(str3, STDERR_FILENO);
	if (str4)
		perror(str4);
}

void	free_execution(t_shell *shell)
{
	if (!shell)
		return ;
	if (shell->env_array)
		free_array(shell->env_array);
	if (shell->paths_array)
		free_array(shell->paths_array);
}

/*
Do we need to add additional message here?
What to free here:
1. Path ft_split
2. Recreated env array of arrays
*/
void 	write_error_malloc()
{
	perror("minishell: malloc");
	exit(errno);
}

void	error_exec_exit(char *str1, t_shell *shell)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	if (*str1)
		ft_putstr_fd(str1, STDERR_FILENO);
	free_execution(shell);
	exit (errno);
}


int	get_env_size(t_env *lst, bool process)
{
	t_env	*tmp;
	int		i;

	i = 0;
	tmp = lst;
	while (tmp)
	{
		if (process == EXPORT || tmp->assigned == true)
			i++;
		tmp = tmp->next;
	}
	return (i);
}

int	get_args_len(char **args)
{
	int	len;

	len = 0;
	while (args[len] != NULL)
		len++;
	return (len);
}

char	*super_strjoin(char const *s1, char const *s2, char const *s3)
{
	char	*joinedstr;
	size_t	str1_l;
	size_t	str2_l;
	size_t	str3_l;

	if (!s1 && !s2 && !s3)
		return (NULL);
	str1_l = ft_strlen(s1);
	str2_l = ft_strlen(s2);
	str3_l = ft_strlen(s3);
	joinedstr = malloc(str1_l + str2_l + str3_l + 1);
	if (!joinedstr)
		return (NULL);
	ft_memcpy(joinedstr, s1, str1_l);
	ft_memcpy(joinedstr + str1_l, s2, str2_l);
	ft_memcpy(joinedstr + str1_l + str2_l, s3, str3_l);
	joinedstr[str1_l + str2_l + str3_l] = '\0';
	return (joinedstr);
}





int	error_close_and_return(char *msg, int *pipefd, int error)
{
	close(pipefd[WRITE_END]);
	close(pipefd[READ_END]);
	return (write_error_and_return(msg, error));
}

void	recreate_env_array(t_env *env, t_shell *shell)
{
	int		i;
	t_env	*temp;

	i = 0;
	temp = env;
	shell->env_array = malloc(sizeof(char *) * (get_env_size(env, EXECUTE) + 1));
	if (!shell->env_array)
		write_error_malloc();
	while (temp)
	{

		if (temp->assigned == true)
		{
			shell->env_array[i] = super_strjoin(temp->key, "=", temp->value);
			if (!shell->env_array[i])
			{
				free_execution(shell);
				write_error_malloc();
			}
			i++;
		}
		temp = temp->next;
	}
	shell->env_array[i] = NULL;
}


char	*find_path_cmd(char**args, bool *malocced, t_shell *shell)
{
	char	*cmd_path;
	int		i;

	if (ft_strchr(args[0], '/'))
		return (args[0]);
	i = 0;
	*malocced = true;
	while (shell->paths_array[i])
	{
		cmd_path = super_strjoin(shell->paths_array[i], "/", args[0]);
		if (!cmd_path)
		{
			free_execution(shell);
			write_error_malloc();
		}
		if (access(cmd_path, F_OK) == 0)
			return (cmd_path);
		free(cmd_path);
		i++;
	}
	write_bulitin_error("minishell: ", *args, ": command not found\n", NULL);
	free_execution (shell);
	exit(EXIT_CMD_NOT_FOUND);
}


//CHECK ENV_ARRAY AND PATHS_ARRAY frees
void	execute_cmd_child(char **args, t_shell *shell)
{
	char	*env_path;
	char	*cmd_path;
	bool	malloced;

	malloced = false;
	env_path = (get_env_value("PATH", shell->env, NO_ALLOC));
	if (!env_path)
	{
		write_bulitin_error("minishell: ", *args, ": command not found\n", NULL);
		exit(EXIT_CMD_NOT_FOUND);
	}
	shell->paths_array = ft_split(env_path, ':');
	if (!shell->paths_array)
		write_error_malloc(); // malloc env_paths here
	recreate_env_array(shell->env, shell);
	cmd_path = find_path_cmd(args, &malloced, shell);
	if (access(cmd_path, X_OK)== 0)
		execve(cmd_path, args, shell->env_array);
	if (malloced == true)
		free (cmd_path);
	error_exec_exit(args[0], shell);
}



// check envp or args?
// will need exstra signal check?
// what should i return if not WIFEXITED?
int	execute_external_cmd(char **args, t_shell *shell)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		return (write_error_and_return("fork", errno));
	if (pid == 0)
		execute_cmd_child(args, shell);
	waitpid(pid, &shell->exit_code, 0);
	if (WIFEXITED(shell->exit_code))
		return (WEXITSTATUS(shell->exit_code));
	return (EXIT_FAILURE);
}

// do i need to check empty cmd here?
int	check_command(t_ast *ast, char *cmd, t_shell *shell)
{
	if (!*cmd)
		shell->exit_code = 0;
	else if (ft_strcmp(cmd, "echo") == 0)
		shell->exit_code = execute_builtin_echo(ast->value + 1);
	else if (ft_strcmp(cmd, "cd") == 0)
		shell->exit_code = execute_builtin_cd(ast->value + 1, shell);
	else if (ft_strcmp(cmd, "pwd") == 0)
		shell->exit_code = execute_builtin_pwd(ast->value + 1, shell);
	else if (ft_strcmp(cmd, "export") == 0)
		shell->exit_code = execute_builtin_export(ast->value + 1, shell);
	else if (ft_strcmp(cmd, "unset") == 0)
		shell->exit_code = execute_builtin_unset(ast->value + 1, shell);
	else if (ft_strcmp(cmd, "env") == 0)
		shell->exit_code = execute_builtin_env(ast->value + 1, shell);
	else if (ft_strcmp(cmd, "exit") == 0)
		shell->exit_code = execute_builtin_exit(ast->value + 1, shell);
	else
		shell->exit_code = execute_external_cmd(ast->value, shell);
	return (shell->exit_code);
}

// check how to include 128+ exit here? --> need to add extra signal return
int	wait_children(pid_t *pids, int children_rem)
{
	int		status;
	pid_t	term_pid;
	int		exit_code;

	exit_code = EXIT_FAILURE;
	if (children_rem == 1)
	{
		term_pid = waitpid(*pids, &status, 0);
		if (term_pid == -1)
			return(write_error_and_return("waitpd", errno));
	}
	while (children_rem > 0)
	{
		term_pid = waitpid(-1, &status, 0);
		if (term_pid == -1)
			return(write_error_and_return("waitpd", errno));
		if (term_pid == pids[0] || term_pid == pids[1])
		{
			children_rem--;
			if (term_pid == pids[1] && (WIFEXITED(status)))
				exit_code = WEXITSTATUS(status);
		}
	}
	return (exit_code);
}

void	execute_left_child(t_ast *ast, t_shell *shell, int *pipefd)
{
	close(pipefd[READ_END]);
	if (dup2(pipefd[WRITE_END], STDOUT_FILENO) == -1)
	{
		close(pipefd[WRITE_END]);
		exit (write_error_and_return("dup2", errno));
		exit (write_error_and_return("dup2", errno));
	}
	close(pipefd[WRITE_END]);
	exit(execute_ast(ast, shell));
	exit(execute_ast(ast, shell));
}

void	execute_right_child(t_ast *ast, t_shell *shell, int *pipefd)
{
	close(pipefd[WRITE_END]);
	if (dup2(pipefd[READ_END], STDIN_FILENO) == -1)
	{
		close(pipefd[READ_END]);
		exit (write_error_and_return("dup2", errno));
		exit (write_error_and_return("dup2", errno));
	}
	close(pipefd[READ_END]);
	exit(execute_ast(ast, shell));
	exit(execute_ast(ast, shell));
}
// need to close read and write ends of pipe

int	execute_pipe(t_ast *ast, t_shell *shell)
{
	int		pipefd[2];
	pid_t	pids[2];

	shell->complete_exit = false;
	if (pipe(pipefd) == -1)
		write_error_and_return("pipe", errno);
	pids[0] = fork();
	if (pids[0] == -1)
		error_close_and_return("fork", pipefd, errno);
	if (pids[0] == 0)
		execute_left_child(ast->left, shell, pipefd);
	pids[1] = fork();
	if (pids[1] == -1)
		error_close_and_return("fork", pipefd, errno);
	if (pids[1] == 0)
		execute_right_child(ast->right, shell, pipefd);
	close(pipefd[WRITE_END]);
	close(pipefd[READ_END]);
	return (wait_children(pids, 2));
}

int	execute_ast(t_ast *ast, t_shell *shell)
{
	if (!ast || !shell)
		return (0);
	if (ast->type == PIPE)
		shell->exit_code = execute_pipe(ast, shell);
	else if (ast->type == WORD)
		shell-> exit_code = check_command(ast, ast->value[0], shell);
	else if (ast->type >= IN && ast->type <= APPEND)
		shell->exit_code = check_redirection (ast, shell);
	return (shell->exit_code); // < do i need it if i modify using pointers?
}