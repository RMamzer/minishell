/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 15:05:10 by rmamzer           #+#    #+#             */
/*   Updated: 2025/09/23 18:16:38 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"




void	process_valueless_export_node(t_shell *shell, char *str)
{
	char	*key;
	t_env	*new_node;

	if (get_env_value(str, shell->env, NO_ALLOC))
		return;
	key = ft_strdup(str);
	if (!key)
		fatality(ERROR_MEM, shell, 1);
	new_node = create_env_node(key, NULL);
	if (!new_node)
	{
		free (key);
		fatality(ERROR_MEM, shell, 1);
	}
	add_env_node(&shell->env, new_node);
}

bool	is_identifier_valid(char *str)
{
	if (!str || *str == '\0' || ft_isdigit(*str) || *str == '=')
		return (false);
	while (*str && *str != '=')
	{
		if (!ft_isalnum(*str) && *str != '_')
			return (false);
		str++;
	}
	return (true);
}

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

int	sort_and_print_export(t_env *env, t_shell *shell)
{
	t_env	**temp_env;
	t_env	*current;
	int		i;

	i = get_env_size(env, EXPORT);
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
			process_env_line(shell, args[i],EXPORT); // check
		else
			process_valueless_export_node(shell, args[i]);
	}
	return (shell->exit_code);
}
