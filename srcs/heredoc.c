/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 14:15:55 by mklevero          #+#    #+#             */
/*   Updated: 2025/10/13 16:19:51 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	process_heredoc_token(t_shell *shell, t_token *current, size_t i);
static void	process_delim(t_token *delim, t_shell *shell);
static void	update_file_name(char **file, size_t *i, t_shell *shell);
static void	update_heredoc_token(t_token *current, char *file, t_shell *shell);

/**
 * Main heredoc processing function.
 * Processes all HEREDOC tokens in the token list by creating temporary files
 * and reading user input until delimiter is found.
 *
 * @param shell Pointer to shell structure containing token list
 * @return SUCCESS if all heredocs processed successfully, FAILURE otherwise
 */
bool	process_heredoc(t_shell *shell)
{
	size_t	i;
	t_token	*current;

	i = 0;
	current = shell->token_list;
	while (current)
	{
		if (current->type == HEREDOC && current->next)
		{
			if (process_heredoc_token(shell, current, i) == FAILURE)
				return (FAILURE);
			i++;
		}
		current = current->next;
	}
	return (SUCCESS);
}

/**
 * Processes a single heredoc token.
 * Creates a temporary file, reads input until delimiter, then converts
 * the heredoc token to a regular input redirection.
 *
 * @param shell Pointer to shell structure
 * @param current Current HEREDOC token
 * @param i Index for unique temporary file naming
 * @return SUCCESS if processed successfully, FAILURE otherwise
 */
static bool	process_heredoc_token(t_shell *shell, t_token *current, size_t i)
{
	int		fd;
	char	*file;

	file = NULL;
	process_delim(current->next, shell);
	update_file_name(&file, &i, shell);
	fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		free(file);
		brutality("minishell: open", shell, 1);
	}
	if (read_heredoc(&fd, current->next, shell, file) == FAILURE)
	{
		close(fd);
		unlink(file);
		free(file);
		return (FAILURE);
	}
	close(fd);
	update_heredoc_token(current, file, shell);
	save_heredoc_file(shell, file);
	free(file);
	return (SUCCESS);
}

/**
 * Removes quotes from heredoc delimiter.
 * Processes the delimiter token to remove surrounding quotes while
 * preserving the actual delimiter content.
 *
 * @param delim Delimiter token to process
 * @param shell Pointer to shell structure for error handling
 */
static void	process_delim(t_token *delim, t_shell *shell)
{
	char	*new_content;
	size_t	i;
	size_t	j;

	new_content = ft_calloc(ft_strlen(delim->content) + 1, sizeof(char));
	if (!new_content)
		fatality(ERROR_MEM, shell, 1);
	i = 0;
	j = 0;
	while (delim->content[i])
	{
		if (delim->content[i] == '\'' || delim->content[i] == '"')
		{
			i++;
			continue ;
		}
		new_content[j++] = delim->content[i++];
	}
	free(delim->content);
	delim->content = new_content;
}

/**
 * Generates a unique temporary file name for heredoc.
 * Creates file names in /tmp/ directory with incrementing numbers
 * until a non-existing file is found.
 *
 * @param file Pointer to store the generated file path
 * @param i Pointer to counter for unique naming
 * @param shell Pointer to shell structure for error handling
 */
static void	update_file_name(char **file, size_t *i, t_shell *shell)
{
	char	*file_num;

	while (1)
	{
		file_num = ft_itoa(*i++);
		if (!file_num)
			fatality(ERROR_MEM, shell, 1);
		*file = ft_strjoin("/tmp/kunteynir", file_num);
		free(file_num);
		if (!*file)
			fatality(ERROR_MEM, shell, 1);
		if (access(*file, F_OK) != 0)
			break ;
		free(*file);
	}
}

/**
 * Converts a HEREDOC token to an input redirection token.
 * Updates the token type and content to redirect from the temporary file.
 *
 * @param current HEREDOC token to convert
 * @param file Temporary file path containing heredoc content
 * @param shell Pointer to shell structure for error handling
 */
static void	update_heredoc_token(t_token *current, char *file, t_shell *shell)
{
	free(current->content);
	current->type = IN;
	current->content = ft_strdup("<");
	if (!current->content)
	{
		free(file);
		fatality(ERROR_MEM, shell, 1);
	}
	if (current->next)
	{
		free(current->next->content);
		current->next->type = WORD;
		current->next->content = ft_strdup(file);
		if (!current->next->content)
		{
			free(file);
			fatality(ERROR_MEM, shell, 1);
		}
	}
}
