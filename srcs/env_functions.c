/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_functions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 16:00:20 by rmamzer           #+#    #+#             */
/*   Updated: 2025/09/30 17:07:34 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
* Frees the mallocs used during env creation on failure of one and exits.
* @param key Pointer to the malloced key string.
* @param Value Pointer to the malloced key string.
* @param shell Pointer to the shell struct.
* @return Void.
*/
void	error_malloc_env_exit(char *key, char *value, t_shell *shell)
{
	if (key)
		free(key);
	if (value)
		free(value);
	fatality(ERROR_MEM, shell, 1);
}


/**
* Checks if the node with the provided key exists.
* @param env Pointer to the environment node.
* @return Void.
*/
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

/**
* Checks if the node with the provided key exist.
* @param env Pointer to the key that should be checked.
* @param env Pointer to the first node of environment list.
* @return Status of check (true or false).
*/
bool	check_env_key(char *key, t_env *env)
{
	t_env	*current;

	current = env;
	while (current)
	{
		if (ft_strcmp(current->key, key) == 0)
			return (true);
		current = current->next;
	}
	return (false);
}

/**
* Executes the removal of the environment node.
* @param env Pointer to the environmnt nodes list.
* @param key Pointer to the key of the node that should be removed.
* @return Void.
*/
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

/**
* Updates a value in env list based on provided key and new value.
* @param env Pointer to the environmnt nodes list.
* @param key Pointer to the key name that should be found.
* @param new_value Pointer to the new_value that should be used.
* @return Flag indicating the result of operation (success or failure).
*/
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

/**
* Updates shllvl value in environment. Increments it by 1 compared to the previous value
* @param shell Pointer to the shell struct.
* @return Void.
*/
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

/**
* Creates an environment node and adds key and value fro env line to it.
* @param env Pointer to an environment node list.
* @param new_node Pointer to the new environment node.
* @return New malloced environment node.
*/
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


/**
* Processes an environment node: checks if alloc of key and value was succesfull, creates
* an environment node, and adds it to the environment list. Can create bth nodes with
* allocated value or without value (NULL) depending on "value_alloc: flag
* @param key Pointer to a key of the environment line.
* @param value Pointer to the value of the environment line.
* @param value_alloc Flag indicating if value was malloced or NULL is inputed manually.
* @return Void.
*/
void	process_env_node(char *key, char *value, bool value_alloc, t_shell *shell)
{
	t_env	*new_node;

	if (!key || (value_alloc == ALLOC && !value))
		error_malloc_env_exit(key, value, shell);
	new_node = calloc(1, sizeof(t_env));
	if (!new_node)
		error_malloc_env_exit(key, value, shell);
	new_node->key = key;
	if (value)
	{
		new_node->value = value;
		new_node->assigned = true;
	}
	add_env_node(&shell->env, new_node);
}

/**
* Turns an envp line into a env node and adds it to other env nodes.
* Divides an environment line into a key and value that are stored in the node.
* @param shell Pointer to the shell struct.
* @param envp Pointer to the single string of envp.
* @param process Type of process being executed (ENV or EXPORT)
* @return Void.
*/
void	process_env_line(t_shell *shell, const char *envp, bool process)
{
	char	*key;
	char	*value;
	char	*equal;

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
	if (process == EXPORT && check_env_key(key, shell->env) == true)
	{
		update_env_value(&shell->env, key, value);
		free(key);
		return ;
	}
	process_env_node(key, value, ALLOC, shell);
}

/**
* Executes the environment creation for the shell. If no environment is provided,
* sets up a minimal environment
* @param shell Pointer to the shell struct.
* @param envp Pointer to envp.
* @return Void.
*/
void	create_env(t_shell *shell, char **envp)
{
	if (!envp || !envp[0])
	{
		process_env_node(ft_strdup("PWD"), getcwd(NULL, 0), ALLOC, shell);
		process_env_node(ft_strdup("SHLVL"), ft_strdup("0"), ALLOC, shell);
		process_env_node(ft_strdup("_"), ft_strdup("/usr/bin/env"), ALLOC, shell);
		process_env_node(ft_strdup("OLDPWD"), NULL, NO_ALLOC, shell);
	}
	else
	{
		while (*envp)
		{
			process_env_line(shell, *envp, ENV);
			envp++;
		}
	}
	update_shllvl_value(shell);
}
