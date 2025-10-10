/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checkers.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 17:56:25 by mklevero          #+#    #+#             */
/*   Updated: 2025/10/10 16:19:26 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Determines the quote context at a specific position in the string.
 * Tracks whether the character at 'index' is inside quotes and which type.
 *
 * @param line Input string to analyze
 * @param index Position to check quote context for
 * @return Quote character if inside quotes (', "), 0 if outside quotes
 */
char	check_quote(char *line, int index)
{
	int		i;
	char	quote;

	i = 0;
	quote = 0;
	while (i < index)
	{
		if (quote == 0 && (line[i] == '\'' || line[i] == '"'))
			quote = line[i];
		else if (quote != 0 && line[i] == quote)
			quote = 0;
		i++;
	}
	return (quote);
}

/**
 * Processes heredoc tokens and enforces limits.
 * Counts heredoc operators and sets delimiter types based on quoting.
 * Enforces the bash limit of 16 heredocs per command line.
 *
 * @param shell Pointer to shell structure containing token list
 * @return SUCCESS if heredoc usage is valid, FAILURE if limit exceeded
 */
bool	check_heredoc(t_shell *shell)
{
	t_token	*current;
	int		count;

	count = 0;
	current = shell->token_list;
	while (current)
	{
		if (current->type == HEREDOC && current->next)
		{
			count++;
			if (current->next->quoted == true)
				current->next->type = HEREDOC_DELIM_QT;
			else
				current->next->type = HEREDOC_DELIM_UQ;
		}
		current = current->next;
	}
	if (count > 16)
	{
		show_error(ERROR_MAX_HER, NULL, shell, 2);
		return (FAILURE);
	}
	return (SUCCESS);
}

/**
 * Validates and trims whitespace from input.
 * Checks for unclosed quotes and prepares input for lexing.
 *
 * @param shell Pointer to shell structure
 * @return SUCCESS if input is valid, FAILURE otherwise
 */
bool	validate_and_trim_input(t_shell *shell)
{
	char	*line;

	line = ft_strtrim(shell->input_line, TO_TRIM);
	if (line == NULL || line[0] == '\0')
	{
		free(shell->input_line);
		free(line);
		return (FAILURE);
	}
	if (check_quote(line, ft_strlen(line)) != 0)
	{
		free(line);
		show_error(ERROR_QUOTE, NULL, shell, 2);
		return (FAILURE);
	}
	free(shell->input_line);
	shell->input_line = line;
	return (SUCCESS);
}
