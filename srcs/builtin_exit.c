/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/24 17:38:22 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/31 23:55:32 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	exit_numeric_error(char *nptr)
{
	ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
	ft_putstr_fd(nptr, STDERR_FILENO);
	ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
	return(EXIT_INVALID_OPTION);
}


int	process_exit_num(char *nptr)
{
	int		sign;
	int		i;
	unsigned long long	num;

	sign = 1;
	i = 0;
	num = 0;
	while ((nptr[i] >= 9 && nptr[i] <= 13) || nptr[i] == 32)
		i++;
	if (nptr[i] == '-' || nptr[i] == '+')
	{
		if (nptr[i] == '-')
			sign *= -1;
		i++;
	}
	while (nptr[i] >= '0' && nptr[i] <= '9')
	{
		num = num * 10 + (nptr[i] - '0');
		if ((num >LLONG_MAX && sign == 1) || (num > (unsigned long long)LLONG_MAX +1 && sign == -1))
			return (exit_numeric_error(nptr));
		i++;
	}
		return ((int)(num * sign % 256));
}

int	check_exit_code(char *nptr)
{
	int i;

	i = 0;
	while ((nptr[i] >= 9 && nptr[i] <= 13) || nptr[i] == 32)
		i++;
	if (nptr[i] == '-' || nptr[i] == '+')
		i++;
	while(ft_isdigit(nptr[i]))
		i++;
	while ((nptr[i] >= 9 && nptr[i] <= 13) || nptr[i] == 32)
		i++;
	if (nptr[i] != '\0')
		return(exit_numeric_error(nptr));
	return (process_exit_num(nptr));
}


int execute_builtin_exit(char **args, t_shell *shell)
{
	if (get_args_len(args) > 1)
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	if (*args)
		shell->exit_code = check_exit_code(args[0]);
	//free_minishell_here
	exit(shell->exit_code);
}