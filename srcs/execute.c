/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamzerr1 <mamzerr1@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2025/09/30 13:50:38 by mamzerr1         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * FINISH THIS PROCESS (๑•̀ᗝ•́)૭ 
 * @param msg Pointer to erorr message string.
 * @param shell Pointer to the shell struct.
 * @param exit_code Exit status of the process.
 * @return Void.
 */
void	brutality(char *msg, t_shell *shell, int exit_code)
{
	if (msg)
		perror(msg);
	if (shell)
		free_shell(shell);
	clear_history();
	exit (exit_code);
}

/**
 * Outputs a custom error message into STDERROR
 * with or without perror message.
 * @param str1 Pointer to the first sting that should be otputted.
 * @param str2 Pointer to the second sting that should be otputted.
 * @param str3 Pointer to the third sting that should be otputted.
 * @param str4 Pointer to a sting that should be outputted with perror.
 * @return Void.
 */
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

/**
 * Calculates the number of relevant nodes in the environment list.
 * @param lst Pointer to the first node of environment list.
 * @param process Type of process being executed (EXECUTE or EXPORT) 
 * @return Number of the relevant nodes in environment list.
 */
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


/**
 * Calculates the number of arguments in the char string.
 * @param args Array of arguments of type char.
 * @return Length of array of arguments.
 */
int	get_args_len(char **args)
{
	int	len;

	len = 0;
	while (args[len] != NULL)
		len++;
	return (len);
}
/**
 * Joins up to 3 strings in single malloc returns a pointer to new string. 
 * @param s1 Pointer to the first sting that should be joined.
 * @param s2 Pointer to the second sting that should be joined.
 * @param s3 Pointer to a third sting that should be joined.
 * @return New malloced  joined sting.
 */
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


/**
 * Closes READ_END and WRITE_END of pipe() and returns error exit status. 
 * @param msg Error message that should be outputed.
 * @param pipefs Pointer to READ_END and WRITE_END of pipe().
 * @param pipefs Error exit status.
 * @return Error exit status.
 */
int	error_close_and_return(char *msg, int *pipefd, int error)
{
	close(pipefd[WRITE_END]);
	close(pipefd[READ_END]);
	return (write_error_and_return(msg, error));
}

/**
 * Recreates envp array using the list of env nodes. 
 * @param env First node of the environment list.
 * @param shell Pointer to the shell struct.
 * @return Void.
 */
void	recreate_env_array(t_env *env, t_shell *shell)
{
	int		i;
	t_env	*temp;

	i = 0;
	temp = env;
	shell->env_array = malloc(sizeof(char *) * (get_env_size(env, EXECUTE)
				+ 1));
	if (!shell->env_array)
		fatality(ERROR_MEM, shell, 1);
	while (temp)
	{
		if (temp->assigned == true)
		{
			shell->env_array[i] = super_strjoin(temp->key, "=", temp->value);
			if (!shell->env_array[i])
				fatality(ERROR_MEM, shell, 1);
			i++;
		}
		temp = temp->next;
	}
	shell->env_array[i] = NULL;
}


/**
 * Waits for the execution of pipe child processes to collect final
 * exit status.
 * @param pid  Id of the child process executing external cmd.
 * @param shell Pointer to the shell struct.
 * @return exit status of the child process executing cmd.
 */
int 	wait_child(pid_t pid, t_shell *shell)
{
	int		status;
	pid_t	term_pid;
	int		exit_code;

	if (shell->complete_exit == true)
		set_signals_exec_parent();
	exit_code = EXIT_FAILURE;
	term_pid = waitpid(pid, &status, 0);
	if (term_pid == -1)
			return (write_error_and_return("waitpd", EXIT_FAILURE));
	if (WIFEXITED(status))
		exit_code = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		exit_code = 128 + WTERMSIG(status);
	return (exit_code);
}

/**
 * Reassembles envp array for execve, finds path to cmd and executes cmd.
 * @param args Array of arguments containing cmd name and its arguments.
 * @param shell Pointer to the shell struct.
 * @return Void.
 */
char	*find_path_cmd(char **args, bool *malocced, t_shell *shell)
{
	char	*cmd_path;
	int		i;

	if (ft_strchr(args[0], '/'))
	{
		if (access(args[0], F_OK) == 0)
			return (args[0]);
		write_bulitin_error("minishell: ", NULL, NULL, args[0]);
		brutality(NULL, shell, EXIT_CMD_NOT_FOUND);
	}
	i = 0;
	*malocced = true;
	while (shell->paths_array[i])
	{
		cmd_path = super_strjoin(shell->paths_array[i], "/", args[0]);
		if (!cmd_path)
			fatality(ERROR_MEM, shell, 1);
		if (access(cmd_path, F_OK) == 0)
			return (cmd_path);
		free(cmd_path);
		i++;
	}
	write_bulitin_error("minishell: ", *args, ": command not found\n", NULL);
	brutality(NULL, shell, EXIT_CMD_NOT_FOUND);
	return (NULL);
}


/**
 * Reassembles envp array for execve, finds path to cmd and executes cmd.
 * @param args Array of arguments containing cmd name and its arguments.
 * @param shell Pointer to the shell struct.
 * @return Void.
 */
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
		brutality(NULL, shell, EXIT_CMD_NOT_FOUND);
	}
	shell->paths_array = ft_split(env_path, ':');
	if (!shell->paths_array)
		fatality(ERROR_MEM, shell, 1);
	recreate_env_array(shell->env, shell);
	cmd_path = find_path_cmd(args, &malloced, shell);
	if (access(cmd_path, X_OK) == 0)
	{
		child_signal();
		execve(cmd_path, args, shell->env_array);
	}
	if (malloced == true)
		free (cmd_path);
	write_bulitin_error("minishell:", NULL, NULL, args[0]);
	brutality(NULL, shell, EXIT_CMD_NOT_EXEC);
}



