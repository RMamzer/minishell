/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 14:15:55 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/29 14:13:52 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"


/**
 * Signal hook callback for readline to handle SIGINT during heredoc.
 * Sets rl_done to 1 when SIGINT is received, causing readline to exit.
 *
 * @return Always returns 0 (required by readline)
 */
int check_sig_hook(void)
{
	if (g_sig == SIGINT)
		rl_done = 1;
	return (0);
}


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
bool	process_heredoc_token(t_shell *shell, t_token *current, size_t i)
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
void	process_delim(t_token *delim, t_shell *shell)
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
void	update_file_name(char **file, size_t *i, t_shell *shell)
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
 * Handles signal interruption during heredoc input.
 * Checks for SIGINT and performs cleanup if interrupted.
 * 
 * @param line Current input line to free if needed
 * @return FAILURE if interrupted, SUCCESS otherwise
 */
static bool	handle_heredoc_signal(char *line)
{
	if (g_sig == SIGINT)
	{
		if (line)
			free(line);
		g_sig = 0;
		rl_event_hook = NULL;
		return (FAILURE);
	}
	return (SUCCESS);
}

/**
 * Writes a line to the heredoc file with newline.
 * Handles write errors and performs cleanup on failure.
 * 
 * @param fd File descriptor to write to
 * @param line Line to write
 * @param shell Shell structure for error handling
 */
static void	write_heredoc_line(int *fd, char *line, t_shell *shell)
{
	if (write(*fd, line, ft_strlen(line)) == -1)
	{
		close(*fd);
		free(line);
		brutality("minishell: write", shell, 1);
	}
	if (write(*fd, "\n", 1) == -1)
	{
		close(*fd);
		free(line);
		brutality("minishell: write", shell, 1);
	}
}

/**
 * Reads heredoc input from user until delimiter is found.
 * Handles variable expansion in unquoted heredocs and writes
 * content to the temporary file.
 * 
 * @param fd Pointer to file descriptor of temporary file
 * @param delim Delimiter token to match against
 * @param shell Pointer to shell structure
 * @param file Temporary file path for error cleanup
 * @return SUCCESS if completed normally, FAILURE if interrupted
 */
bool	read_heredoc(int *fd, t_token *delim, t_shell *shell, char *file)
{
	char	*line;

	set_heredoc_signal();
	rl_event_hook = check_sig_hook;
	while (1)
	{
		line = readline("> ");
		if (handle_heredoc_signal(line) == FAILURE)
			return (FAILURE);
		if (!line)
		{
			ft_putendl_fd(ERROR_EOF, 2);
			break ;
		}
		if (is_delim_written(line, delim))
			break ;
		if (delim->quoted == false)
			expand_heredoc(&line, shell, file, fd);
		write_heredoc_line(fd, line, shell);
		free(line);
	}
	rl_event_hook = NULL;
	return (SUCCESS);
}

/**
 * Checks if the input line matches the heredoc delimiter.
 * Compares the line with the delimiter and frees the line if they match.
 * This indicates the end of heredoc input.
 * 
 * @param line Input line to compare with delimiter
 * @param delim Delimiter token containing the end marker
 * @return true if line matches delimiter, false otherwise
 */
bool is_delim_written(char *line, t_token *delim)
{
    if(ft_strcmp(line, delim->content) == 0)
    {
        free(line);
        return (true);
    }
    return (false);
}

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
char	*heredoc_expander(char *line, t_shell *shell, char *file, int *fd)
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
bool	expand_line(char *line, char **new_content, t_shell *shell)
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
char	*get_new_content(char *line, size_t *i, t_shell *shell)
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
/**
 * Converts a HEREDOC token to an input redirection token.
 * Updates the token type and content to redirect from the temporary file.
 * 
 * @param current HEREDOC token to convert
 * @param file Temporary file path containing heredoc content
 * @param shell Pointer to shell structure for error handling
 */
void	update_heredoc_token(t_token *current, char *file, t_shell *shell)
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
char	**allocate_heredoc_array(t_shell *shell, char *file, size_t *count)
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

/**
 * Cleans up all heredoc temporary files.
 * Removes files from filesystem and frees the tracking array.
 * Called at the end of command execution.
 * 
 * @param shell Pointer to shell structure
 */
void	free_heredoc_files(t_shell *shell)
{
	size_t	i;

	if (!shell->heredoc_files)
		return ;
	i = 0;
	while (shell->heredoc_files[i])
	{
		unlink(shell->heredoc_files[i]);
		free(shell->heredoc_files[i]);
		i++;
	}
	free(shell->heredoc_files);
	shell->heredoc_files = NULL;
}

