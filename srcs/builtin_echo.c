/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mamzerr1 <mamzerr1@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/18 15:37:48 by rmamzer           #+#    #+#             */
/*   Updated: 2025/09/19 11:34:51 by mamzerr1         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	execute_builtin_echo(char **args)
{
	bool	n_flag;
	int		i;

	i = 0;
	n_flag = false;
	if (*args)
	{
		while (ft_strcmp("-n", args[i]) == 0)
		{
			n_flag = true;
			i++;
		}
		while (args[i])
		{
			ft_putstr_fd(args[i], STDOUT_FILENO);
			i++;
			if (args[i])
				ft_putchar_fd(' ', STDOUT_FILENO);
		}
	}
	if (n_flag == false)
		ft_putchar_fd('\n', STDOUT_FILENO);
	return (0);
}
