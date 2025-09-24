/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 14:54:37 by rmamzer           #+#    #+#             */
/*   Updated: 2025/09/24 14:17:33 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// FOR MAXIM BELOW
// cleanup for temp file
// TODO: HEREDOC
// TODO: SIGNALS
// TODO: check push

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

// static void	print_value(char **value)
// {
// 	int	i;

// 	i = 0;
// 	if (!value)
// 		return ;
// 	printf(" (");
// 	while (value[i])
// 	{
// 		if (i > 0)
// 			printf(" ");
// 		printf("%s", value[i]);
// 		i++;
// 	}
// 	printf(")");
// }

// void	print_ast(t_ast *node, int depth)
// {
// 	int	i;

// 	if (!node)
// 	{
// 		printf("ast is clean\n");
// 		return ;
// 	}
// 	/* indentation */
// 	i = 0;
// 	while (i++ < depth)
// 		printf("  ");
// 	printf("%s", type_to_str(node->type));
// 	if (node->value && node->value[0])
// 		print_value(node->value);
// 	printf("\n");
// 	if (node->left)
// 		print_ast(node->left, depth + 1);
// 	if (node->right)
// 		print_ast(node->right, depth + 1);
// }

// int	main(int ac, char **av, char **env)
// {
// 	t_shell	*shell;
// 	char	*input;

// 	(void)ac;
// 	(void)av;
// 	shell = init_data();
// 	create_env(shell, env);
// 	// test_env(shell->env); // test
// 	while (1) // addded for testing
// 	{
// 		input = readline("dirty_shell> ");
// 		if (!input)
// 			break ;
// 		if (input[0] != '\0')
// 			add_history(input);
// 		shell->input_line = input;
// 		printf("%sp\n", shell->input_line);
// 		if (process_input(shell->input_line, shell) != SUCCESS)
// 		{
// 			// free_shell_data(shell);
// 			continue ;
// 		}
// 		if (parse_tokens(shell) != SUCCESS)
// 		{
// 			free_shell_data(shell);
// 			continue ;
// 		}
// 		execute_ast(shell->ast, shell);
// 		free_shell_data(shell);
// 	}
// 	free_shell(shell);
// }

int	main(int ac, char **av, char **env)
{
	t_shell	*shell;

	(void)ac;
	(void)av;
	shell = init_data();
	create_env(shell, env);
	while (1)
	{
		if (receive_input(shell) == FAILURE)
			continue ;
		if (process_input(shell) == FAILURE)
			continue ;
		if (parse_tokens(shell) == FAILURE)
			continue ;
		execute_ast(shell->ast, shell);
		free_shell_data(shell);
	}
	free_shell(shell);
}

bool	receive_input(t_shell *shell)
{
	shell->input_line = readline("dirty_shell> ");
	if (!shell->input_line)
		fatality(NULL, shell, 0); // for now
	if (shell->input_line[0] == '\0')
	{
		free(shell->input_line);
		return (FAILURE);
	}
	if (ft_strlen(shell->input_line) > 1024) // for now
	{
		show_error("input is too long", NULL, shell, 2);
		return (FAILURE);
	}
	add_history(shell->input_line);
	return (SUCCESS);
}

bool	process_input(t_shell *shell)
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
	lexer(shell->input_line, shell);
	if (check_syntax(shell) == FAILURE)
		return (FAILURE);
	process_heredoc(shell);
	expander(shell);
	split_variables(shell);
	delete_empty_tokens(shell);
	// test_tokens(shell->token_list);
	return (SUCCESS);
}

t_shell	*init_data(void)
{
	t_shell *shell;

	shell = malloc(sizeof(t_shell));
	if (shell == NULL)
	{
		ft_putendl_fd(ERROR_MEM, 2);
		exit(FAILURE);
	}
	shell->exit_code = 0; // for now
	shell->input_line = NULL;
	shell->token_list = NULL;
	shell->env = NULL;
	shell->ast = NULL;
	shell->heredoc_files = NULL;
	shell->env_array = NULL;
	shell->paths_array = NULL;
	shell->complete_exit = true;
	return (shell);
}