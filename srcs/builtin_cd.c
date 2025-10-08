/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2025/10/08 14:35:29 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Executes the change of the directory using chdir() function. On succesfull 
 * change, also updates PWF and OLDPWD environment variables
 * 
 * @param path Path to the new directory.
 * @param shell Pointer to the shell struct.
 * @return Exit status of command execution (0 on full success, 1 on failure).
 */
int	change_working_directory(char *path, t_shell *shell)
{
	char	*new_pwd;
	char	*old_pwd;

	if (chdir(path) != 0)
	{
		write_bulitin_error("minishell: cd: ", NULL, NULL, path);
		return (EXIT_FAILURE);
	}
	if (get_env_value("PWD", shell->env, NO_ALLOC))
	{
		old_pwd = get_env_value("PWD", shell->env, ALLOC);
		if (!old_pwd)
			fatality(ERROR_MEM, shell, 1);
		if (update_env_value (&shell->env, "OLDPWD", old_pwd) == false)
			free (old_pwd);
		new_pwd = getcwd(NULL, 0);
		if (!new_pwd)
			return (write_error_and_return("cd: getcwd", errno));
		if (update_env_value(&shell->env, "PWD", new_pwd) == false)
			free(new_pwd);
	}
	return (EXIT_SUCCESS);
}


/**
 * Executes the built-in 'cd' command. The command does not accept options.
 * The command changes the current working directory based on the arguments provided. 
 * if no arguments are passed, changes the dir to HOME. 
 * 
 * @param args Arguments passed to the command.
 * @param shell Pointer to the shell struct.
 * @return Exit status of command execution (0 on full success, 1 on failure).
 */
int	execute_builtin_cd(char **args, t_shell *shell)
{
	char	*path;

	if (!args[0])
	{
		path = get_env_value("HOME", shell->env, NO_ALLOC);
		if (!path)
		{
			ft_putstr_fd("minishell: cd: HOME not set\n",STDERR_FILENO);
			return (EXIT_FAILURE);
		}
	return (change_working_directory(path, shell));
	}
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
