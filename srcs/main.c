/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 14:54:37 by rmamzer           #+#    #+#             */
/*   Updated: 2025/09/25 20:29:25 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "minishell.h"

volatile sig_atomic_t	g_sig = 0;

void	signal_to_exitcode(t_shell *shell)
{
	static t_shell	*sig_shell;

	if (shell)
		sig_shell = shell;
	else if(g_sig != 0)
	{
		sig_shell->exit_code = g_sig + 128;
	}
}


void	handle_readline_sigint(int sig)
{
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	g_sig = sig;
	signal_to_exitcode(NULL);
}

void	handle_heredoc_signal(int signum)
{
	write(1, "\n", 1);
	g_sig = signum;
	signal_to_exitcode(NULL);
}

void	handle_sigint_exe(int signum)
{
	write(1, "\n", 1);
	g_sig = signum;
}


void	set_readline_signals(void)
{
	rl_done = 0;
	rl_event_hook= NULL;
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, handle_readline_sigint);
}

void	child_signal(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

void	set_heredoc_signal(void)
{
	signal(SIGQUIT, SIG_IGN);
	signal(SIGINT, handle_heredoc_signal);
}

// void	restore_main_signals(void)
// {
// 	signal(SIGQUIT, SIG_IGN);
// 	signal(SIGINT, handle_sigint);
// }




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


int	main(int ac, char **av, char **env)
{
	t_shell	*shell;

	(void)ac;
	(void)av;
	shell = init_data();
	create_env(shell, env);
	while (1)
	{
		if (receive_input(shell) == FAILURE)
			continue ;
		if (process_input(shell) == FAILURE)
			continue ;
		if (parse_tokens(shell) == FAILURE)
			continue ;
		execute_ast(shell->ast, shell);
		free_shell_data(shell);
	}
	free_shell(shell);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////




bool	receive_input(t_shell *shell)
{
	set_readline_signals();
	shell->input_line = readline("dirty_shell> ");
	if (!shell->input_line)
		fatality(NULL, shell, 0); // for now
	if (shell->input_line[0] == '\0')
	{
		free(shell->input_line);
		return (FAILURE);
	}
	if (ft_strlen(shell->input_line) > 1024) // for now
	{
		show_error("input is too long", NULL, shell, 2);
		return (FAILURE);
	}
	add_history(shell->input_line);
	return (SUCCESS);
}

bool	process_input(t_shell *shell)
{
	char	*line;

	line = ft_strtrim(shell->input_line, TO_TRIM);
	if (line == NULL || line[0] == '\0')
	{
		free(shell->input_line);
		free(line);
		return (FAILURE);
	}
	if (check_quote(line, ft_strlen(line)) != 0)
	{
		free(line);
		show_error(ERROR_QUOTE, NULL, shell, 2);
		return (FAILURE);
	}
	free(shell->input_line);
	shell->input_line = line;
	lexer(shell->input_line, shell);
	if (check_syntax(shell) == FAILURE)
		return (FAILURE);
	if (check_heredoc(shell) == FAILURE)
		return (FAILURE);
	if (process_heredoc(shell) == FAILURE)
		return (free_shell_data(shell), FAILURE);
	expander(shell);
	split_variables(shell);
	delete_empty_tokens(shell);
	return (SUCCESS);
}

t_shell	*init_data(void)
{
	t_shell *shell;

	shell = ft_calloc(1, sizeof(t_shell));
	if (shell == NULL)
	{
		ft_putendl_fd(ERROR_MEM, 2);
		exit(FAILURE);
	}
	shell->complete_exit = true;
	signal_to_exitcode(shell);
	return (shell);
}
