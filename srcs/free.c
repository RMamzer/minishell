/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 17:21:04 by mklevero          #+#    #+#             */
/*   Updated: 2025/10/13 16:19:38 by mklevero         ###   ########.fr       */
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
 * Joins two strings and frees both input strings.
 *
 * @param new_content First string (will be freed)
 * @param temp Second string (will be freed)
 * @return Newly allocated joined string
 */
char	*strjoin_free(char *new_content, char *temp)
{
	char	*result;

	result = ft_strjoin(new_content, temp);
	free(new_content);
	free(temp);
	if (result == NULL)
		return (NULL);
	return (result);
}
