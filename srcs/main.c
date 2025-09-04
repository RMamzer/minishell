/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 14:54:37 by rmamzer           #+#    #+#             */
/*   Updated: 2025/09/04 19:42:36 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// FOR MAXIM BELOW
// TODO: test var expansion. // echo $"PATH"
//	->PATH in bash vs dirty_shell> $"PATH"
//[TYPE: 3] "$PATH"
// TODO: new branch for stuff below.
// TODO: plan for heredoc(probably should be handeled before the expansion)
// TODO: remove quotes.
// TODO: AST situation.
// TODO: ERROR MANAGEMENT

// test function, remove later
void	test_tokens(t_token *list)
{
	while (list)
	{
		printf("[TYPE: %d] \"%s\"\n", list->type, list->content);
		list = list->next;
	}
}

// void	test_env(t_env *envlist)
// {
// 	while (envlist)
// 	{
// 		// printf("KEY: %s VALUE:%s \n", envlist->key, envlist->value);
// 		printf("%s=%s\n", envlist->key, envlist->value);
// 		envlist = envlist->next;
// 	}
// }
// remove
const char	*type_to_str(t_token_type t)
{
	switch (t)
	{
	case WORD:
		return ("WORD");
	case IN:
		return ("IN");
	case OUT:
		return ("OUT");
	case APPEND:
		return ("APPEND");
	case HEREDOC:
		return ("HEREDOC");
	case PIPE:
		return ("PIPE");
	default:
		return ("UNKNOWN");
	}
}

void	print_ast(t_ast *node, int depth)
{
	if (!node)
	{
		printf("ast is clean\n");
		return ;
	}
	// indentation for readability
	for (int i = 0; i < depth; i++)
		printf("  ");
	printf("%s", type_to_str(node->type));
	if (node->value && node->value[0])
		printf(" (%s)", node->value[0]); // show filename or command name
	printf("\n");
	if (node->left)
		print_ast(node->left, depth + 1);
	if (node->right)
		print_ast(node->right, depth + 1);
}

int	main(int ac, char **av, char **env)
{
	t_shell	*shell;
	char	*input;

	(void)ac; // addded for testing
	(void)av; // addded for testing
	shell = init_data();
	create_env(shell, env);
	// test_env(shell->env); // test
	while (1) // addded for testing
	{
		input = readline("dirty_shell> ");
		if (!input)
			break ;
		if (input[0] != '\0')
			add_history(input);
		shell->input_line = input;
		if (process_input(shell->input_line, shell) == SUCCESS)
		{
			test_tokens(shell->token_list);
		}
		if (parse_tokens(shell) == SUCCESS)
		{
			print_ast(shell->ast, 0);
			execute_ast(shell->ast, shell);
			free_ast(&shell->ast);
			print_ast(shell->ast, 0);
			shell->ast = NULL;
		}
	}
}

bool	validate_redirection(t_token *redirection)
{
	t_token	*token;

	token = redirection->next;
	if (!redirection || !redirection->next)
	{
		ft_putendl_fd("minishell: syntax error near unexpected token `newline'",
			2);
		return (false);
	}
	if (token->type != WORD)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
		ft_putstr_fd(token->content, 2);
		ft_putendl_fd("'", 2);
		return (false);
	}
	if (token->expanded && token->content[0] == '\0')
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(token->content, 2);
		ft_putendl_fd(": ambiguous redirection", 2);
		return (false);
	}
	if (token->expanded && token->next && token->next->type == WORD)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(token->content, 2);
		ft_putendl_fd(": ambiguous redirection", 2);
		return (false);
	}
	return (true);
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

t_shell	*init_data(void)
{
	t_shell	*shell;

	shell = malloc(sizeof(t_shell));
	if (shell == NULL)
	{
		ft_putendl_fd("malloc: Memory allocation failed", 2);
		exit(FAILURE);
	}
	shell->exit_code = -1; // for now
	shell->input_line = NULL;
	shell->token_list = NULL;
	shell->env = NULL;
	shell->ast = NULL;
	return (shell);
}

/*
 *   Initial processing of an input line.
 *   TO_TRIM chars removed from both ends of the line.
 *   Checks validity of QUOTES
 *   Checks validity of PIPE
 *   Checks validity of REDIRECTIONS
 */
bool	process_input(char *input_line, t_shell *shell)
{
	char	*line;

	line = ft_strtrim(input_line, TO_TRIM);
	if (line == NULL || line[0] == '\0')
	{
		free(shell->input_line);
		free(line);
		return (FAILURE);
	}
	if (check_quote(line, ft_strlen(line)) != 0)
	{
		free(shell->input_line);
		free(line);
		return (show_error("syntax error: unclosed quote", 258), FAILURE);
	}
	free(shell->input_line);
	shell->input_line = line;
	lexer(shell->input_line, shell);
	quote_flag(shell);
	check_heredoc(shell);
	if (check_syntax(shell) == FAILURE)
	{
		free_list(&shell->token_list);
		return (FAILURE);
	}
	expander(shell); // test
	split_variables(shell);
	return (SUCCESS);
}

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

void	show_error(char *msg, int exit_code)
{
	(void)exit_code; // addded for testing
	ft_putendl_fd(msg, 2);
	// global exit status = exit_code
}
void	lexer_error(char *input_line, t_shell *shell)
{
	free(input_line);
	clear_history();
	free_list(&shell->token_list);
	ft_putendl_fd("malloc: Memory allocation failed", 2);
	shell->exit_code = 1;
	exit(shell->exit_code);
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
				return (show_error("syntax error near unexpected token '|'",
						258), FAILURE);
			if (current->next->type == PIPE)
				return (show_error("syntax error near unexpected token '|'",
						258), FAILURE);
		}
		if (current->type == IN || current->type == OUT
			|| current->type == APPEND || current->type == HEREDOC)
		{
			if (current->next == NULL || (current->next->type != WORD
					&& current->next->type != HEREDOC_DELIM_QT
					&& current->next->type != HEREDOC_DELIM_UQ))
				return (show_error("syntax error near unexpected token", 258),
					FAILURE);
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

	count = 0;
	current = shell->token_list;
	while (current)
	{
		if (current->type == HEREDOC && current->next)
		{
			count++;
			if (current->next->content[0] == '\''
				|| current->next->content[0] == '"')
				current->next->type = HEREDOC_DELIM_QT;
			else
				current->next->type = HEREDOC_DELIM_UQ;
		}
		current = current->next;
	}
	if (count > 16)
		lexer_error("heredoc max count", shell);
}
