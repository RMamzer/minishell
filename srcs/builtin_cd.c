/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2025/09/10 18:51:26 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	change_working_directory(char *path, t_shell *shell)
{
	char	*new_pwd;

	if (!path)
	{
		ft_putstr_fd("minishell: cd: HOME not set\n", STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	if (chdir(path) != 0)
	{
		write_bulitin_error("minishell: cd: ", NULL, NULL, path);
		return (EXIT_FAILURE); // heck code here
	}
	new_pwd = getcwd(NULL, 0);
	if (!new_pwd)
		return (write_error_and_return("cd: getcwd", errno));
	if (update_env_value(&shell->env, "OLDPWD", get_env_value("PWD", shell->env,
				ALLOC)) == false)
	{
		free(new_pwd);
		write_error_malloc();
	}
	update_env_value(&shell->env, "PWD", new_pwd);
	return (EXIT_SUCCESS);
}

int	execute_builtin_cd(char **args, t_shell *shell)
{
	if (!args[0])
		return (change_working_directory(get_env_value("HOME", shell->env,
					NO_ALLOC), shell));
	if (args[0][0] == '-')
	{
		write_bulitin_error("minishell: cd: ", args[0], ": invalid option\n",
			NULL);
		return (EXIT_INVALID_OPTION);
	}
	if (get_args_len(args) > 1)
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	return (change_working_directory(args[0], shell));
}
