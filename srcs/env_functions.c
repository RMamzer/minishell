/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_functions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 16:00:20 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/13 18:42:41 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"



/*
needs to be done:
- add env setup without envp
- check exit paths
- make code more readable?
- should shllvl update and shell name update be combined?
*/

void	error_exit(char	*msg)
{
	perror(msg);
	exit(errno);
}


void	error_env_exit(char *key, char *value, t_shell  *shell)
{
	if (key)
		free(key);
	if (value)
		free(value);
	if (shell)
		// should anything beyond env be freed?
}



bool	update_env_value(t_env **env, char *key, char *new_value)
{
	t_env *temp;

	if (!new_value || !env || !(*env))
		return (false);
	temp = *env;
	while (temp)
	{
		if (temp->key == key)
		{
			free(temp->value);
			temp->value = new_value;
			return (true);
		}
		temp = temp->next;
	}
	return(false);
}

char *find_env_value(char *str, t_env *env)
{
	while (env!= NULL)
	{
		if (ft_strcmp(str, env->key))
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

void	update_shllvl_values(t_shell *shell)
{
	char *value_shlvl;
	char *value_shell;
	int	level;

	value_shlvl = find_env_value("SHLVL", shell);
	level = ft_atoi(value_shlvl) + 1;
	value_shlvl = ft_itoa(level);
	if (!value_shlvl)
		error_exit("minishell: itoa failed"); // <---------------------- what fucntion to exit with?
	update_env_value(&shell->env, "SHLLVL", value_shlvl);
	value_shell = ft_strdup("minishell");
	if(!value_shell)
		error_exit("minishell: malloc failed"); //<---------------------- what fucntion to exit with?
	update_env_value(&shell->env, "SHELL", value_shell);
}

void	add_env_node(t_shell **env, t_env *new_node)
{
	t_env	*last;

	if (!new_node)
		return ;
	if ( !(env))
	{
		*env = new_node;
		return ;
	}
	last = *env;
	while (last->next != NULL)
		last = last->next;
	last->next = new_node;
}

t_env	*create_env_node(char *key, char *value) // should i add shell?
{
	t_env *new_node;

	// if (!key || !value)

	new_node =  malloc(sizeof(t_env));
	if (!new_node)
		return (NULL);
	new_node->key = key;
	new_node->value = value;
	new_node->next = NULL;
	return(new_node);
}

void	process_env_line(t_shell *shell, const char *envp)
{
	char	*key;
	char	*value;
	char	*equal;
	char	*new_node;

	key = NULL;
	value = NULL;
	equal = ft_strchr(envp, '=');
	if (!equal)
		error_env_exit(key, value, shell);
	key = ft_substr(envp, 0, equal - *envp);
	if (!key)
		error_env_exit(key, value, shell);
	value = ft_strdup(equal + 1);
	if (!value)
		error_env_exit(key, value, shell);
	new_node = create_env_node(key, value);
	if (!new_node)
		error_env_exit(key, value, shell);
	add_env_node(&shell->env, new_node);
}

void	set_minimal_env(t_shell)
{

}

void	create_env(t_shell *shell, const char **envp)
{
	char	*shell_name;

	if (!envp || !envp[0])
		set_minimal_env();
	while (*envp) // what if !envp? will it crash here?
	{
		process_env_line(shell, *envp);
		envp++;
	}
	update_shllvl_value(shell);
	shell_name = ft_strdup("minishell");
	if (!shell_name)
		error_exit("minishell: malloc failed:"); //<---------------------- what fucntion to exit with?
	update_env_value(&shell->env, "SHELL", shell_name);
}
