/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_functions.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamzerr1 <mamzerr1@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 16:00:20 by rmamzer           #+#    #+#             */
/*   Updated: 2025/09/30 14:36:30 by mamzerr1         ###   ########.fr       */
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
	perror("minishell: malloc");
	if (key)
		free(key);
	if (value)
		free(value);
	free_shell(shell);
	exit (EXIT_FAILURE);
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
* Creates an environment node and add key and value to ir.
* @param key Pointer to a key of the environment line.
* @param envp Pointer to the value of the environment line.
* @return New malloced environment node.
*/
t_env	*create_env_node(char *key, char *value)
{
	t_env	*new_node;

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
	t_env	*new_node;

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
	new_node = create_env_node(key, value);
	if (!new_node)
		error_malloc_env_exit(key, value, shell);
	add_env_node(&shell->env, new_node);
}

/**
* Creates the minimal enviornment for shell if nothing is provided.
* @param shell Pointer to the shell struct.
* @return Void.
*/
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
	process_env_line(shell, pwd_line, ENV); // check if it leaks (set env env -i)
	process_env_line(shell, "SHLVL=0", ENV);
	process_env_line(shell, "_=/usr/bin/env",ENV);
	old_pwd_node = create_env_node(ft_strdup("OLDPWD"), NULL);
	if (!old_pwd_node)
		error_malloc_env_exit(NULL, NULL, shell);
	add_env_node(&shell->env, old_pwd_node);
}

/**
* Executes the environment creation for the shell.
* @param shell Pointer to the shell struct.
* @param envp Pointer to envp.
* @return Void.
*/
void	create_env(t_shell *shell, char **envp)
{
	if (!envp || !envp[0])
		set_minimal_env(shell);
	else
	{
		while (*envp)
		{
			process_env_line(shell, *envp, ENV);
			envp++;
		}
	}
	update_shllvl_value(shell); // check overflow lol?
}
