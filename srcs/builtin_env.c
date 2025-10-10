/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 15:10:59 by rmamzer           #+#    #+#             */
/*   Updated: 2025/10/10 15:22:28 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Executes the built-in 'env' command. The function does not accept any
 * arguments. The function prints the list of environment varibles
 * into the STDIN in the style "KEY=VALUE".
 *
 * @param args Array of arguments for command execution.
 * @param shell Pointer to the shell struct.
 * @return Exit status of command execution (0 on full success, 1 on failure).
 */
int	execute_builtin_env(char **args, t_shell *shell)
{
	t_env	*temp;

	if (*args)
	{
		ft_putstr_fd("minishell: env: no options or arguments are supported\n",
			STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	temp = shell->env;
	while (temp)
	{
		if (temp->assigned == true)
			printf("%s=%s\n", temp->key, temp->value);
		temp = temp->next;
	}
	return (0);
}
