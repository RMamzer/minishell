/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 15:10:59 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/30 14:40:15 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//check if it works for dup"""
int	execute_builtin_env(char **args, t_shell *shell)
{
	t_env *temp;

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