/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expander.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 17:28:42 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/03 12:13:18 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 *  Expands all environment variables in the shell's token list.
 *  @param shell Pointer to the shell struct containing tokens.
 */
void	expander(t_shell *shell)
{
	t_token	*current;

	current = shell->token_list;
	while (current)
	{
		if (current->type == WORD) //&& ft_strchr(current->content, '$')
			current->content = expand_content(current->content, shell, current);
		current = current->next;
	}
}

/**
 * Expands environment variables, the $? variable,
 * and handels quotes in the string.
 * @param content The string to expand.
 * @param shell Pointer to the shell struct(conteins env and exit code).
 * @return Newly allocated expanded string.
 */
char	*expand_content(char *content, t_shell *shell, t_token *token)
{
	size_t	i;
	char	*new_content;
	char	*temp;

	i = 0;
	new_content = ft_strdup("");
	if (new_content == NULL)
		lexer_error(content, shell);
	while (content[i])
	{
		temp = process_content(content, &i, shell, token);
		if (temp == NULL)
		{
			free(new_content);
			lexer_error(content, shell);
		}
		new_content = strjoin_free(new_content, temp);
		if (new_content == NULL)
			lexer_error(content, shell);
	}
	free(content);
	return (new_content);
}

/**
 * Determens wether to handle a dollar variable or normal characters.
 * @param content The string being processed.
 * @param i Pointer to the current index in the string.
 * @param shell Pointer to the shell struct.
 * @return Newly allocated string for the processed part.
 */
char	*process_content(char *content, size_t *i, t_shell *shell, t_token *token)
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
char	*handle_double_quote(char *content, size_t *i, t_shell *shell, t_token *token)
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
 * Expands a variable starting with '$'.
 * @param content The string being processed.
 * @param i Pointer to the index after '$' in the string.
 * @param shell Pointer to the shell struct(conteins env and exit code).
 * @return Newly allocated string for the processed part.
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
		// or functon which updates exitcode idk yet
		expanded = ft_itoa(shell->exit_code);
        token->expanded = true; // not sure yet
		return (expanded);
	}
	else
	{
		expanded = ft_strdup("$");
		return (expanded);
	}
}

/**
 * Reads consecutive non-variable characters.
 * @param content The string being processed.
 * @param i Pointer to the current index (updated to the end of read chars.)
 * @return Newly allocated string containing the characters, or NULL.
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
 * Extracts a variable name and returns its value from the env.
 * @param content The string being processed.
 * @param i Pointer to the current index (updated to end of variable name).
 * @param env Pointer to th evironment linked list.
 * @return Newly allocated string with variabl's value, or NULL.
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
	if (value == NULL)
		return (NULL);
	return (value);
}
/**
 * Joins two strings and frees the originals.
 * @param new_content First string to join (freed inside).
 * @param temp Second string to join (freed inside).
 * @return Newly allocated concatenated string or NULL.
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
