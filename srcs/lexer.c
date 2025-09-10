/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 17:29:11 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/10 18:52:24 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	lexer(char *input_line, t_shell *shell)
{
	size_t	i;

	i = 0;
	while (input_line[i])
	{
		while (is_delimiter(input_line[i]))
			i++;
		if (is_operator(input_line[i]) && check_quote(input_line, i) == 0)
			i += handle_operator(input_line, i, shell);
		else
			i += handle_word(input_line, i, shell);
	}
}

size_t	handle_word(char *input_line, size_t start, t_shell *shell)
{
	size_t	i;
	size_t	word_start;
	size_t	len;
	char	*temp;

	i = start;
	word_start = start;
	while (input_line[i])
	{
		if (check_quote(input_line, i) == 0)
		{
			if (is_delimiter(input_line[i]) || is_operator(input_line[i]))
				break ;
		}
		i++;
	}
	len = i - word_start;
	temp = malloc(len + 1);
	if (temp == NULL)
		lexer_error(input_line, shell);
	ft_strlcpy(temp, &input_line[word_start], len + 1);
	add_token(shell, WORD, temp);
	free(temp);
	return (len);
}

/*
 *	Processes operator tokens.
 *	Adds the corresponding token to the token list.
 *	Returns the length of the operator token processed (1 or 2).
 */
size_t	handle_operator(char *input_line, size_t i, t_shell *shell)
{
	if (input_line[i] == '|')
	{
		add_token(shell, PIPE, "|");
		return (1);
	}
	if (input_line[i] == input_line[i + 1])
	{
		if (input_line[i] == '<')
			add_token(shell, HEREDOC, "<<");
		else
			add_token(shell, APPEND, ">>");
		return (2);
	}
	else
	{
		if (input_line[i] == '<')
			add_token(shell, IN, "<");
		else
			add_token(shell, OUT, ">");
		return (1);
	}
}

/*
 *	Creates a new token and appends it to the token list.
 *	If token list is NULL, adds token as head node.
 *	if token list is not NULL, adds token as the last one.
 */
void	add_token(t_shell *shell, t_token_type type, char *content)
{
	t_token	*token;
	t_token	*temp;

	token = malloc(sizeof(t_token));
	if (token == NULL)
		lexer_error(shell->input_line, shell);
	token->type = type;
	token->expanded = false;
	token->quoted = false;
	token->next = NULL;
	token->content = ft_strdup(content);
	if (token->content == NULL)
	{
		free(token);
		lexer_error(shell->input_line, shell);
	}
	if (shell->token_list == NULL)
		shell->token_list = token;
	else
	{
		temp = shell->token_list;
		while (temp->next)
			temp = temp->next;
		temp->next = token;
	}
}

bool	is_delimiter(int i)
{
	if (i == ' ' || i == '\n' || i == '\t')
		return (true);
	return (false);
}

bool	is_operator(char c)
{
	if (c == '|' || c == '<' || c == '>')
		return (true);
	return (false);
}
