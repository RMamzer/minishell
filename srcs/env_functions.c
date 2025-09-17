/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_functions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamzerr1 <mamzerr1@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 16:00:20 by rmamzer           #+#    #+#             */
/*   Updated: 2025/09/17 14:23:14 by mamzerr1         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"



void	error_malloc_env_exit(char *key, char *value, t_shell *shell)
{
	perror("minishell: malloc");
	if (key)
		free(key);
	if (value)
		free(value);
	free_shell(shell);
	exit (EXIT_FAILURE);
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
	t_env	*current;
	t_env	*previous;

	if (!key || !env || !(*env))
		return ;
	current = *env;
	if (ft_strcmp((*env)->key, key) == 0)
	{
		*env = current->next;
		free_env_node(current);
		return ;
	}
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
		{
			previous->next = current->next;
			free_env_node(current);
			return ;
		}
		previous = current;
		current = current->next;
	}
}


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
			if (temp->value)
				free(temp->value);
			temp->value = new_value;
			temp->assigned = true;
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
	level = ft_atoi(value_shlvl) + 1;
	value_shlvl = ft_itoa(level);
	if (!value_shlvl)
		error_malloc_env_exit(NULL, NULL, shell); 
	if (update_env_value(&shell->env, "SHLVL", value_shlvl) == false)
		free (value_shlvl);
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

	if (!key) // check cause i throw malloc here, cant find case how to get no key otherwise
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
		return ;
	key = ft_substr(envp, 0, equal - envp);
	if (!key)
		error_malloc_env_exit(key, value, shell); 
	value = ft_strdup(equal + 1);
	if (!value)
		error_malloc_env_exit(key, value, shell); 
	new_node = create_env_node(key, value);
	if (!new_node)
		error_malloc_env_exit(key, value, shell);
	add_env_node(&shell->env, new_node);
}
// check it later
void	set_minimal_env(t_shell *shell)
{
	char	*pwd_line;
	char	*pwd;
	t_env	*old_pwd_node;

	pwd = getcwd(NULL, 0);
	if (!pwd)
		error_malloc_env_exit(NULL, NULL, shell);
	pwd_line = ft_strjoin("PWD=", pwd);
	free(pwd);
	if (!pwd_line)
		error_malloc_env_exit(NULL, NULL, shell);
	process_env_line(shell, pwd_line); // check if it leaks (set env env -i)
	process_env_line(shell, "SHLVL=0");
	process_env_line(shell, "_=/usr/bin/env");
	old_pwd_node = create_env_node(ft_strdup("OLDPWD"), NULL);
	if (!old_pwd_node)
		error_malloc_env_exit(NULL, NULL, shell);
	add_env_node(&shell->env, old_pwd_node);
}

void	create_env(t_shell *shell, char **envp)
{
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
	update_shllvl_value(shell); // check overflow lol?
}
/*
1. What should be the malloc error message and exit code?
2. What should be the structure of free functions? (env is separate from bash_init_free)
*/