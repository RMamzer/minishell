/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 15:05:10 by rmamzer           #+#    #+#             */
/*   Updated: 2025/10/08 22:28:15 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Prints the array of environment nodes to STDOUT in the required
 * `declare -x KEY="VALUE"` format for the 'export' command.
 * @param temp_env Array of environment node pointers (already sorted).
 * @return Void.
 */
void	print_env_export(t_env **temp_env)
{
	int	i;

	i = 0;
	while (temp_env[i])
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		if (temp_env[i]->assigned == false)
			printf("%s\n", temp_env[i]->key);
		else
			printf("%s=\"%s\"\n", temp_env[i]->key, temp_env[i]->value);
		i++;
	}
}

/**
 * Sorts the array of environment node pointers alphabetically by key
 * using the Bubble Sort algorithm.
 * @param env Array of environment node pointers.
 * @param len The number of elements in the array.
 * @return Void.
 */
void	bubble_sort_env(t_env **env, int len)
{
	int		i;
	int		j;
	t_env	*temp;

	i = 0;
	while (i < len - 1)
	{
		j = i + 1;
		while (j < len)
		{
			if (ft_strcmp(env[i]->key, env[j]->key) > 0)
			{
				temp = env[j];
				env[j] = env[i];
				env[i] = temp;
			}
			j++;
		}
		i++;
	}
}

/**
 * Prepares the environment list for 'export' display: creates a temporary
 * array, filters out the non-standard '_' variable, sorts the array
 * alphabetically by key, and prints the result.
 * @param env First node of the environment list.
 * @param shell Pointer to the shell struct.
 * @return 0 on success.
 */
int	sort_and_print_export(t_env *env, t_shell *shell)
{
	t_env	**temp_env;
	t_env	*current;
	int		i;

	i = get_var_num(env, EXPORT);
	temp_env = calloc(i, sizeof(t_env *));
	if (!temp_env)
		fatality(ERROR_MEM, shell, 1);
	current = env;
	i = 0;
	while (current)
	{
		if (ft_strcmp("_", current->key) != 0)
		{
			temp_env[i] = current;
			i++;
		}
		current = current->next;
	}
	temp_env[i] = NULL;
	bubble_sort_env(temp_env, i);
	print_env_export(temp_env);
	free(temp_env);
	return (0);
}

/**
 * Executes the built-in 'export' command. With no arguments, prints the
 * sorted environment variables. With arguments, validates each and adds/updates
 * environment variables. Sets the shell's exit code to 1 on identifier
 * validation failure, but continues processing other arguments.
 * @param args Array of arguments for command execution.
 * @param shell Pointer to the shell struct.
 * @return Exit status of command execution (0 on full success, 1 on failure).
 */
int	execute_builtin_export(char **args, t_shell *shell)
{
	int	i;

	if (args[0] == NULL)
		return (sort_and_print_export(shell->env, shell));
	if (args[0][0] == '-')
	{
		write_bulitin_error("minishell: export: ", *args,
			": options are not supported\nexport: usage: [name ...]\n", NULL);
		return (EXIT_INVALID_OPTION);
	}
	i = -1;
	while (args[++i])
	{
		if (is_identifier_valid(args[i]) == false)
		{
			write_bulitin_error("minishell: export: `", args[i],
				"': not a valid identifier\n", NULL);
			shell->exit_code = EXIT_FAILURE;
		}
		else if (ft_strchr(args[i], '='))
			process_env_line(shell, args[i], EXPORT);
		else
			process_valueless_export_node(shell, args[i]);
	}
	return (shell->exit_code);
}
