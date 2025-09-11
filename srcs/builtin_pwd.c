/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 15:46:34 by rmamzer           #+#    #+#             */
/*   Updated: 2025/09/11 19:19:00 by rmamzer          ###   ########.fr       */
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
	if (!shell)
		return (1);
	// path = get_env_value("PWD", shell->env, NO_ALLOC);
	// if (!path)
	// 	return (EXIT_FAILURE);
	// printf("%s\n", path);
	path = getcwd(NULL, 0);
	if (!path)
		return (write_error_and_return("cd: getcwd", errno));
	printf("%s\n", path);
	free(path);
	return (0);
}