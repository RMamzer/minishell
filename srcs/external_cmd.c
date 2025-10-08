/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   external_cmd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 22:15:08 by rmamzer           #+#    #+#             */
/*   Updated: 2025/10/08 22:22:59 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Recreates envp array using the list of env nodes.
 * @param envp First node of the environment list.
 * @param shell Pointer to the shell struct.
 * @return Void.
 */
void	recreate_envp_array(t_env *env, t_shell *shell)
{
	int		i;
	t_env	*temp;

	i = 0;
	temp = env;
	shell->env_array = malloc(sizeof(char *) * (get_var_num(env, EXECUTE)
				+ 1));
	if (!shell->env_array)
		fatality(ERROR_MEM, shell, 1);
	while (temp)
	{
		if (temp->assigned == true)
		{
			shell->env_array[i] = super_strjoin(temp->key, "=", temp->value);
			if (!shell->env_array[i])
				fatality(ERROR_MEM, shell, 1);
			i++;
		}
		temp = temp->next;
	}
	shell->env_array[i] = NULL;
}

/**
 * Checks if cmd is an absolute or relative path. If not a path,
 *  checks if cmd one of $PATH paths
 * @param args Array of arguments containing cmd name and its arguments.
 * @param mallocced Bool indicating if returned value is malloced .
 * @param shell Pointer to the shell struct.
 * @return Void.
 */
char	*find_path_cmd(char **args, bool *malocced, t_shell *shell)
{
	char	*cmd_path;
	int		i;

	if (ft_strchr(args[0], '/'))
	{
		if (access(args[0], F_OK) == 0)
			return (args[0]);
		write_bulitin_error("minishell: ", NULL, NULL, args[0]);
		brutality(NULL, shell, EXIT_CMD_NOT_FOUND);
	}
	i = 0;
	*malocced = true;
	while (shell->paths_array[i])
	{
		cmd_path = super_strjoin(shell->paths_array[i], "/", args[0]);
		if (!cmd_path)
			fatality(ERROR_MEM, shell, 1);
		if (access(cmd_path, F_OK) == 0)
			return (cmd_path);
		free(cmd_path);
		i++;
	}
	write_bulitin_error("minishell: ", *args, ": command not found\n", NULL);
	brutality(NULL, shell, EXIT_CMD_NOT_FOUND);
	return (NULL);
}

/**
 * Reassembles envp array for execve, finds path to cmd and executes cmd.
 * @param args Array of arguments containing cmd name and its arguments.
 * @param shell Pointer to the shell struct.
 * @return Void.
 */
void	execute_cmd_child(char **args, t_shell *shell)
{
	char	*env_path;
	char	*cmd_path;
	bool	malloced;

	malloced = false;
	env_path = (get_env_value("PATH", shell->env, NO_ALLOC));
	if (!env_path)
	{
		write_bulitin_error("minishell: ", *args, ": command not found\n", NULL);
		brutality(NULL, shell, EXIT_CMD_NOT_FOUND);
	}
	shell->paths_array = ft_split(env_path, ':');
	if (!shell->paths_array)
		fatality(ERROR_MEM, shell, 1);
	recreate_envp_array(shell->env, shell);
	cmd_path = find_path_cmd(args, &malloced, shell);
	if (access(cmd_path, X_OK) == 0)
	{
		child_signal();
		execve(cmd_path, args, shell->env_array);
	}
	if (malloced == true)
		free (cmd_path);
	write_bulitin_error("minishell:", NULL, NULL, args[0]);
	brutality(NULL, shell, EXIT_CMD_NOT_EXEC);
}

/**
 * Creates a child process to execute an external command.
 * @param args Array of arguments containing cmd name and its arguments.
 * @param shell Pointer to the shell struct.
 * @return exit status of the command execution.
 */
int	execute_external_cmd(char **args, t_shell *shell)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		return (write_error_and_return("fork", EXIT_FAILURE));
	if (pid == 0)
		execute_cmd_child(args, shell);
	return (wait_child(pid, shell));
}