/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exit.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 17:56:59 by mklevero          #+#    #+#             */
/*   Updated: 2025/10/13 16:19:22 by mklevero         ###   ########.fr       */
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
	exit(exit_code);
}

/**
 * Fatal error handler that cleans up and exits the program.
 * Used for unrecoverable errors like memory allocation failures.
 * Performs complete cleanup before terminating.
 *
 * @param msg Error message to display (optional)
 * @param shell Pointer to shell structure to clean up
 * @param exit_code Exit code for the program
 */
void	fatality(char *msg, t_shell *shell, int exit_code)
{
	if (msg)
		ft_putendl_fd(msg, 2);
	if (shell)
		free_shell(shell);
	clear_history();
	exit(exit_code);
}

/**
 * Displays syntax error messages and updates shell state.
 * Handles both custom error messages and token-specific errors.
 * Does not exit the program, allowing minishell to continue.
 *
 * @param msg Custom error message (optional)
 * @param wrong_token Token causing the syntax error (optional)
 * @param shell Pointer to shell structure
 * @param exit_code Exit code to set in minishell
 */
void	show_error(char *msg, t_token *wrong_token, t_shell *shell,
		int exit_code)
{
	if (!shell)
		return ;
	if (msg)
		ft_putendl_fd(msg, 2);
	else
	{
		ft_putstr_fd("syntax error near unexpected token `", 2);
		if (wrong_token == NULL)
			ft_putstr_fd("newline", 2);
		else if (wrong_token->content)
			ft_putstr_fd(wrong_token->content, 2);
		ft_putendl_fd("'", 2);
	}
	free_shell_data(shell);
	shell->exit_code = exit_code;
}

/**
 * Handles lexer-specific errors with appropriate cleanup.
 * Used for memory allocation failures during tokenization.
 * Performs specific cleanup for lexer state before exiting.
 *
 * @param input_line Input line being processed (to free)
 * @param shell Pointer to shell structure
 * @param temp_cont Temporary content to free (optional)
 */
void	lexer_error(char *input_line, t_shell *shell, char *temp_cont)
{
	if (temp_cont)
		free(temp_cont);
	free(input_line);
	free_list(&shell->token_list);
	free_heredoc_files(shell);
	clear_history();
	ft_putendl_fd(ERROR_MEM, 2);
	shell->exit_code = 1;
	exit(shell->exit_code);
}

/**
 * Compares two strings for equality.
 *
 * @param s1 First string
 * @param s2 Second string
 * @return 0 if equal, positive/negative difference otherwise
 */
int	ft_strcmp(const char *s1, const char *s2)
{
	size_t	i;

	i = 0;
	while (s1[i] != '\0' || s2[i] != '\0')
	{
		if ((unsigned char)s1[i] != (unsigned char)s2[i])
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	return (0);
}
