/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_creation.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/13 16:00:20 by rmamzer           #+#    #+#             */
/*   Updated: 2025/10/13 16:19:11 by mklevero         ###   ########.fr       */
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
 * Updates shllvl value in environment. Increments it by 1 compared to the
 * previous value.
 *
 * @param shell Pointer to the shell struct
 * @return Void
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
		free(value_shlvl);
}

/**
 * Processes an environment node: checks if alloc of key and value was
 * succesfull, creates an environment node, and adds it to the environment list.
 * Can create bth nodes with allocated value or without value (NULL)
 * depending on "value_alloc: flag
 *
 * @param key Pointer to a key of the environment line
 * @param value Pointer to the value of the environment line
 * @param value_alloc Flag indicating if value was malloced or NULL is inputed
 * @return Void
 */
void	process_env_node(char *key, char *value, bool value_alloc,
		t_shell *shell)
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
 * @param shell Pointer to the shell struct
 * @param envp Pointer to the single string of envp
 * @param process Type of process being executed (ENV or EXPORT)
 * @return Void
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
 * Executes the environment creation for the shell. If no environment is
 * provided, sets up a minimal environment.
 *
 * @param shell Pointer to the shell struct
 * @param envp Pointer to envp
 * @return Void
 */
void	create_env(t_shell *shell, char **envp)
{
	if (!envp || !envp[0])
	{
		process_env_node(ft_strdup("PWD"), getcwd(NULL, 0), ALLOC, shell);
		process_env_node(ft_strdup("SHLVL"), ft_strdup("0"), ALLOC, shell);
		process_env_node(ft_strdup("_"), ft_strdup("/usr/bin/env"), ALLOC,
			shell);
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
