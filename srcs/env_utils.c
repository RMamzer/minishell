/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_utils.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 15:13:52 by rmamzer           #+#    #+#             */
/*   Updated: 2025/10/08 15:14:12 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
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