/**
 * Creates a child process to execute an external command.
 * @param args Array of arguments containing cmd name and its arguments.
 * @param shell Pointer to the shell struct.
 * @return exit status of the command execution.
 */
int	execute_external_cmd(char **args, t_shell *shell)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		return (write_error_and_return("fork", EXIT_FAILURE));
	if (pid == 0)
		execute_cmd_child(args, shell);
	return (wait_child(pid, shell));
}

/**
 * Verifies if cmd is a build-in or external command and starts
 * its execution.
 * @param ast The ast containing right side of the pipe execution.
 * @param cmd Command being processed.
 * @param shell Pointer to the shell struct.
 * @return Exit status of the command execution.
 */
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

/**
 * Waits for the execution of pipe child processes to collect final
 * exit status.
 * @param pids The array with 2 pids (left chinf and right child).
 * @param children_rem How many children should be waited for.
 * @param shell Pointer to the shell struct.
 * @return Exit status of the pipe pipe execution sequence.
 */
int	wait_pipe(pid_t *pids, int children_rem, t_shell *shell)
{
	int		status;
	pid_t	term_pid;
	int		exit_code;

	if (shell->exit_code == true)
		set_signals_exec_parent();
	exit_code = EXIT_FAILURE;
	while (children_rem > 0)
	{
		term_pid = waitpid(-1, &status, 0);
		if (term_pid == -1)
			return (write_error_and_return("waitpd", EXIT_FAILURE));
		if (term_pid == pids[0] || term_pid == pids[1])
		{
			children_rem--;
			if (term_pid == pids[1])
			{
 				if (WIFEXITED(status))
					exit_code = WEXITSTATUS(status);
				else if (WIFSIGNALED(status))
					exit_code = 128 + WTERMSIG(status);
			}
		}
	}
	return (exit_code);
}

/**
 * Executes pipe redirection for left side of the pipe.
 * @param ast The ast node containing left side of the pipe execution.
 * @param shell Pointer to the shell struct.
 * @param pipefd Pointer to the WRITE_END of pipe().
 * @return Void.
 */
void	execute_left_child(t_ast *ast, t_shell *shell, int *pipefd)
{
	shell->complete_exit = false;
	close(pipefd[READ_END]);
	if (dup2(pipefd[WRITE_END], STDOUT_FILENO) == -1)
	{
		close(pipefd[WRITE_END]);
		brutality("minishell: dup2",shell, EXIT_FAILURE);
	}
	close(pipefd[WRITE_END]);
	fatality(NULL, shell, execute_ast(ast, shell));
}


/**
 * Executes pipe redirection for right side of the pipe.
 * @param ast The ast containing right side of the pipe execution.
 * @param shell Pointer to the shell struct.
 * @param pipefd Pointer to the READ_END of pipe().
 * @return Void.
 */
void	execute_right_child(t_ast *ast, t_shell *shell, int *pipefd)
{
	shell->complete_exit = false;
	close(pipefd[WRITE_END]);
	if (dup2(pipefd[READ_END], STDIN_FILENO) == -1)
	{
		close(pipefd[READ_END]);
		brutality("minishel: dup2",shell, EXIT_FAILURE);
	}
	close(pipefd[READ_END]);
	fatality(NULL, shell, execute_ast(ast, shell));
}

/**
 * Processes execution of a pipe in the ast, creates a children process
 * for each side of a pipe.
 * @param ast The pipe ast node being analyzed .
 * @param shell Pointer to the shell struct.
 * @return Exit status of the pipe execution sequence.
 */
int	execute_pipe(t_ast *ast, t_shell *shell)
{
	int		pipefd[2];
	pid_t	pids[2];

	if (pipe(pipefd) == -1)
		write_error_and_return("pipe", EXIT_FAILURE);
	pids[0] = fork();
	if (pids[0] == -1)
		error_close_and_return("fork", pipefd, EXIT_FAILURE);
	if (pids[0] == 0)
		execute_left_child(ast->left, shell, pipefd);
	pids[1] = fork();
	if (pids[1] == -1)
		error_close_and_return("fork", pipefd, EXIT_FAILURE);
	if (pids[1] == 0)
		execute_right_child(ast->right, shell, pipefd);
	close(pipefd[WRITE_END]);
	close(pipefd[READ_END]);
	if (shell->complete_exit == true)
		set_signals_exec_parent();
	return (wait_pipe(pids, 2, shell));
}

/**
 * Starts the recursion executing pipes, redirections, and cmds from ast.
 * @param ast The ast node being analyzed (1st on the initial run).
 * @param shell Pointer to the shell struct.
 * @return Exit status of the execution sequence.
 */
int	execute_ast(t_ast *ast, t_shell *shell)
{
	if (!ast || !shell)
		return (0);
	if (ast->type == WORD)
		shell->exit_code = check_command(ast, ast->value[0], shell);
	if (ast->type == PIPE)
		shell->exit_code = execute_pipe(ast, shell);
	else if (ast->type >= IN && ast->type <= APPEND)
		shell->exit_code = check_redirection(ast, shell);
	return (shell->exit_code);
}