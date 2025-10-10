/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 15:41:16 by mklevero          #+#    #+#             */
/*   Updated: 2025/10/10 17:16:53 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Prints error message into STDERROR and returns provided exit code.
 * @param msg Pointer to the error message.
 * @param ast The ast node being analyzed.
 * @return Exit status of the redirection sequence.
 */
int	write_error_and_return(char *msg, int error)
{
	ft_putstr_fd("minishell: ", STDERR_FILENO);
	perror(msg);
	return (error);
}

/**
 * Executes redirection for infile.
 * @param ast The ast node being analyzed.
 * @return Exit status of the redirection sequence.
 */
int	execute_redirection_in(t_ast *ast)
{
	int	infile_fd;

	infile_fd = open(*ast->value, O_RDONLY);
	if (infile_fd < 0)
		return (write_error_and_return(*ast->value, EXIT_FAILURE));
	if (dup2(infile_fd, STDIN_FILENO) == -1)
	{
		close(infile_fd);
		return (write_error_and_return("dup2", errno));
	}
	close(infile_fd);
	return (0);
}

/**
 * Executes redirection for outfile.
 * @param ast The ast node being analyzed.
 * @param type Enum indicating type of redirection out.
 * @return Exit status of the redirection sequence .
 */
int	execute_redirection_out(t_ast *ast, t_token_type type)
{
	int	outfile_fd;

	if (type == OUT)
		outfile_fd = open(*ast->value, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	else
		outfile_fd = open(*ast->value, O_WRONLY | O_CREAT | O_APPEND, 0644);
	if (outfile_fd < 0)
		return (write_error_and_return(*ast->value, EXIT_FAILURE));
	if (dup2(outfile_fd, STDOUT_FILENO) == -1)
	{
		close(outfile_fd);
		return (write_error_and_return("dup2", errno));
	}
	close(outfile_fd);
	return (0);
}

/**
 * Creates a child process executiong proper redirection.
 * @param ast The ast node being analyzed.
 * @param shell Pointer to the shell struct.
 * @return Exit status of the redirection sequence .
 */
int	check_redirection(t_ast *ast, t_shell *shell)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		return (write_error_and_return("fork", errno));
	if (pid == 0)
	{
		shell->is_parent = false;
		if (ast->type == IN)
			shell->exit_code = execute_redirection_in(ast->right);
		else if (ast->type == OUT || ast->type == APPEND)
			shell->exit_code = execute_redirection_out(ast->right, ast->type);
		if (shell->exit_code != 0)
			fatality(NULL, shell, shell->exit_code);
		fatality(NULL, shell, execute_ast(ast->left, shell));
	}
	return (wait_child(pid, shell));
}
