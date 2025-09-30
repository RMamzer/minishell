/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 18:05:25 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/30 17:37:08 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static t_ast	*parse_command_and_redirection(t_token **token_list,
					t_shell *shell);
static t_ast	*handle_word_ast(t_token **token_list, t_ast *cmd_node,
					t_shell *shell);
static t_ast	*handle_redir_ast(t_token **token_list, t_ast **root,
					t_ast **tail, t_shell *shell);
static void		move_and_free(t_token **token_list);

/**
 * Parses pipe operations with right associativity.
 * Pipes have the lowest precedence and are parsed recursively.
 * Grammar: cmd | cmd | cmd...
 *
 * @param token_list POinter to current position in token list
 * @param shell Pointer to shell structure for memory management
 * @return AST node representing the pipe operation or single command
 */
t_ast	*parse_pipe(t_token **token_list, t_shell *shell)
{
	t_ast	*node;
	t_ast	*pipe_node;
	t_token	*pipe_token;

	node = parse_command_and_redirection(token_list, shell);
	if (*token_list && (*token_list)->type == PIPE)
	{
		pipe_node = add_ast_node(PIPE, shell);
		pipe_token = *token_list;
		*token_list = (*token_list)->next;
		free(pipe_token->content);
		free(pipe_token);
		pipe_node->left = node;
		pipe_node->right = parse_pipe(token_list, shell);
		return (pipe_node);
	}
	return (node);
}

/**
 * Parses commands and their associated redirections.
 * Handles the precedence where redirections can appear before, after,
 * or interspersed with command arguments.
 * Grammar: [redir] [word] [redir] [word]...
 *
 * @param token_list Pointer to current position in token list
 * @param shell Pointer to shell structure for memory management
 * @return AST node representing command with redirections
 */
static t_ast	*parse_command_and_redirection(t_token **token_list,
		t_shell *shell)
{
	t_ast	*root;
	t_ast	*tail;
	t_ast	*cmd_node;

	root = NULL;
	tail = NULL;
	cmd_node = NULL;
	while (*token_list && (is_redir((*token_list)->type)
			|| (*token_list)->type == WORD))
	{
		if ((*token_list)->type == WORD)
			cmd_node = handle_word_ast(token_list, cmd_node, shell);
		else
			handle_redir_ast(token_list, &root, &tail, shell);
	}
	if (tail)
	{
		tail->left = cmd_node;
		return (root);
	}
	return (cmd_node);
}

/**
 * Handles WORD tokens by building or extending a command AST node.
 * First WORD becomes the command, subsequent WORDs become arguments.
 *
 * @param token_list Pointer to current position in token list
 * @param cmd_node Existing command or NULL
 * @param shell Pointer to shell structure for memory management
 * @return Command AST node with updated arguments
 */
static t_ast	*handle_word_ast(t_token **token_list, t_ast *cmd_node,
		t_shell *shell)
{
	if (!cmd_node)
		cmd_node = add_ast_node((*token_list)->type, shell);
	append_arg(cmd_node, (*token_list)->content, shell);
	move_and_free(token_list);
	return (cmd_node);
}

/**
 * Handles redirection tokens by building a redirection AST node.
 * Creates a chain of redirections where each redirection's left child
 * points to the next redirection or final command.
 *
 * @param token_list Pointer to current position in token list
 * @param root Pointer to root of redirection chain
 * @param tail Pointer to last redirection in chain
 * @param shell Pointer to shell structure for memory management
 * @return The new redirection AST
 */
static t_ast	*handle_redir_ast(t_token **token_list, t_ast **root,
		t_ast **tail, t_shell *shell)
{
	t_token	*redir_token;
	t_token	*file_token;
	t_ast	*node;
	t_token	*next;

	redir_token = *token_list;
	file_token = redir_token->next;
	next = file_token->next;
	node = add_ast_node(redir_token->type, shell);
	node->right = add_file_node(file_token, shell);
	*token_list = next;
	free(redir_token->content);
	free(redir_token);
	if (!*root)
	{
		*root = node;
		*tail = node;
	}
	else
	{
		(*tail)->left = node;
		*tail = node;
	}
	return (node);
}

/**
 * Advances token list pointer and frees the consumed token.
 * Helper function to clean up tokens after processing.
 *
 * @param token_list Pointer to current position in token list
 */
static void	move_and_free(t_token **token_list)
{
	t_token	*temp;

	if (!token_list || !*token_list)
		return ;
	temp = *token_list;
	*token_list = (*token_list)->next;
	free(temp->content);
	free(temp);
}
