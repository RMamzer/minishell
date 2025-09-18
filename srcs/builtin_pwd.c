/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 15:46:34 by rmamzer           #+#    #+#             */
/*   Updated: 2025/09/18 16:15:54 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute_builtin_pwd(char **args, t_shell *shell)
{
	char *path;
	if (*args && **args == '-')
	{
		write_bulitin_error("minishell: unset: ", *args,
			": options are not supported\n" ,NULL);
		return (EXIT_INVALID_OPTION);
	}
	if (!shell)
		return (1);
	path = get_env_value("PWD", shell->env, NO_ALLOC);
	if (path)
	{
		printf("%s\n", path);
		return (0);
	}
	path = getcwd(NULL, 0);
	if (!path)
		return (write_error_and_return("cd: getcwd", errno));
	printf("%s\n", path);
	free(path);
	return (0);
}