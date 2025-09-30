/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander_handlers.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 14:52:45 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/30 15:00:12 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

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
