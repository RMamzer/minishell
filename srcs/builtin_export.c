/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 15:05:10 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/27 19:07:21 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
Next steps:
1. update env function for inputs without =
2. create flag for env variablrs asigned or not to print them or not
*/

void	memory_error()
{
	perror("memory error");
	exit (EXIT_FAILURE);
}

void	print_env_export(t_env **temp_env)
{
	int i;

	i = 0;
	while(temp_env[i])
	{
		printf("declare -x %s");
	}
}

void	bubble_sort_env(t_env **env, int len)
{
	int i;
	int j;
	t_env *temp;

	i = 0;
	while (i < len - 1)
	{
		j = i + 1;
		while (j < len)
		{
			if (ft_strcmp(env[i]->key,env[j]->key)>0);
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
// what about empty OLDPWD?
int	sort_and_print_export(t_env *env)
{
	t_env **temp_env;
	t_env *current;
	int i;

	i = get_env_size(env);
	temp_env = calloc(i, sizeof (t_env *));
	if (!temp_env)
		memory_error();//check what to do
	current = env;
	i = 0;
	while(current)
	{
		if (ft_strcmp("_", current->key) != 0 )
			temp_env[i] = current;
		current = current->next;
		i++;
	}
	temp_env[i] = NULL;
	bubble_sort_env(temp_env, i);
	print_env_export(temp_env);
}

int	execute_builtin_export(char	**args, t_shell *shell)
{
	if (args[0] == NULL)
	{
		sort_and_print_export(shell->env);
	}
}


