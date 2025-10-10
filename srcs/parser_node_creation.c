/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_node_creation.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 14:20:27 by mklevero          #+#    #+#             */
/*   Updated: 2025/10/10 17:10:16 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void	init_arg(t_ast *cmd_node, const char *str, t_shell *shell);
static char	**load_arg(char **old, size_t count, t_shell *shell);

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
static void	init_arg(t_ast *cmd_node, const char *str, t_shell *shell)
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
static char	**load_arg(char **old, size_t count, t_shell *shell)
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
