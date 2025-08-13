/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 14:54:37 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/13 11:52:27 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// test function, remove later
void	test_tokens(t_token *list)
{
	while (list)
	{
		printf("[TYPE: %d] \"%s\"\n", list->type, list->content);
		list = list->next;
	}
}

int	main(int ac, char **av, char **env)
{
	t_shell	*data;
	char	*input;

	(void)ac;  // addded for testing
	(void)av;  // addded for testing
	(void)env; // addded for testing
	data = init_data();
	while (1) // addded for testing
	{
		input = readline("dirty_shell> ");
		if (!input)
			break ;
		if (input[0] != '\0')
			add_history(input);
		data->input_line = input;
		if (process_input(data->input_line, data) == SUCCESS)
		{
			lexer(data->input_line, data);
			test_tokens(data->token_list); // added for test
			free_list(&data->token_list);  // added for test
		}
	}
}
t_shell	*init_data(void)
{
	t_shell	*data;

	data = malloc(sizeof(t_shell));
	if (data == NULL)
	{
		ft_putendl_fd("malloc: Memory allocation failed", 2);
		exit(FAILURE);
	}
	data->exit_code = -1; // for now
	data->input_line = NULL;
	data->token_list = NULL;
	return (data);
}

/*
 *   Initial processing of an input line.
 *   TO_TRIM chars removed from both ends of the line.
 *   Checks validity of QUOTES
 *   Checks validity of PIPE
 *   Checks validity of REDIRECTIONS
 */
bool	process_input(char *input_line, t_shell *data)
{
	char	*line;

	line = ft_strtrim(input_line, TO_TRIM);
	if (line == NULL || line[0] == '\0')
	{
		free(data->input_line);
		free(line);
		return (FAILURE);
	}
	if (check_quote(line, ft_strlen(line)) != 0)
	{
		free(data->input_line);
		free(line);
		return (show_error("syntax error: unclosed quote", 258), FAILURE);
	}
	if (valid_pipe_usage(line) == FAILURE
		|| valid_redirection_usage(line) == FAILURE)
	{
		free(data->input_line);
		free(line);
		return (FAILURE);
	}
	free(data->input_line);
	data->input_line = line;
	return (SUCCESS);
}

/*
 *   Checks the correctness of pipe ('|') usage.
 *   Returns FAILURE if:
 *   - The line starts with a pipe.
 *   - There are consecutive pipes '||' outside of quotes
 *   - The line ends with a pipe.
 *   The function ignores pipes inside quotes
 *   Otherwse, returns SUCCESS
 */
bool	valid_pipe_usage(char *line)
{
	int	i;

	if (line[0] == '|')
		return (show_error("syntax error near unexpected token '|'", 258),
			FAILURE);
	i = 0;
	while (line[i])
	{
		if (line[i] == '|')
		{
			if (check_quote(line, i) == 0)
			{
				if (line[i + 1] == '|')
					return (show_error("syntax error near unexpected token '||'",
							258), FAILURE);
			}
		}
		i++;
	}
	if (line[i - 1] == '|')
		return (show_error("syntax error near unexpected token '|'", 258),
			FAILURE);
	return (SUCCESS);
}
/*
 *   Checks the correctness of redirection operators ('>' or '<')
 *   Returns FAILURE if:
 *    - operator appears inside invalid syntax(more than 2 consecutive).
 *    - operator is not followed by a valid token
 *    - operator outside of quotes
 *   Otherwise, returns SUCCESS
 */
bool	valid_redirection_usage(char *line)
{
	int		i;
	int		count;
	char	c;

	i = 0;
	while (line[i])
	{
		if ((line[i] == '>' || line[i] == '<') && check_quote(line, i) == 0)
		{
			c = line[i];
			count = 1;
			while (line[i + count] == c)
				count++;
			if (count > 2)
				return (show_error("syntax error near unexpected token", 258),
					FAILURE);
			i += count;
			while (line[i] == ' ' || line[i] == '\t' || line[i] == '\n')
				i++;
			if (line[i] == '|' || line[i] == '<' || line[i] == '>'
				|| line[i] == '\0')
				return (show_error("syntax error near unexpected token", 258),
					FAILURE);
		}
		else
			i++;
	}
	return (SUCCESS);
}

/*
 *
 *   returns 0 if not inside quotes at position 'index',
 *   ore returns the quote character (' or ") if inside a quote.
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

void	show_error(char *msg, int exit_code)
{
	(void)exit_code; // addded for testing
	ft_putendl_fd(msg, 2);
	// global exit status = exit_code
}
void	lexer_error(char *input_line, t_shell *data)
{
	free(input_line);
	clear_history();
	free_list(&data->token_list);
	ft_putendl_fd("malloc: Memory allocation failed", 2);
	data->exit_code = 1;
	exit(data->exit_code);
}
void	free_list(t_token **list)
{
	t_token	*temp;

	if (!list)
		return ;
	while (*list)
	{
		temp = *list;
		*list = (*list)->next;
		free(temp->content);
		temp->content = NULL;
		free(temp);
		temp = NULL;
	}
	*list = NULL;
}

void	lexer(char *input_line, t_shell *data)
{
	size_t	i;

	i = 0;
	while (input_line[i])
	{
		while (is_delimiter(input_line[i]))
			i++;
		if (is_operator(input_line[i]) && check_quote(input_line, i) == 0)
			i += handle_operator(input_line, i, data);
		else
			i += handle_word(input_line, i, data);
	}
}

size_t	handle_word(char *input_line, size_t start, t_shell *data)
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
		lexer_error(input_line, data);
	ft_strlcpy(temp, &input_line[word_start], len + 1);
	add_token(data, WORD, temp);
	free(temp);
	return (len);
}

/*
 *	Processes operator tokens.
 *	Adds the corresponding token to the token list.
 *	Returns the length of the operator token processed (1 or 2).
 */
size_t	handle_operator(char *input_line, size_t i, t_shell *data)
{
	if (input_line[i] == '|')
	{
		add_token(data, PIPE, "|");
		return (1);
	}
	if (input_line[i] == input_line[i + 1])
	{
		if (input_line[i] == '<')
			add_token(data, HEREDOC, "<<");
		else
			add_token(data, APPEND, ">>");
		return (2);
	}
	else
	{
		if (input_line[i] == '<')
			add_token(data, IN, "<");
		else
			add_token(data, OUT, ">");
		return (1);
	}
}

/*
 *	Creates a new token and appends it to the token list.
 *	If token list is NULL, adds token as head node.
 *	if token list is not NULL, adds token as the last one.
 */
void	add_token(t_shell *data, t_token_type type, char *content)
{
	t_token	*token;
	t_token	*temp;

	token = malloc(sizeof(t_token));
	if (token == NULL)
		lexer_error(data->input_line, data);
	token->type = type;
	token->next = NULL;
	token->content = ft_strdup(content);
	if (token->content == NULL)
	{
		free(token);
		lexer_error(data->input_line, data);
	}
	if (data->token_list == NULL)
		data->token_list = token;
	else
	{
		temp = data->token_list;
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