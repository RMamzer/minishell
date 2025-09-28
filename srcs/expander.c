/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 17:28:42 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/28 17:28:51 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
			current->content = expand_and_remove_quotes(current->content, shell, current);
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
char	*expand_and_remove_quotes(char *content, t_shell *shell, t_token *token)
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
char	*process_content(char *content, size_t *i, t_shell *shell,
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
 * Handles single-quoted strings by preserving content literally.
 * Single quotes prevent all expansion - everything inside is literal.
 * Removes the quote characters themselves from the output.
 * 
 * @param content String being processed
 * @param i Pointer to current position (at opening quote)
 * @return Literal content between quotes
 */
char	*handle_single_quote(char *content, size_t *i)
{
	size_t	start;
	char	*temp;

	(*i)++;
	start = *i;
	while (content[*i] && content[*i] != '\'')
		(*i)++;
	temp = ft_substr(content, start, *i - start);
	if (temp == NULL)
		return (NULL);
	if (content[*i] == '\'')
		(*i)++;
	return (temp);
}

/**
 * Handles double-quoted strings with variable expansion.
 * Double quotes allow variable expansion but prevent word splitting.
 * Removes the quote characters from output while processing content.
 * 
 * @param content String being processed
 * @param i Pointer to current position (at opening quote)
 * @param shell Pointer to shell structure for variable lookup
 * @param token Current token for expansion marking
 * @return Processed content with variables expanded
 */
char	*handle_double_quote(char *content, size_t *i, t_shell *shell,
		t_token *token)
{
	char	*temp;
	char	*result;

	(*i)++;
	result = ft_strdup("");
	if (result == NULL)
		return (NULL);
	while (content[*i] && content[*i] != '"')
	{
		if (content[*i] == '$')
			temp = handle_dollar(content, i, shell, token);
		else
			temp = handle_characters(content, i, IN_DOUBLE_QUOTE);
		if (temp == NULL)
		{
			free(result);
			return (NULL);
		}
		result = strjoin_free(result, temp);
	}
	if (content[*i] == '"')
		(*i)++;
	return (result);
}

/**
 * Handles environment variable expansion starting with '$'.
 * Supports ($VAR), ($?) or literal '$'.
 * Marks token as expanded for later word splitting.
 *  
 * @param content String being processed
 * @param i Pointer to current position (at dollar sign)
 * @param shell Pointer to shell struct
 * @param token Current token to mark as expanded
 * @return Expanded variable value or literal dollar
 */
char	*handle_dollar(char *content, size_t *i, t_shell *shell, t_token *token)
{
	char	*expanded;

	(*i)++;
	if (ft_isalpha(content[*i]) || content[*i] == '_')
	{
		expanded = expand_env_var(content, i, shell->env);
		token->expanded = true;
		return (expanded);
	}
	else if (content[*i] == '?')
	{
		(*i)++;
		expanded = ft_itoa(shell->exit_code);
		token->expanded = true;
		return (expanded);
	}
	else
	{
		expanded = ft_strdup("$");
		return (expanded);
	}
}

/**
 * Handles sequence of literal characters.
 * Copies characters until a special character is encountered.
 * Stops at '$', quotes, or end of the string.
 * Behavior changes based on quote context.
 * 
 * @param content String being processed
 * @param i Pointer to current position
 * @param in_dq Whether we're inside double quotes
 * @return String containing literal characters
 */
char	*handle_characters(char *content, size_t *i, bool in_dq)
{
	size_t	start;
	char	*chars;

	start = *i;
	if (in_dq)
	{
		while (content[*i] && content[*i] != '$' && content[*i] != '"')
			(*i)++;
	}
	else
	{
		while (content[*i] && content[*i] != '$' && content[*i] != '\''
			&& content[*i] != '"')
			(*i)++;
	}
	chars = ft_substr(content, start, *i - start);
	if (chars == NULL)
		return (NULL);
	return (chars);
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

/**
 * Compares two strings for equality.
 * 
 * @param s1 First string
 * @param s2 Second string
 * @return 0 if equal, positive/negative difference otherwise
 */
int	ft_strcmp(const char *s1, const char *s2)
{
	size_t	i;

	i = 0;
	while (s1[i] != '\0' || s2[i] != '\0')
	{
		if ((unsigned char)s1[i] != (unsigned char)s2[i])
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	return (0);
}
