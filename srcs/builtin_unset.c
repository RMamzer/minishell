/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/25 16:05:01 by rmamzer           #+#    #+#             */
/*   Updated: 2025/09/01 17:20:40 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute_builtin_unset(char **args, t_shell *shell)
{
	int	i;

	if (*args && **args == '-')
	{
		write_bulitin_error("minishell: unset: ", *args,
			": options are not supported\n", "unset:  usage: [name ...]\n");
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
