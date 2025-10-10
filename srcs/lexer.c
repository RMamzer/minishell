/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lexer.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 17:29:11 by mklevero          #+#    #+#             */
/*   Updated: 2025/10/10 15:06:41 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static size_t	handle_word(char *input_line, size_t start, t_shell *shell);
static size_t	handle_operator(char *input_line, size_t i, t_shell *shell);
static void		add_token(t_shell *shell, t_token_type type, char *content);
static void		quote_flag(t_shell *shell);

/**
 * Main lexical analyzer entry point.
 * Tokenizes an input line into linked list of tokens.
 * Skips delimiters, identifies operators (|, >, <, >>, <<)
 * and groups words while respecting quotes.
 *
 * @param input_line The raw command line string
 * @param shell Pointer to shell structure for token storage
 */
void	lexer(char *input_line, t_shell *shell)
{
	size_t	i;

	i = 0;
	while (input_line[i])
	{
		while (input_line[i] && is_delimiter(input_line[i]))
			i++;
		if (input_line[i] && is_operator(input_line[i])
			&& check_quote(input_line, i) == 0)
			i += handle_operator(input_line, i, shell);
		else if (input_line[i])
			i += handle_word(input_line, i, shell);
	}
	quote_flag(shell);
}

/**
 * Extracts a word token from the input.
 * A word is a sequence of characters that is not a delimiter
 * or operator, unless enclosed in quotes.
 *
 * @param input_line Input string being tokenized
 * @param start Starting position of the word
 * @param shell Pointer to the shell structure for token creation
 * @return Length of the processed word
 */
static size_t	handle_word(char *input_line, size_t start, t_shell *shell)
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
		lexer_error(input_line, shell, temp);
	ft_strlcpy(temp, &input_line[word_start], len + 1);
	add_token(shell, WORD, temp);
	free(temp);
	return (len);
}

/**
 *	Processes operator tokens (|, >, <, >>, <<)
 *	Adds the corresponding token to the token list.
 *	Returns the length of the operator token processed (1 or 2).
 *
 *	@param input_line Input string being tokenized
 *	@param i Current position in input string
 *	@param shell Pointer to the shell structure for token creation
 *	@return Length of operator processed (1 or 2).
 */
static size_t	handle_operator(char *input_line, size_t i, t_shell *shell)
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

/**
 * Creates and appends a new token to the token list.
 * If the list is empty, the new token becomes the head.
 * Otherwise, it is appended at the end.
 *
 * @param shell Pointer to the shell structure containing token list
 * @param type Type of token being created (WORD, PIPE, IN, OUT, etc.)
 * @param content String content of the token
 */
static void	add_token(t_shell *shell, t_token_type type, char *content)
{
	t_token	*token;
	t_token	*temp;

	token = malloc(sizeof(t_token));
	if (token == NULL)
		lexer_error(shell->input_line, shell, content);
	token->type = type;
	token->expanded = false;
	token->quoted = false;
	token->next = NULL;
	token->content = ft_strdup(content);
	if (!token->content)
	{
		free(token);
		lexer_error(shell->input_line, shell, content);
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

/**
 * Sets the 'quoted' flag for WORD tokens containing quotes.
 * This flag is used later during expansion to determine
 * whether word splitting should occur.
 *
 * @param shell Pointer to shell structure containing token list
 */
static void	quote_flag(t_shell *shell)
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
