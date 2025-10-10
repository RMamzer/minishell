/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   is_helper.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 14:13:04 by mklevero          #+#    #+#             */
/*   Updated: 2025/10/10 15:06:29 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Helper function to check if a token type is a redirection operator.
 *
 * @param type Token type to check
 * @return true if token is a redirection, false otherwise
 */
bool	is_redir(t_token_type type)
{
	return (type == IN || type == OUT || type == APPEND);
}

/**
 * Checks if a character is a delimiter.
 * Delimiters are whitespace characters: space, newline, or tab.
 *
 * @param i Character to check
 * @return true if character is a delimiter, false otherwise
 */
bool	is_delimiter(int i)
{
	return (i == ' ' || i == '\n' || i == '\t');
}

/**
 * Checks if a character is an operator.
 * Recognized operators: |, >, <.
 *
 * @param c Character to check
 * @return True if operator, false otherwise
 */
bool	is_operator(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

/**
 * Checks if the input line matches the heredoc delimiter.
 * Compares the line with the delimiter and frees the line if they match.
 * This indicates the end of heredoc input.
 *
 * @param line Input line to compare with delimiter
 * @param delim Delimiter token containing the end marker
 * @return true if line matches delimiter, false otherwise
 */
bool	is_delim_written(char *line, t_token *delim)
{
	if (ft_strcmp(line, delim->content) == 0)
	{
		free(line);
		return (true);
	}
	return (false);
}
