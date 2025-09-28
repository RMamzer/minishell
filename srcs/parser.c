/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 18:05:25 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/28 12:44:47 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


/**
 * Validates redirection tokens to detect ambiguous redirection.
 * An ambiguous redirect occurs when an expanded variable results is 
 * multiple words or none, where only one filename is expected.
 * 
 * @param redirection The redirection token to validate
 * @return true if redirection is valid, false if ambiguous
 */
bool	validate_redirection(t_token *redirection)
{
	t_token	*token;

	if (!redirection || !redirection->next)
		return (false);
	token = redirection->next;
	if (token->expanded && token->next && token->next->type == WORD)
		return (false);
	return (true);
}


/**
 * Confirms the syntax validity of the token list before parsing.
 * Check for ambiguous redirections that could cause parsing errors.
 * 
 * @param token_list The list of tokens to validate
 * @param shell Pointer to shell structure for error reporting
 * @return SUCCESS if syntax is valid, FAILURE otherwise
 */
bool	syntax_confirmed(t_token *token_list, t_shell *shell)
{
	t_token	*current;

	current = token_list;
	while (current)
	{
		if (is_redir(current->type) && !validate_redirection(current))
		{
			show_error("minishell: ambiguous redirect", NULL, shell, 2);
			return (FAILURE);
		}
		current = current->next;
	}
	return (SUCCESS);
}


/**
 * Main parsing entry point. Validates syntax and builds the AST.
 * 
 * @param shell Pointer to the shell structure containing token list
 * @return SUCCESS if parsing succeeds, FAILURE otherwise
 */
bool	parse_tokens(t_shell *shell)
{
	if (!shell || !shell->token_list)
		return (FAILURE);
	if (syntax_confirmed(shell->token_list, shell) == FAILURE)
		return (FAILURE);
	shell->ast = parse_pipe(&shell->token_list, shell);
	return (SUCCESS);
}
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
 * Helper function to check if a token type is a redirection operator.
 * 
 * @param type Token type to check
 * @return true if token is a redirection, false otherwise
 */
bool	is_redir(t_token_type type)
{
	return (type == IN || type == OUT || type == APPEND);
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
t_ast	*parse_command_and_redirection(t_token **token_list, t_shell *shell)
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
t_ast	*handle_word_ast(t_token **token_list, t_ast *cmd_node, t_shell *shell)
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
t_ast	*handle_redir_ast(t_token **token_list, t_ast **root, t_ast **tail,
		t_shell *shell)
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
 * Appends an argument to a command AST node's argument list.
 * Dynamically resizes the argument array to accommodate new arguments.
 * 
 * @param cmd_node Command AST node to modify
 * @param str Argument string to add
 * @param shell Pointer to shell structure for error handling
 */
void	append_arg(t_ast *cmd_node, const char *str, t_shell *shell)
{
	char	**new;
	size_t	i;

	i = 0;
	if (!cmd_node->value)
		return (init_arg(cmd_node, str, shell));
	while (cmd_node->value[i])
		i++;
	new = load_arg(cmd_node->value, i, shell);
	new[i] = ft_strdup(str);
	if (!new[i])
		fatality(ERROR_MEM, shell, 1);
	new[i + 1] = NULL;
	cmd_node->value = new;
}

/**
 * Initializes the argument array for a command AST node.
 * Called when adding the first argument (usually the command name).
 * 
 * @param cmd_node Command AST node to initialize
 * @param str First argument string
 * @param shell Pointer to shell structure for error handling
 */
void	init_arg(t_ast *cmd_node, const char *str, t_shell *shell)
{
	cmd_node->value = ft_calloc(2, sizeof(char *));
	if (!cmd_node->value)
		fatality(ERROR_MEM, shell, 1);
	cmd_node->value[0] = ft_strdup(str);
	if (!cmd_node->value[0])
		fatality(ERROR_MEM, shell, 1);
	cmd_node->value[1] = NULL;
}

/**
 * Reallocates and copies arguments array to accommodate additional arguments.
 * Increases array size by one slot plus NULL terminator.
 * 
 * @param old Existing argument array
 * @param count Current number of arguments
 * @param shell Pointer to shell structure for error handling
 * @return New argument array with additional space
 */
char	**load_arg(char **old, size_t count, t_shell *shell)
{
	char	**new;
	size_t	i;

	i = 0;
	new = ft_calloc(count + 2, sizeof(char *));
	if (!new)
		fatality(ERROR_MEM, shell, 1);
	while (i < count)
	{
		new[i] = old[i];
		i++;
	}
	free(old);
	return (new);
}
/**
 * Creates an AST node for a filename in redirection operations.
 * Converts a token into an AST node and cleans up the token.
 * 
 * @param token Token containing the filename
 * @param shell Pointer to shell structure for error handling
 * @return AST node representing the file
 */
t_ast	*add_file_node(t_token *token, t_shell *shell)
{
	t_ast	*file_node;

	file_node = add_ast_node(token->type, shell);
	file_node->value = ft_calloc(2, sizeof(char *));
	if (file_node == NULL)
		fatality(ERROR_MEM, shell, 1);
	file_node->value[0] = ft_strdup(token->content);
	if (file_node->value[0] == NULL)
		fatality(ERROR_MEM, shell, 1);
	free(token->content);
	free(token);
	return (file_node);
}


/**
 * Creates a new AST node with the specified type.
 * Initializes all fields to default values.
 * 
 * @param type Type of AST node to create
 * @param shell Pointer to shell structure for error handling
 * @return Newly allocated and initialized AST node
 */
t_ast	*add_ast_node(t_token_type type, t_shell *shell)
{
	t_ast	*node;

	node = ft_calloc(1, sizeof(t_ast));
	if (!node)
		fatality(ERROR_MEM, shell, 1);
	node->type = type;
	node->fd[0] = -1;
	node->fd[1] = -1;
	return (node);
}
/**
 * Advances token list pointer and frees the consumed token.
 * Helper function to clean up tokens after processing.
 * 
 * @param Pointer to current position in token list
 */
void	move_and_free(t_token **token_list)
{
	t_token	*temp;

	if (!token_list || !*token_list)
		return ;
	temp = *token_list;
	*token_list = (*token_list)->next;
	free(temp->content);
	free(temp);
}
