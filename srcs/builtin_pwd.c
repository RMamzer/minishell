/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 15:46:34 by rmamzer           #+#    #+#             */
/*   Updated: 2025/09/01 17:20:38 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute_builtin_pwd(char **args, t_shell *shell)
{
	char *path;
	if (*args)
	{
		ft_putstr_fd("minishell: pwd: no options or arguments are supported\n",
			STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	path = get_env_value("PWD", shell->env, NO_ALLOC);
	if (!path)
		return (EXIT_FAILURE);
	printf("%s\n", path);
	return (0);
}