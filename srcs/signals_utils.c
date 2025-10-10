/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 15:48:33 by mklevero          #+#    #+#             */
/*   Updated: 2025/10/10 17:01:10 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	signal_to_exitcode(t_shell *shell)
{
	static t_shell	*sig_shell;

	if (shell)
		sig_shell = shell;
	else if (g_sig != 0)
	{
		sig_shell->exit_code = g_sig + 128;
	}
	g_sig = 0;
}

/**
 * Signal handler for SIGINT during readline input.
 * Provides clean interrupt behavior when Ctrl+C is pressed.
 * Updates display and sets signal for exit code.
 *
 * @param sig Signal number received
 */
void	handle_readline_sigint(int sig)
{
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	g_sig = sig;
	signal_to_exitcode(NULL);
}

/**
 * Signal handler for SIGINT during heredoc input.
 * Handles interruption of heredoc reading.
 *
 * @param signum Signal number received
 */
void	handle_heredoc_signal(int signum)
{
	g_sig = signum;
	signal_to_exitcode(NULL);
}

/**
 * Signal handler for SIGINT during exec input.
 * Handles interruption of execution.
 *
 * @param signum Signal number received
 */
void	handle_exe_sigint(int signum)
{
	(void)signum;
	write(1, "\n", 1);
}
