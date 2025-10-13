/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export_utils.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 15:26:04 by rmamzer           #+#    #+#             */
/*   Updated: 2025/10/13 16:18:50 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Adds a key-only environment variable (e.g., `export KEY`) to the
 * environment list if it is not already present.
 *
 * @param shell Pointer to the shell struct.
 * @param str The variable name (key) to be added.
 * @return Void.
 */
void	process_valueless_export_node(t_shell *shell, char *str)
{
	char	*key;

	if (check_env_key(str, shell->env) == true)
		return ;
	key = ft_strdup(str);
	if (!key)
		fatality(ERROR_MEM, shell, 1);
	process_env_node(key, NULL, NO_ALLOC, shell);
}

/**
 * Verifies if the input string is a valid shell identifier for environment
 * variable naming. Must start with a letter or underscore, cannot start
 * with a digit, and can optionally contain a single '=' as a separator.
 *
 * @param str The string to validate.
 * @return True if the identifier is valid, false otherwise.
 */
bool	is_identifier_valid(char *str)
{
	if (!str || *str == '\0' || ft_isdigit(*str) || *str == '=')
		return (false);
	while (*str && *str != '=')
	{
		if (!ft_isalnum(*str) && *str != '_')
			return (false);
		str++;
	}
	return (true);
}
