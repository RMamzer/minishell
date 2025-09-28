/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free_and_exit.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/08 17:56:59 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/28 19:02:21 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Frees a linked list of tokens.
 * Traverses the token list and frees each token's content and structure.
 * Sets the list pointer to NULL.
 * 
 * @param list Pointer to the token list head pointer
 */
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

/**
 * Recursively frees an Abstract Syntax Tree (AST).
 * Frees the argument array and the node structure itself.
 * 
 * @param ast Pointer to the AST root
 */
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

/**
 * Frees the environment variable linked list.
 * Traverses the list and frees each environment node.
 * 
 * @param env Head of the environment variable list
 */
void	free_env(t_env *env)
{
	t_env	*temp;
	t_env	*current;

	if (!env)
		return ;
	temp = env;
	while (temp)
	{
		current = temp;
		temp = temp->next;
		free_env_node(current);
	}
}

/**
 * Frees shell data structures for the current command cycle.
 * Cleans up input line, token list, AST, and execution arrays.
 * Prepares shell for the next command without freeing persistent data.
 * 
 * @param shell Pointer to shell structure
 */
void	free_shell_data(t_shell *shell)
{
	if (!shell)
		return ;
	free(shell->input_line);
	shell->input_line = NULL;
	free_list(&shell->token_list);
	free_ast(&shell->ast);
	//free_heredoc_files(shell);
	shell->complete_exit = true;
	free_array(shell->env_array);
	shell->env_array = NULL;
	free_array(shell->paths_array);
	shell->paths_array = NULL;
}

/**
 * Completely frees the shell structure and all associated data.
 * Called when exiting the minishell entirely.
 * Frees both per-command data and persistent shell data.
 * 
 * @param shell Pointer to shell structure
 */
void	free_shell(t_shell *shell)
{
	if (!shell)
		return ;
	free_shell_data(shell);
	if (shell->env)
		free_env(shell->env);
	free(shell);
}

/**
 * Frees a null-terminated array of strings.
 * Generic function for freeing arrays like arguments, environment, paths.
 * 
 * @param arr Array of strings to free
 */
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

/**
 * Fatal error handler that cleans up and exits the program.
 * Used for unrecoverable errors like memory allocation failures.
 * Performs complete cleanup before terminating.
 * 
 * @param msg Error message to display (optional)
 * @param shell Pointer to shell structure to clean up
 * @param exit_code Exit code for the program
 */
void	fatality(char *msg, t_shell *shell, int exit_code)
{
	if (msg)
		ft_putendl_fd(msg, 2);
	if (shell)
		free_shell(shell);
	clear_history();
	exit(exit_code);
}

/**
 * Displays syntax error messages and updates shell state.
 * Handles both custom error messages and token-specific errors.
 * Does not exit the program, allowing minishell to continue.
 * 
 * @param msg Custom error message (optional)
 * @param wrong_token Token causing the syntax error (optional)
 * @param shell Pointer to shell structure
 * @param exit_code Exit code to set in minishell
 */
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
}

/**
 * Handles lexer-specific errors with appropriate cleanup.
 * Used for memory allocation failures during tokenization.
 * Performs specific cleanup for lexer state before exiting.
 * 
 * @param input_line Input line being processed (to free)
 * @param shell Pointer to shell structure
 * @param temp_cont Temporary content to free (optional)
 */
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
