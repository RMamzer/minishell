/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_functions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 16:00:20 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/15 18:19:46 by mklevero         ###   ########.fr       */
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

void	error_exit(char *msg)
{
	perror(msg);
	exit(errno);
}

void	error_env_exit(char *key, char *value, t_shell *shell)
{
	if (key)
		free(key);
	if (value)
		free(value);
	(void)shell;
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
		if (ft_strcmp(temp->key, key))
		{
			free(temp->value);
			temp->value = new_value;
			return (true);
		}
		temp = temp->next;
	}
	return (false);
}

char	*find_env_value(char *str, t_env *env)
{
	while (env != NULL)
	{
		if (ft_strcmp(str, env->key))
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

// updates shllvl value in environment. Increments it by 1 compared to the previous value
void	update_shllvl_value(t_shell *shell)
{
	char	*value_shlvl;
	int		level;

	value_shlvl = find_env_value("SHLVL", shell->env);
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
	if (!key || !value)
		return (NULL);
	new_node = malloc(sizeof(t_env));
	if (!new_node)
		return (NULL);
	new_node->key = key;
	new_node->value = value;
	new_node->next = NULL;
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
		error_env_exit(key, value, shell);
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

/*
logic
- create env
	- if no env, set basic
	- otherwise, actual env
	- change name of env and n of shllvl
- basic functions:
	- function to read env line
	- function to create and add node from inputs (would be useful for later work with env)
		- i want to use strdup directly there,
			so i dont need to do it manually. Can we assume that we will not create empty info nodes?
	- fucntion to exit if one of mallocs breaks
*/