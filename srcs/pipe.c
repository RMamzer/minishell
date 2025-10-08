/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipe.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 21:56:42 by rmamzer           #+#    #+#             */
/*   Updated: 2025/10/08 22:06:53 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
	shell->is_parent = false;
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
	shell->is_parent = false;
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
	if (shell->is_parent == true)
		set_signals_exec_parent();
	return (wait_pipe(pids, 2, shell));
}