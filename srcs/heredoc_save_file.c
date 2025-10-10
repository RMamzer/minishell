/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_save_file.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 16:20:57 by mklevero          #+#    #+#             */
/*   Updated: 2025/10/10 15:06:06 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	**allocate_heredoc_array(t_shell *shell, char *file, size_t *count);

/**
 * Saves a heredoc temporary file path for later cleanup.
 * Maintains an array of all temporary files created for heredocs.
 *
 * @param shell Pointer to shell structure
 * @param file Temporary file path to save
 */
void	save_heredoc_file(t_shell *shell, char *file)
{
	char	**new;
	size_t	count;
	size_t	i;

	i = 0;
	new = allocate_heredoc_array(shell, file, &count);
	while (i < count)
	{
		new[i] = shell->heredoc_files[i];
		i++;
	}
	new[count] = ft_strdup(file);
	if (!new[count])
	{
		free(new);
		free(file);
		fatality(ERROR_MEM, shell, 1);
	}
	free(shell->heredoc_files);
	shell->heredoc_files = new;
}

/**
 * Allocates a new heredoc files array with additional space.
 * Helper function for expanding the heredoc files array.
 *
 * @param shell Pointer to shell structure
 * @param file Temporary file path (for error cleanup)
 * @param count Pointer to store current file count
 * @return Newly allocated array with space for one more file
 */
static char	**allocate_heredoc_array(t_shell *shell, char *file, size_t *count)
{
	char	**new;

	*count = 0;
	if (shell->heredoc_files)
		while (shell->heredoc_files[*count])
			(*count)++;
	new = ft_calloc(*count + 2, sizeof(char *));
	if (!new)
	{
		free(file);
		fatality(ERROR_MEM, shell, 1);
	}
	return (new);
}
