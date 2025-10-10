/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 15:37:48 by rmamzer           #+#    #+#             */
/*   Updated: 2025/10/10 14:55:19 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Check if the variable is an variance of "-n" option for echo buildin
 * @param str Pointer to the char argument
 * @return Bool value indicating check result (true if an option, false if not).
 */
bool	check_echo_flag(char *str)
{
	if (*str != '-')
		return (false);
	str++;
	while (*str)
	{
		if (*str != 'n')
			return (false);
		str++;
	}
	return (true);
}

/**
 * Executes the built-in 'echo' command. The command accepts  "-n" option.
 * The function prints the provided arguments with the space in between them and
 * a new line at the end. -n option removes the new line.
 * @param args Array of arguments for command execution.
 * @return Exit status of command execution (0 on full success, 1 on failure).
 */
int	execute_builtin_echo(char **args)
{
	bool	n_output;
	int		i;

	i = 0;
	n_output = true;
	while (args[i])
	{
		if (check_echo_flag(args[i]))
		{
			n_output = false;
			i++;
		}
		else
		{
			ft_putstr_fd(args[i], STDOUT_FILENO);
			i++;
			if (args[i])
				ft_putchar_fd(' ', STDOUT_FILENO);
		}
	}
	if (n_output == true)
		ft_putchar_fd('\n', STDOUT_FILENO);
	return (0);
}
