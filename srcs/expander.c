/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 17:28:42 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/30 15:00:17 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*expand_and_remove_quotes(char *content, t_shell *shell,
				t_token *token);
static char	*process_content(char *content, size_t *i, t_shell *shell,
				t_token *token);

/**
 * Main expansion entry point for all WORD tokens.
 * Processes variable expansion, quote removal, and word reconstruction.
 *
 * @param shell Pointer to shell structure containing token list
 */
void	expander(t_shell *shell)
{
	t_token	*current;

	current = shell->token_list;
	while (current)
	{
		if (current->type == WORD)
			current->content = expand_and_remove_quotes(current->content, shell,
					current);
		current = current->next;
	}
}

/**
 * Expands variables and removes quotes from token content.
 * Processes the entire content character by character, handling:
 * - Variable expansion ($VAR, $?)
 * - Quote removal (both single and double quotes)
 * - Preserving literal characters
 *
 * @param content Original token content to expand
 * @param shell Pointer to shell structure for environment access
 * @param token Current token (to mark as expanded)
 * @return Newly allocated expanded and unquoted string
 */
static char	*expand_and_remove_quotes(char *content, t_shell *shell,
		t_token *token)
{
	size_t	i;
	char	*new_content;
	char	*temp;

	i = 0;
	new_content = ft_strdup("");
	if (new_content == NULL)
		lexer_error(content, shell, NULL);
	while (content[i])
	{
		temp = process_content(content, &i, shell, token);
		if (temp == NULL)
			lexer_error(content, shell, new_content);
		new_content = strjoin_free(new_content, temp);
		if (new_content == NULL)
			lexer_error(content, shell, NULL);
	}
	free(content);
	return (new_content);
}

/**
 * Processes a character sequence based on its type.
 * Dispatches to appropriate handler based on the current character:
 * - Single quotes: preserve content literally
 * - Double quotes: allow variable expansion inside
 * - Dollar sign: variable expansion
 * - Regular characters: copy as-is
 *
 * @param content String being processed
 * @param i Pointer to current position (will be updated)
 * @param shell Pointer to shell structure
 * @param token Current token for expansion marking
 * @return Processed string segment
 */
static char	*process_content(char *content, size_t *i, t_shell *shell,
		t_token *token)
{
	if (content[*i] == '\'')
		return (handle_single_quote(content, i));
	else if (content[*i] == '"')
		return (handle_double_quote(content, i, shell, token));
	else if (content[*i] == '$')
		return (handle_dollar(content, i, shell, token));
	else
		return (handle_characters(content, i, NO_QUOTE));
}

/**
 * Expands an environment variable by name.
 * Extracts variable name and looks up its value in the environment.
 *
 * @param content String containing variable name
 * @param i Pointer to current position (at start of variable name)
 * @param env Environment variable list
 * @return Variable value or empty string if not found
 */
char	*expand_env_var(char *content, size_t *i, t_env *env)
{
	size_t	start;
	char	*name;
	char	*value;

	start = *i;
	while (ft_isalnum(content[*i]) || content[*i] == '_')
		(*i)++;
	name = ft_substr(content, start, *i - start);
	if (name == NULL)
		return (NULL);
	value = get_env_value(name, env, ALLOC);
	if (value == NULL)
	{
		free(name);
		return (NULL);
	}
	free(name);
	return (value);
}
/**
 * Retrieves environment variable value by name.
 * Searches the environment list for matching key.
 *
 * @param name Variable name to look up
 * @param env Environment variable list
 * @param alloc Whether to allocate new string or return pointer
 * @return Variable value (allocated or pointer) or empty string
 */
char	*get_env_value(char *name, t_env *env, bool alloc)
{
	t_env	*current;
	char	*value;

	current = env;
	while (current)
	{
		if (ft_strcmp(current->key, name) == 0)
		{
			if (alloc == NO_ALLOC)
				return (current->value);
			if (current->value == NULL)
				return (ft_strdup(""));
			value = ft_strdup(current->value);
			if (value == NULL)
				return (NULL);
			return (value);
		}
		current = current->next;
	}
	if (alloc == NO_ALLOC)
		return (NULL);
	value = ft_strdup("");
	return (value);
}
