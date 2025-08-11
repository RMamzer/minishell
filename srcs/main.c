/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 14:54:37 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/11 12:03:25 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
            lexer(data);
        }
	}
}
t_shell	*init_data(void)
{
	t_shell	*data;

	data = malloc(sizeof(t_shell));
	if (data == NULL)
		return (NULL);    // for now null need exit
	data->exit_code = -1; // for now
	data->input_line = NULL;
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
 *   Skips escaped characters and
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
		if (line[i] == '\\' && quote != '\'' && (i + 1) < index)
		{
			i += 2;
			continue ;
		}
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



void	lexer(t_shell *data)
{
// loop through the input
// find smth
// extract
// create node, add type, add to list
}

