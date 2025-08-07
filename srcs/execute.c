/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 15:58:25 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/07 16:29:19 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	error_exit(char	*msg)
{
	perror (msg);
	exit (errno);
}


void	execute_command(t_ast *node, t_shell *shell)
{
	// check builtins
	// otherwise, run established command
	// check other shells logic and figure out mallocd

}


/*
external command:

*/
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
		execute_right_child(node->left, shell, pipefd);
	close(pipefd[0]);
	close(pipefd[1]);
	shell ->exit_code = pipe_wait_children(pids, 2);
}



	// pid[0]= fork();
	// if (pid[0] == -1)
	// 	error_exit("minishell: fork failure" );



int		execute_ast(t_ast *node, t_shell *shell)
{
	if (!node || !shell)
		return(0);
	if (node->type == PIPE)
		execute_pipe(node, shell);
	if (node->type == WORD)
		execute_command(node, shell);

	//check redicrection

	return (shell->exit_code);
}