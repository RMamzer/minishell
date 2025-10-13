/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_expander.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 16:16:58 by mklevero          #+#    #+#             */
/*   Updated: 2025/10/13 16:19:44 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*heredoc_expander(char *line, t_shell *shell, char *file, int *fd);
static bool	expand_line(char *line, char **new_content, t_shell *shell);
static char	*get_new_content(char *line, size_t *i, t_shell *shell);

/**
 * Wrapper function for heredoc line expansion.
 * Expands variables in a heredoc line and replaces the original.
 *
 * @param line Pointer to line to expand (will be modified)
 * @param shell Pointer to shell structure
 * @param file Temporary file path for error cleanup
 * @param fd File descriptor to close on error
 */
void	expand_heredoc(char **line, t_shell *shell, char *file, int *fd)
{
	char	*expanded;

	if (!line || !*line)
		return ;
	expanded = heredoc_expander(*line, shell, file, fd);
	free(*line);
	*line = expanded;
}

/**
 * Expands variables in a heredoc line.
 * Similar to regular expansion but simpler (no quotes to handle).
 *
 * @param line Line to expand
 * @param shell Pointer to shell structure
 * @param file Temporary file path for error cleanup
 * @param fd File descriptor to close on error
 * @return Newly allocated expanded line
 */
static char	*heredoc_expander(char *line, t_shell *shell, char *file, int *fd)
{
	char	*new_content;

	new_content = ft_strdup("");
	if (!new_content)
	{
		close(*fd);
		free(file);
		free(line);
		fatality(ERROR_MEM, shell, 1);
	}
	if (expand_line(line, &new_content, shell) == FAILURE)
	{
		close(*fd);
		free(file);
		free(new_content);
		free(line);
		fatality(ERROR_MEM, shell, 1);
	}
	return (new_content);
}

/**
 * Expands variables in a line character by character.
 * Processes the line and expands any variable references found.
 *
 * @param line Line to process
 * @param new_content Pointer to result string (will be built up)
 * @param shell Pointer to shell structure
 * @return SUCCESS if expansion completed, FAILURE on error
 */
static bool	expand_line(char *line, char **new_content, t_shell *shell)
{
	size_t	i;
	char	*temp;

	i = 0;
	while (line[i])
	{
		temp = get_new_content(line, &i, shell);
		if (!temp)
			return (FAILURE);
		*new_content = strjoin_free(*new_content, temp);
		if (!*new_content)
			return (FAILURE);
	}
	return (SUCCESS);
}

/**
 * Handles variable expansion ($VAR, $?) or literal text.
 *
 * @param line Line being processed
 * @param i Pointer to current position (will be updated)
 * @param shell Pointer to shell structure
 * @return Processed content segment
 */
static char	*get_new_content(char *line, size_t *i, t_shell *shell)
{
	size_t	start;
	char	*temp;

	if (line[*i] == '$')
	{
		(*i)++;
		if (ft_isalpha(line[*i]) || line[*i] == '_')
			temp = expand_env_var(line, i, shell->env);
		else if (line[*i] == '?')
		{
			(*i)++;
			temp = ft_itoa(shell->exit_code);
		}
		else
			temp = ft_strdup("$");
	}
	else
	{
		start = *i;
		while (line[*i] && line[*i] != '$')
			(*i)++;
		temp = ft_substr(line, start, *i - start);
	}
	return (temp);
}
