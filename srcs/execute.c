/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 15:58:25 by rmamzer           #+#    #+#             */
/*   Updated: 2025/09/01 17:20:44 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	write_bulitin_error(char *str1, char *str2, char *str3, char *str4)
{
	if (str1)
		ft_putstr_fd(str1, STDERR_FILENO);
	if (str2)
		ft_putstr_fd(str2, STDERR_FILENO);
	if (str3)
		ft_putstr_fd(str3, STDERR_FILENO);
	if (str4)
		ft_putstr_fd(str4, STDERR_FILENO);
}

int	get_env_size(t_env *lst)
{
	t_env	*tmp;
	int		i;

	i = 0;
	tmp = lst;
	while (tmp)
	{
		tmp = tmp->next;
		i++;
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

// use a main error exit when parent breaks:
// draw the exit path for functions
void	error_exit(char *msg)
{
	perror(msg);
	// free env,
	// if (shell->env)
	// free env
	// free ast?
	exit(errno);
}

void	error_close_and_exit(char *msg, int *pipefd)
{
	close(pipefd[WRITE_END]);
	close(pipefd[READ_END]);
	error_exit(msg);
}

// int	ft_strcmp( const char *s1, const char *s2)
// {
// 	size_t i;

// 	i = 0;
// 	while (s1[i] || s2[i])
// 	{
// 		if (s1[i] != s2[i])
// 			return((unsigned char)s1[i] - (unsigned char)s2[i]);
// 		i++;
// 	}
// 	return (0);
// }

void	recreate_env_array(t_env *env, t_shell *shell)
{
	int		i;
	t_env	*temp;

	i = 0;
	temp = env;
	shell->env_array = malloc(sizeof(char *) * (get_env_size(env) + 1));
	if (!shell->env_array)
		error_exit("minishell: malloc broke");
	while (temp)
	{
		shell->env_array[i] = super_strjoin(temp->key, "=", temp->value);
		if (!shell->env_array[i])
			error_exit("minishell: malloc broke");
		i++;
		temp = temp->next;
	}
	shell->env_array[i] = NULL;
}

void	execute_cmd_child(char **args, t_shell *shell)
{
	char	*env_path;
	char	*joined_path;
	int		i;

	i = 0;
	env_path = (get_env_value("PATH", shell->env, NO_ALLOC));
	if (!env_path)
		error_exit("minishell: cmd: not found");
	shell->paths_array = ft_split(env_path, ':');
	if (!shell->paths_array)
		error_exit("minishell: malloc broke"); // malloc env_paths here
	recreate_env_array(shell->env, shell);
	while (shell->paths_array[i])
	{
		joined_path = super_strjoin(shell->paths_array[i], "/", args[0]);
		if (!joined_path)
			error_exit("minishell: malloc broke"); // malloc env_paths here
		if (access(joined_path, F_OK) == 0)
		{
			if (access(joined_path, X_OK) == 0)
			{
				execve(joined_path, args, shell->env_array);
				free(joined_path);
				error_exit("minishell: execution error");
			}
			free(joined_path);
			error_exit("minishell: cannot execute cmd");
		}
		free(joined_path);
		i++;
	}
}

// check envp or args?
// will need exstra signal check?
// what should i return if not WIFEXITED?
int	execute_external_cmd(char **args, t_shell *shell)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		error_exit("minishell: fork failure");
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
	if (ft_strcmp(cmd, "echo") == 0)
		shell->exit_code = execute_builtin_echo(ast->value + 1);
	else if (ft_strcmp(cmd, "cd") == 0)
		execute_builtin_cd(ast->value + 1, shell);
	else if (ft_strcmp(cmd, "pwd") == 0)
		shell->exit_code = execute_builtin_pwd(ast->value + 1, shell);
	else if (ft_strcmp(cmd, "export") == 0)
		shell->exit_code = execute_builtin_export(ast->value + 1, shell);
	else if (ft_strcmp(cmd, "unset") == 0)
		shell->exit_code = execute_builtin_unset(ast->value + 1, shell);
	else if (ft_strcmp(cmd, "env") == 0)
		shell->exit_code = execute_builtin_env(ast->value + 1, shell);
	else if (ft_strcmp(cmd, "exit") == 0)
		execute_builtin_exit(ast->value + 1, shell);
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
			error_exit("minishell: waitpd failed");
	}
	while (children_rem > 0)
	{
		term_pid = waitpid(-1, &status, 0);
		if (term_pid == -1)
			error_exit("minishell: waitpd failed");
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
		ft_putstr_fd("minishell: fork failure\n", STDERR_FILENO);
		exit(errno);
	}
	close(pipefd[WRITE_END]);
	shell->exit_code = execute_ast(ast, shell);
	exit(shell->exit_code);
}

void	execute_right_child(t_ast *ast, t_shell *shell, int *pipefd)
{
	close(pipefd[WRITE_END]);
	if (dup2(pipefd[READ_END], STDIN_FILENO) == -1)
	{
		close(pipefd[READ_END]);
		ft_putstr_fd("minishelll: fork failure\n", STDERR_FILENO);
		exit(errno);
	}
	close(pipefd[READ_END]);
	shell->exit_code = execute_ast(ast, shell);
	exit(shell->exit_code);
}
// need to close read and write ends of pipe
void	execute_pipe(t_ast *ast, t_shell *shell)
{
	int		pipefd[2];
	pid_t	pids[2];

	if (pipe(pipefd) == -1)
		error_exit("minishell: pipe failure");
	pids[0] = fork();
	if (pids[0] == -1)
		error_close_and_exit("minishell: fork falure", pipefd);
	if (pids[0] == 0)
		execute_left_child(ast->left, shell, pipefd);
	pids[1] = fork();
	if (pids[1] == -1)
		error_close_and_exit("minishell: fork falure", pipefd);
	if (pids[1] == 0)
		execute_right_child(ast->right, shell, pipefd);
	close(pipefd[WRITE_END]);
	close(pipefd[READ_END]);
	shell->exit_code = wait_children(pids, 2);
}

int	execute_ast(t_ast *ast, t_shell *shell)
{
	if (!ast || !shell)
		return (0);
	if (ast->type == PIPE)
		execute_pipe(ast, shell);
	else if (ast->type == WORD)
		check_command(ast, ast->value[0], shell);
	else if (ast->type >= IN && ast->type <= APPEND)
		shell->exit_code = check_redirection (ast, shell);
	return (shell->exit_code); // < do i need it if i modify using pointers?
}