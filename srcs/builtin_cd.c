/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/20 17:49:04 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/20 18:13:25 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

execute_builtin_pwd(char	**args , t_shell *shell)
{
	// if (!*args)
	// 	return(go_home_path);

	if (args[0][0] == '-')// undefined behavior? do we want to implement -?
	{
		ft_putstr_fd("minishell: cd:", STDERR_FILENO);
		ft_putstr_fd(args[0], STDERR_FILENO);
		ft_putstr_fd(":invalid option", STDERR_FILENO);
		return(EXIT_INVALID_OPTION);
	}

}