/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_functions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 16:00:20 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/30 14:14:47 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/*
needs to be done:
- check exit paths
- make code more readable?
- do i need to build whole env back before the execution?
*/

void	error_env_exit(char *key, char *value, t_shell *shell)
{
	if (key)
		free(key);
	if (value)
		free(value);
	(void)shell;
}

void	free_env_node(t_env *env)
{
	if (env)
	{
		free(env->key);
		if (env->assigned == true)
			free(env->value);
		free(env);
	}
}

void	remove_env_variable(t_env **env, char *key)
{
	t_env *current;
	t_env *previous;

	if (!key || !env || !(*env))
		return ;
	current = *env;
	if ( ft_strcmp((*env)->key, key) == 0)
	{
		*env = current->next;
		free_env_node(current);
		return ;
	}
	while (current)
	{
		if (ft_strcmp(current->key, key)== 0)
		{
			previous->next = current->next;
			free_env_node(current);
			return ;
		}
		previous = current;
		current = current->next;
	}
}

// update the value of a key to a new value: do we need to send there strdup?
// can add new_value check and error exit with malloc error here,
// so we can send ft_strdup
bool	update_env_value(t_env **env, char *key, char *new_value)
{
	t_env	*temp;

	if (!new_value || !env || !(*env))
		return (false);
	temp = *env;
	while (temp)
	{
		if (ft_strcmp(temp->key, key) == 0)
		{
			free(temp->value);
			temp->value = new_value;
			return (true);
		}
		temp = temp->next;
	}
	return (false);
}


// updates shllvl value in environment. Increments it by 1 compared to the previous value
void	update_shllvl_value(t_shell *shell)
{
	char	*value_shlvl;
	int		level;

	value_shlvl = get_env_value("SHLVL", shell->env, NO_ALLOC);
	// if !value --> malloc
	level = ft_atoi(value_shlvl) + 1;
	value_shlvl = ft_itoa(level);
	if (!value_shlvl)
		error_exit("minishell: itoa failed");
	// <---------------------- what fucntion to exit with?
	update_env_value(&shell->env, "SHLLVL", value_shlvl);
}

// add the node to the env and connect last->next to new_node
void	add_env_node(t_env **env, t_env *new_node)
{
	t_env	*last;

	if (!new_node)
		return ;
	if (!(*env))
	{
		*env = new_node;
		return ;
	}
	last = *env;
	while (last->next != NULL)
		last = last->next;
	last->next = new_node;
}

t_env	*create_env_node(char *key, char *value)
{
	t_env	*new_node;

	// check later,
	// would it mean that malloc broke or no input?
	if (!key)
		return (NULL);
	new_node = calloc(1, sizeof(t_env));
	if (!new_node)
		return (NULL);
	new_node->key = key;
	if (value)
	{
		new_node->value = value;
		new_node->assigned = true;
	}
	return (new_node);
}

void	process_env_line(t_shell *shell, const char *envp)
{
	char	*key;
	char	*value;
	char	*equal;
	t_env	*new_node;

	key = NULL;
	value = NULL;
	equal = ft_strchr(envp, '=');
	if (!equal)
		return;
	key = ft_substr(envp, 0, equal - envp);
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

void	set_minimal_env(t_shell *shell)
{
	char	*pwd_line;
	char	*pwd;
	t_env	*old_pwd_node;

	pwd = getcwd(NULL, 0);
	if (!pwd)
		error_env_exit(NULL, NULL, shell);
	pwd_line = ft_strjoin("PWD=", pwd);
	free(pwd);
	if (!pwd_line)
		error_env_exit(NULL, NULL, shell);
	process_env_line(shell, pwd_line);
	process_env_line(shell, "SHLVL=0");
	process_env_line(shell, "_=/usr/bin/env");
	old_pwd_node = create_env_node(ft_strdup("OLDPWD"),NULL);
	if (!old_pwd_node)
		error_env_exit(NULL, NULL, shell);
	add_env_node(&shell->env, old_pwd_node);
}

void	create_env(t_shell *shell, char **envp)
{
	char	*shell_name;

	if (!envp || !envp[0])
		set_minimal_env(shell);
	else
	{
		while (*envp)
		{
			process_env_line(shell, *envp);
			envp++;
		}
	}
	update_shllvl_value(shell);
	shell_name = ft_strdup("minishell");
	if (!shell_name)
		error_exit("minishell: malloc failed:"); //<-what fucntion to exit with?
	update_env_value(&shell->env, "SHELL", shell_name);
}