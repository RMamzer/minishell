/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 15:58:25 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/07 19:33:40 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	error_exit(char	*msg)
{
	perror (msg);
	exit (errno);
}

// focus on external cmd firstan
void	check_command(t_ast *node, char *cmd, t_shell *shell)
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
	// 	execute_external_cmd;
}


// check how to include 128 here? --> need to add extra signal return
int	pipe_wait_children(pid_t *pids, int children_rem)
{
	int		status;
	pid_t	term_pid;
	int		exit_code;

	exit_code = EXIT_FAILURE;
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
	shell->exit_code = pipe_wait_children(pids, 2);
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