/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 14:54:37 by rmamzer           #+#    #+#             */
/*   Updated: 2025/09/30 18:23:07 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "minishell.h"

volatile sig_atomic_t	g_sig = 0;

static void	print_value(char **value)
{
	int	i;

	i = 0;
	if (!value)
		return ;
	printf(" (");
	while (value[i])
	{
		if (i > 0)
			printf(" ");
		printf("%s", value[i]);
		i++;
	}
	printf(")");
}

const char	*type_to_str(t_token_type t)
{
	switch (t)
	{
	case WORD:
		return ("WORD");
	case IN:
		return ("IN");
	case OUT:
		return ("OUT");
	case APPEND:
		return ("APPEND");
	case HEREDOC:
		return ("HEREDOC");
	case PIPE:
		return ("PIPE");
	default:
		return ("UNKNOWN");
	}
}

void	print_ast(t_ast *node, int depth)
{
	int	i;

	if (!node)
	{
		printf("ast is clean\n");
		return ;
	}
	/* indentation */
	i = 0;
	while (i++ < depth)
		printf("  ");
	printf("%s", type_to_str(node->type));
	if (node->value && node->value[0])
		print_value(node->value);
	printf("\n");
	if (node->left)
		print_ast(node->left, depth + 1);
	if (node->right)
		print_ast(node->right, depth + 1);
}





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
	// write(1, "\n", 1);
	g_sig = signum;
	signal_to_exitcode(NULL);
}

void	handle_sigint_exe(int signum)
{
	(void)signum;
	write(1, "\n", 1);
}


void	set_readline_signals(void)
{
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

//  void	restore_main_signals(void)
//  {
//  	signal(SIGQUIT, SIG_IGN);
//  	signal(SIGINT, handle_sigint_exe);
// }


void set_signals_exec(void)
{
	signal(SIGINT, SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
}

void set_signals_exec_parent(void)
{
	signal(SIGINT, handle_sigint_exe);
	signal(SIGQUIT, SIG_IGN);
}


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
		//print_ast(shell->ast, 0);
		set_signals_exec();
		execute_ast(shell->ast, shell);
		free_heredoc_files(shell);
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
	shell->is_parent = true;
	signal_to_exitcode(shell);
	return (shell);
}
