/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checkers.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 17:56:25 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/12 15:54:55 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
 *
 *   returns 0 if not inside quotes at position 'index',
 *   or returns the quote character (' or ") if inside a quote.
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

bool	check_syntax(t_shell *shell)
{
	t_token	*current;

	if (shell->token_list == NULL)
		return (SUCCESS);
	current = shell->token_list;
	while (current)
	{
		if (current->type == PIPE)
		{
			if (current == shell->token_list || current->next == NULL)
				return (show_error(ERROR_PIPE, shell, 258), FAILURE);
			if (current->next->type == PIPE)
				return (show_error(ERROR_PIPE, shell, 258), FAILURE);
		}
		if (current->type == IN || current->type == OUT
			|| current->type == APPEND || current->type == HEREDOC)
		{
			if (current->next == NULL || (current->next->type != WORD
					&& current->next->type != HEREDOC_DELIM_QT
					&& current->next->type != HEREDOC_DELIM_UQ))
				return (show_error(ERROR_REDIR, shell, 258), FAILURE);
		}
		current = current->next;
	}
	return (SUCCESS);
}

// maybe add count for heredocs here ?
void	check_heredoc(t_shell *shell)
{
	t_token	*current;
	int		count;
	int		i;

	i = 0;
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
		show_error(ERROR_MAX_HER, shell, 2);
}

void	quote_flag(t_shell *shell)
{
	t_token	*current;
	size_t	i;

	current = shell->token_list;
	while (current)
	{
		if (current->type == WORD)
		{
			i = 0;
			while (current->content[i])
			{
				if (current->content[i] == '\'' || current->content[i] == '"')
				{
					current->quoted = true;
					break ;
				}
				i++;
			}
		}
		current = current->next;
	}
}
