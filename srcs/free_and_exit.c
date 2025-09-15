/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_and_exit.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 17:56:59 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/15 12:18:22 by mklevero         ###   ########.fr       */
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
// free shell shell
void	free_shell_data(t_shell *shell)
{
	if (!shell)
		return ;
	free(shell->input_line);
	shell->input_line = NULL;
	free_list(&shell->token_list);
	free_ast(&shell->ast);
    free_heredoc_files(shell);
}

void	free_shell(t_shell *shell)
{
	if (!shell)
		return ;
	free_shell_data(shell);
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
	// free env here so not to add it in free_shell_data
	clear_history();
	exit(exit_code);
}
void	show_error(char *msg, t_shell *shell, int exit_code)
{
	if (!shell)
		return ;
	if (msg)
		ft_putendl_fd(msg, 2);
	free_shell_data(shell);
	shell->exit_code = exit_code;
	// global exit status = exit_code
}

void	lexer_error(char *input_line, t_shell *shell)
{
	free(input_line);
	clear_history();
	free_list(&shell->token_list);
	ft_putendl_fd(ERROR_MEM, 2);
	shell->exit_code = 1;
	exit(shell->exit_code);
}
