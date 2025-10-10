/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_read_loop.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamzerr1 <mamzerr1@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 15:17:43 by mklevero          #+#    #+#             */
/*   Updated: 2025/10/10 09:59:15 by mamzerr1         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static bool	heredoc_signal_check(char *line);
static void	write_heredoc_line(int *fd, char *line, t_shell *shell);
static int	check_sig_hook(void);

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

	rl_event_hook = check_sig_hook;
	set_heredoc_signal();
	while (1)
	{
		line = readline("> ");
		if (heredoc_signal_check(line) == FAILURE)
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
 * Handles signal interruption during heredoc input.
 * Checks for SIGINT and performs cleanup if interrupted.
 *
 * @param line Current input line to free if needed
 * @return FAILURE if interrupted, SUCCESS otherwise
 */
static bool	heredoc_signal_check(char *line)
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
 * Signal hook callback for readline to handle SIGINT during heredoc.
 * Sets rl_done to 1 when SIGINT is received, causing readline to exit.
 *
 * @return Always returns 0 (required by readline)
 */
static int	check_sig_hook(void)
{
	if (g_sig == SIGINT)
		rl_done = 1;
	return (0);
}
