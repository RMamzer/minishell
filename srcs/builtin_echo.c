/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 15:37:48 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/20 15:30:38 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute_builtin_echo(char	**args)
{
	bool	n_flag;
	int i;

	i = 0;
	n_flag = false;
	while (ft_strcmp("-n",args[i]) == 0)
	{
		n_flag = true;
		i++;
	}
	while(args[i])
	{
		ft_putstr_fd(args[i],STDOUT_FILENO);
		i++;
		if (args[i])
			ft_putchar_fd(' ', STDOUT_FILENO);
	}
	if (n_flag == false)
		ft_putchar_fd('\n', STDOUT_FILENO);
	return (0);
}
