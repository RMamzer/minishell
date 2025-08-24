/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 17:49:04 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/24 16:57:44 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int change_working_directory(char *path, t_shell *shell)
{
	char	*new_pwd;

	if (!path)
	{
		ft_putstr_fd("minishell: cd: HOME not set\n", STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	if (chdir(path) != 0)
	{
		ft_putstr_fd("minishell: cd:", STDERR_FILENO);
		error_exit(path);
	}
	new_pwd = getcwd(NULL, 0);
	if (!new_pwd) // check error message
		error_exit("minishell: cd:");
	update_env_value(&shell->env, "OLDPWD",get_env_value(&shell->env,"PWD", NO_ALLOC));
	update_env_value(&shell->env,"PWD", new_pwd);
	return (EXIT_SUCCESS);
}

// check ~ arg
int	execute_builtin_cd(char	**args , t_shell *shell)
{
	if (!args[0])
		return (change_working_directory(get_env_value(&shell->env, "HOME", NO_ALLOC), shell));
	if (args[0][0] == '-')// undefined behavior? do we want to implement -?
	{
		ft_putstr_fd("minishell: cd:", STDERR_FILENO);
		ft_putstr_fd(args[0], STDERR_FILENO);
		ft_putstr_fd(": invalid option\n", STDERR_FILENO);
		return(EXIT_INVALID_OPTION);
	}

	if (args[1]!= NULL)// check if it causes segfault or not?
	{
		ft_putstr_fd("minishell: cd: too many arguments\n", STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	return (change_working_directory(args[0], shell));
}


