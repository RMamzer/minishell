/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/10 15:46:32 by mklevero          #+#    #+#             */
/*   Updated: 2025/10/13 16:20:23 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Sets up signal handling for readline input phase.
 * Configures appropriate signal behavior for interactive input.
 */
void	set_readline_signals(void)
{
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, handle_readline_sigint);
}

/**
 * Sets default signal handling for child processes.
 * Restores default signal behavior for executed commands.
 */
void	set_child_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

/**
 * Sets up signal handling for heredoc input phase.
 * Configures signals for heredoc reading with special behavior.
 */
void	set_heredoc_signals(void)
{
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, handle_heredoc_signal);
}

/**
 * Sets signal handling during command execution preparation.
 * Ignores signals while setting up execution environment.
 */
void	set_exec_signals(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

/**
 * Sets signal handling for parent process during command execution.
 * Provides appropriate signal handling while waiting for children.
 */
void	set_exec_parent_signals(void)
{
	signal(SIGINT, handle_exe_sigint);
	signal(SIGQUIT, SIG_IGN);
}
