/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/24 17:38:22 by rmamzer           #+#    #+#             */
/*   Updated: 2025/10/13 16:18:47 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Outputs error message if exit function fails
 * @param nptr The exit code argument string.
 * @return Exit status of failure.
 */
int	exit_numeric_error(char *nptr)
{
	ft_putstr_fd("minishell: exit: ", STDERR_FILENO);
	ft_putstr_fd(nptr, STDERR_FILENO);
	ft_putstr_fd(": numeric argument required\n", STDERR_FILENO);
	return (EXIT_INVALID_OPTION);
}

/**
 * Converts a string to a unsigned long long integer, checking for overflow,
	and then
 * returns the result % 256 for the exit status.
 * @param nptr The exit code argument string.
 * @return The calculated exit status (0-255),
	or 255 if an overflow is detected.
 */
int	process_exit_num(char *nptr)
{
	int					sign;
	int					i;
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
		if ((num > LLONG_MAX && sign == 1)
			|| (num > (unsigned long long)LLONG_MAX + 1 && sign == -1))
			return (exit_numeric_error(nptr));
		i++;
	}
	return ((int)(num * sign % 256));
}

/**
 * Validates the 'exit' command code passed.
 * @param nptr The argument string to check.
 * @return The calculated exit status (0-255) on success, or 255 if non-numeric
 * characters are found.
 */
int	check_exit_code(char *nptr)
{
	int	i;

	i = 0;
	while ((nptr[i] >= 9 && nptr[i] <= 13) || nptr[i] == 32)
		i++;
	if (nptr[i] == '-' || nptr[i] == '+')
		i++;
	while (ft_isdigit(nptr[i]))
		i++;
	while ((nptr[i] >= 9 && nptr[i] <= 13) || nptr[i] == 32)
		i++;
	if (nptr[i] != '\0')
		return (exit_numeric_error(nptr));
	return (process_exit_num(nptr));
}

/**
 * Executes the built-in 'exit' command. It handles argument validation,
 * sets the shell's exit code, and terminates the shell process if executed
 * in the parent.
 *
 * @param args Arguments passed to the command (max one).
 * @param shell Pointer to the shell struct.
 * @return Exit status of minishell or passed exit value.
 */
int	execute_builtin_exit(char **args, t_shell *shell)
{
	if (get_args_len(args) > 1)
	{
		ft_putstr_fd("minishell: exit: too many arguments\n", STDERR_FILENO);
		return (EXIT_FAILURE);
	}
	if (*args)
		shell->exit_code = check_exit_code(args[0]);
	if (shell->is_parent == true)
	{
		write(STDOUT_FILENO, "exit\n", 5);
		fatality(NULL, shell, shell->exit_code);
	}
	return (shell->exit_code);
}
