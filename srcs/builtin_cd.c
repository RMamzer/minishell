/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamzerr1 <mamzerr1@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by                   #+#    #+#             */
/*   Updated: 2025/09/13 10:30:59 by mamzerr1         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "minishell.h"



// int	change_working_directory(char *path, t_shell *shell)
// {
// 	char	*new_pwd;
// 	char	*old_pwd:

// 	if (!path)
// 		return (write_error_and_return("minishell: cd: HOME not set\n", EXIT_FAILURE));
// 	if (chdir(path) != 0)
// 	{
// 		write_bulitin_error("minishell: cd: ", NULL, NULL, path);
// 		return (EXIT_FAILURE);
// 	}
// 	new_pwd = getcwd(NULL, 0);
// 	if (!new_pwd)
// 		return (write_error_and_return("cd: getcwd", errno));
// 	if (get_env_value("PWD", shell->env, NO_ALLOC) && get_env_value("PWD", shell->env, NO_ALLOC))
// 	{
// 		update_env_value(&shell->env, "OLDPWD", get_env_value("PWD", shell->env,
// 		ALLOC)) == false)
// 			{
// 				free (new_pwd);
// 				write_error_malloc();
// 			}
// 	}
// 	if (update_env_value(&shell->env, "PWD", new_pwd) == false)
// 		free(new_pwd);
// 	return (EXIT_SUCCESS);
// }


int	change_working_directory(char *path, t_shell *shell)
{
	char	*new_pwd;
	char	*old_pwd;

	if (chdir(path) != 0)
	{
		printf("chdir fail");
		write_bulitin_error("minishell: cd: ", NULL, NULL, path);
		return (EXIT_FAILURE);
	}
	if (get_env_value("PWD", shell->env, NO_ALLOC))
	{
		old_pwd = get_env_value("PWD", shell->env, ALLOC);
		if (!old_pwd)
			write_error_malloc();
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
