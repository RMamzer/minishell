/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 16:05:01 by rmamzer           #+#    #+#             */
/*   Updated: 2025/10/10 15:28:37 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Executes built-in unset function. Returns an error message if options
 * are used. Function deletes the inputted environment variables except "_".
 *
 * @param args List of arguments for command execution
 * @param shell Pointer to the shell struct
 * @return Exit status of command execution
 */
int	execute_builtin_unset(char **args, t_shell *shell)
{
	int	i;

	if (*args && **args == '-')
	{
		write_bulitin_error("minishell: unset: ", *args,
			": options are not supported\nunset:  usage: [name ...]\n", NULL);
		return (EXIT_INVALID_OPTION);
	}
	i = 0;
	while (args[i])
	{
		if (ft_strcmp("_", args[i]) != 0)
			remove_env_variable(&shell->env, args[i]);
		i++;
	}
	return (0);
}
