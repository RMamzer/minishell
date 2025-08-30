/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 15:05:10 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/30 16:33:21 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
Next steps:
1. update the error message when issue detected in export var
2. update process line code
3. check that reguar execution works correctly;
*/

void	memory_error()
{
	perror("memory error");
	exit (EXIT_FAILURE);
}


bool is_identifier_valid(char *str)
{
	if (!str || *str =='\0' || ft_isdigit(*str) || *str =='=')
		return (false);
	while (*str && *str != '=')
	{
		if (!ft_isalnum(*str) && *str !='_')
			return (false);
		str++;
	}
	return (true);
}


void	print_env_export(t_env **temp_env)
{
	int i;

	i = 0;
	while(temp_env[i])
	{
		ft_putstr_fd("declare -x ", STDOUT_FILENO);
		if (temp_env[i]->assigned == false)
			printf("%s\n", temp_env[i]->key);
		else
			printf("%s=\"%s\"\n",temp_env[i]->key, temp_env[i]->value);
		i++;
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
			if (ft_strcmp(env[i]->key,env[j]->key)>0)
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

int	execute_builtin_export(char	**args, t_shell *shell)
{
	int i;

	if (args[0] == NULL)
		return(sort_and_print_export(shell->env));
	i = 0;
	if (args[i] == '-')
	{
		ft_putstr_fd("minishell: export: ", STDERR_FILENO);
		ft_putstr_fd(*args, STDERR_FILENO);
		ft_putstr_fd(": options are not supported\n", STDERR_FILENO);
		ft_putstr_fd("export:  usage: [name ...]\n", STDERR_FILENO);
		return (EXIT_INVALID_OPTION);
	}
	while(args[i])
	{
		if (is_identifier_valid(args[i]) == false)
		{
			ft_putstr_fd("minishell: export: `", STDERR_FILENO);
			ft_putstr_fd(args[i], STDERR_FILENO);
			ft_putstr_fd("': not a valid identifier\n", STDERR_FILENO);
			//update error code
		}
		i++;
	}
	return (0);
}


