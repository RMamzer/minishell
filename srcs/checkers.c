/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   checkers.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 17:56:25 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/19 17:42:29 by mklevero         ###   ########.fr       */
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
		if (current->type == PIPE && check_pipe_syntax(current,
				shell) == FAILURE)
			return (FAILURE);
		if ((current->type == IN || current->type == OUT
				|| current->type == APPEND || current->type == HEREDOC)
			&& check_redir_syntax(current, shell) == FAILURE)
			return (FAILURE);
		current = current->next;
	}
	return (SUCCESS);
}

bool	check_pipe_syntax(t_token *current, t_shell *shell)
{
	if (current == shell->token_list)
		return (show_error(NULL, current, shell, 2), FAILURE);
	if (current->next == NULL)
		return (show_error(NULL, NULL, shell, 2), FAILURE);
	if (current->next->type == PIPE)
		return (show_error(NULL, current->next, shell, 2), FAILURE);
	return (SUCCESS);
}

bool	check_redir_syntax(t_token *current, t_shell *shell)
{
	if (current->next == NULL)
		return (show_error(NULL, NULL, shell, 2), FAILURE);
	if (current->next->type != WORD && current->next->type != HEREDOC_DELIM_QT
		&& current->next->type != HEREDOC_DELIM_UQ)
		return (show_error(NULL, current->next, shell, 2), FAILURE);
	return (SUCCESS);
}

void	check_heredoc(t_shell *shell)
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
		show_error(ERROR_MAX_HER, NULL, shell, 2);
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
