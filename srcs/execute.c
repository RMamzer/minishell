/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 15:58:25 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/13 15:57:48 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	error_exit(char	*msg)
{
	perror(msg);
	exit(errno);
}


int	ft_strcmp( const char *s1, const char *s2)
{
	size_t i;

	i = 0;
	while (s1[i] || s2[i])
	{
		if (s1[i] != s2[i])
			return((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	return (0);
}



char *find_env_value(char *str, t_env *env)
{
	while (env!= NULL)
	{
		if (ft_strcmp(str, env->key))
			return (env->content);
		env = env->next;
	}
	return (NULL);
}


void	execute_cmd_child(char **args, t_shell *shell)
{
	char *env_paths;
	env_paths = find_env_value("PATH", shell->env);
	if (!env_paths)
		error_exit("minishell: cmd: not found") // should return minishell" <cmd> not found????
}

// check envp or args?
// will need exstra signal check?
// what should i return if not WIFEXITED?
int	execute_external_cmd(char	**args, t_shell *shell)
{
pid_t	pid;

pid = fork();
if (pid == -1)
	error_exit("minishell: fork failure");
if (pid == 0)
	execute_cmd_child(args, shell);
waitpid(pid, &shell->exit_code, 0);
if (WIFEXITED(shell->exit_code))
	return(WEXITSTATUS(shell->exit_code));
return (EXIT_FAILURE);
}

// do i need to check empty cmd here?
int	check_command(t_ast *node, char *cmd, t_shell *shell)
{
	// if (ft_strcmp(cmd, "echo"))
	// 	execute_builtin_echo;
	// else if (ft_strcmp(cmd, "cd"))
	// 	execute_builtin_cd;
	// else if (ft_strcmp(cmd, "pwd"))
	// 	execute_builtin_pwd;
	// else if (ft_strcmp(cmd, "export"))
	// 	execute_builtin_export;
	// else if (ft_strcmp(cmd, "unset"))
	// 	execute_builtin_unset;
	// else if (ft_strcmp(cmd, "env"))
	// 	execute_builtin_env;
	// else if (ft_strcmp(cmd, "exit"))
	// 	execute_builtin_exit;
	// else
	shell->exit_code = execute_external_cmd(node->value, shell);
	return (shell->exit_code);
}


// check how to include 128 here? --> need to add extra signal return
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
	return(exit_code);
}

void	execute_left_child(t_ast *node, t_shell *shell, int *pipefd)
{
	close (pipefd[READ_END]);
	if (dup2(pipefd[WRITE_END], STDOUT_FILENO)== -1)
	{
		close(pipefd[WRITE_END]);
		error_exit("minishell: fork failure");
	}
	close (pipefd[WRITE_END]);
	shell->exit_code = execute_ast(node, shell);
	exit(shell->exit_code);
}

void	execute_right_child(t_ast *node, t_shell *shell, int *pipefd)
{
	close (pipefd[WRITE_END]);
	if (dup2(pipefd[READ_END], STDIN_FILENO) == -1)
	{
		close (pipefd[READ_END]);
		error_exit("minishelll: fork failure");
	}
	close (pipefd[READ_END]);
	shell->exit_code = execute_ast(node, shell);
	exit (shell->exit_code);
}

void	execute_pipe(t_ast *node, t_shell *shell)
{
	int	pipefd[2];
	pid_t	pids[2];

	if (pipe(pipefd) == -1)
		error_exit ("minishell: pipe failure");
	pids[0] = fork();
	if (pids[0] == -1)
		error_exit("minishell: fork falure");
	if (pids[0] == 0)
		execute_left_child(node->left, shell, pipefd);
	pids[1] = fork();
	if (pids[1] == -1)
		error_exit("minishell: fork falure");
	if (pids[1] == 0)
		execute_right_child(node->right, shell, pipefd);
	close(pipefd[WRITE_END]);
	close(pipefd[READ_END]);
	shell->exit_code = wait_children(pids, 2);
}

int		execute_ast(t_ast *node, t_shell *shell)
{
	if (!node || !shell)
		return(0);
	if (node->type == PIPE)
		execute_pipe(node, shell);
	if (node->type == WORD)
		check_command(node, node->value[0], shell);

	//check redicrection

	return (shell->exit_code); // < do i need it if i modify using pointers?
}