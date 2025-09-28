/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_variables.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 17:26:38 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/28 13:13:05 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Splits expanded variables into multiple tokens based on IFS characters.
 * This function handles word splitting after variable expansion, which is
 * a critical part of shell processing. Only unquoted, expanded variables 
 * are subjected to word splitting.
 * 
 * Example: $MAXIM containing "likes orange juice" becomes three tokens
 * 
 * @param shell Pointer to shell structure containing token list
 */
void	split_variables(t_shell *shell)
{
	t_token	*current;
	char	**split_result;

	current = shell->token_list;
	while (current)
	{
		if (current->type == WORD && !current->quoted && current->expanded)
		{
			split_result = ft_split_IFS(current->content, " \t\n");
			if (!split_result)
				fatality(ERROR_MEM, shell, 1);
			process_split_result(shell, current, split_result);
			free_split(split_result);
		}
		current = current->next;
	}
}
/**
 * Processes the result of variable splitting and updates the token list.
 * Handles three cases:
 * - ZERO_WORDS (0): Replace with empty token
 * - ONE_WORD (1): Replace current token content
 * - Multiple words: Replace current + create additional tokens
 * 
 * @param shell Pointer to shell structure for error handling
 * @param current Token being processed
 * @param split_result Array of strings from splitting operation
 */
void	process_split_result(t_shell *shell, t_token *current,
		char **split_result)
{
	int	str_qty;
	int	i;

	i = 0;
	str_qty = check_qty(split_result);
	if (str_qty == ZERO_WORDS)
		replace_token_content(current, "", shell, split_result);
	else if (str_qty == ONE_WORD)
		replace_token_content(current, split_result[0], shell, split_result);
	else
	{
		replace_token_content(current, split_result[i], shell, split_result);
		i++;
		while (i < str_qty)
		{
			current = add_expanded_token(current, WORD, split_result[i]);
			if (!current)
			{
				free_split(split_result);
				fatality(ERROR_MEM, shell, 1);
			}
			i++;
		}
	}
}
/**
 * Replaces the content of an existing token with new content.
 * Frees the old content and allocates new memory for the replacement.
 * 
 * @param current Token whose content will be replaced
 * @param new_content New string content for the token
 * @param shell Pointer to the shell structure for error handling
 * @param split_result Split array for cleanup on error
 */
void	replace_token_content(t_token *current, char *new_content,
		t_shell *shell, char **split_result)
{
	free(current->content);
	current->content = ft_strdup(new_content);
	if (!current->content)
	{
		free_split(split_result);
		fatality(ERROR_MEM, shell, 1);
	}
}

/**
 * Creates and inserts a new token after the current token.
 * Used when variable splitting results in multiple words.
 * The new token is inserted into the linked list maintaining order.
 * 
 * @param current Token after which to insert new token
 * @param type Token type
 * @param content String content for the new token
 * @return Pointer to the newly created token, or NULL on failure
 */
t_token	*add_expanded_token(t_token *current, t_token_type type, char *content)
{
	t_token	*new_token;

	if (!current || !content)
		return (NULL);
	new_token = malloc(sizeof(t_token));
	if (!new_token)
		return (NULL);
	new_token->expanded = false;
	new_token->quoted = false;
	new_token->type = type;
	new_token->content = ft_strdup(content);
	if (!new_token->content)
	{
		free(new_token);
		return (NULL);
	}
	new_token->next = current->next;
	current->next = new_token;
	return (new_token);
}

/**
 * Frees a null-terminated array of strings.
 * Helper function to clean up memory allocated by splitting operations.
 * 
 * @param split Array of strings to free
 */
void	free_split(char **split)
{
	int	i;

	i = 0;
	if (!split)
		return ;
	while (split[i])
	{
		free(split[i]);
		i++;
	}
	free(split);
}

/**
 * Counts the number of strings in a null-terminated array.
 * Used to determine how many words resulted from variable splitting.
 * 
 * @param split_result Array of strings to count
 * @return Number of strings in the array
 */
int	check_qty(char **split_result)
{
	int	i;

	i = 0;
	if (!split_result)
		return (0);
	while (split_result[i])
		i++;
	return (i);
}
