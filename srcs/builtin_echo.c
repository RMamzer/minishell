/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 15:37:48 by rmamzer           #+#    #+#             */
/*   Updated: 2025/10/08 14:26:47 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Executes the built-in 'echo' command. The command accepts  "-n" option.
 * The function prints the provided arguments with the space in between them and
 * a new line at the end. -n option removes the new line.
 * @param args Array of arguments for command execution.
 * @return Exit status of command execution (0 on full success, 1 on failure).
 */
int	execute_builtin_echo(char **args)
{
	bool	n_flag;
	int		i;

	i = 0;
	n_flag = false;
	if (*args)
	{
		while ( args[i] && ft_strcmp("-n", args[i]) == 0)
		{
			n_flag = true;
			i++;
		}
		while (args[i])
		{
			ft_putstr_fd(args[i], STDOUT_FILENO);
			i++;
			if (args[i])
				ft_putchar_fd(' ', STDOUT_FILENO);
		}
	}
	if (n_flag == false)
		ft_putchar_fd('\n', STDOUT_FILENO);
	return (0);
}
