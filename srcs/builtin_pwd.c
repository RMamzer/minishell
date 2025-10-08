/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 15:46:34 by rmamzer           #+#    #+#             */
/*   Updated: 2025/10/08 15:51:12 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Executes built-in 'pwd' (Print Working Directory) function. 
 * First attempts to print the value of the 'PWD' environment variable.
 * If 'PWD' is not set, falls back to using getcwd() to find and print
 * the current absolute path. Returns an error if options are used.
 * @param args Array of arguments (should be empty for standard PWD).
 * @param shell Pointer to the shell struct.
 * @return 0 on success, EXIT_INVALID_OPTION (2) if options are provided,
 * or EXIT FAILURE (1) if getcwd fails.
 */ 
int	execute_builtin_pwd(char **args, t_shell *shell)
{
	char *path;
	if (*args && **args == '-')
	{
		write_bulitin_error("minishell: pwd: ", *args,
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
		return (write_error_and_return("cd: getcwd", EXIT_FAILURE));
	printf("%s\n", path);
	free(path);
	return (0);
}