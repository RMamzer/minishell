/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_and_exit.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 17:56:59 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/18 16:17:53 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// free token list
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
// free ast
void	free_ast(t_ast **ast)
{
	t_ast	*temp;

	if (!ast || !*ast)
		return ;
	temp = *ast;
	free_ast(&temp->left);
	free_ast(&temp->right);
	free_array(temp->value);
	free(temp);
	*ast = NULL;
}
void	free_env(t_env *env)
{
	t_env *temp;
	t_env *current;

	if (!env)
		return;
	temp = env;
	while (temp)
	{
		current = temp;
		temp = temp->next;
		free_env_node(current);
	}
}

// free shell shell
void	free_shell_data(t_shell *shell)
{
	if (!shell)
		return ;
	free(shell->input_line);
	shell->input_line = NULL;
	free_list(&shell->token_list);
	free_ast(&shell->ast);
	shell->complete_exit = true;
	// probably free_env
}

void	free_shell(t_shell *shell)
{
	if (!shell)
		return ;
	free_shell_data(shell);
	if (shell->env)
		free_env(shell->env);
	free(shell);
}

// free array of strings
void	free_array(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return ;
	while (arr[i])
		free(arr[i++]);
	free(arr);
}

void	fatality(char *msg, t_shell *shell, int exit_code)
{
	if (msg)
		ft_putendl_fd(msg, 2);
	if (shell)
		free_shell(shell);
	clear_history();
	exit(exit_code);
}
void	show_error(char *msg, t_token *wrong_token, t_shell *shell,
		int exit_code)
{
	if (!shell)
		return ;
	if (msg)
		ft_putendl_fd(msg, 2);
	else
	{
		ft_putstr_fd("syntax error near unexpected token `", 2);
		if (wrong_token == NULL)
			ft_putstr_fd("newline", 2);
		else if (wrong_token->content)
			ft_putstr_fd(wrong_token->content, 2);
		ft_putendl_fd("'", 2);
	}
	free_shell_data(shell);
	shell->exit_code = exit_code;
	// global exit status = exit_code
}

void	lexer_error(char *input_line, t_shell *shell, char *temp_cont)
{
	if (temp_cont)
		free(temp_cont);
	free(input_line);
	free_list(&shell->token_list);
	free_heredoc_files(shell);
	clear_history();
	ft_putendl_fd(ERROR_MEM, 2);
	shell->exit_code = 1;
	exit(shell->exit_code);
}
