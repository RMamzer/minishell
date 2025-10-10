/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/08 22:09:45 by rmamzer           #+#    #+#             */
/*   Updated: 2025/10/10 16:11:09 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Outputs a custom error message into STDERROR
 * with or without perror message.
 * @param str1 Pointer to the first sting that should be otputted.
 * @param str2 Pointer to the second sting that should be otputted.
 * @param str3 Pointer to the third sting that should be otputted.
 * @param str4 Pointer to a sting that should be outputted with perror.
 * @return Void.
 */
void	write_bulitin_error(char *str1, char *str2, char *str3, char *str4)
{
	if (str1)
		ft_putstr_fd(str1, STDERR_FILENO);
	if (str2)
		ft_putstr_fd(str2, STDERR_FILENO);
	if (str3)
		ft_putstr_fd(str3, STDERR_FILENO);
	if (str4)
		perror(str4);
}

/**
 * Calculates the number of arguments in the char string.
 * @param args Array of arguments of type char.
 * @return Length of array of arguments.
 */
int	get_args_len(char **args)
{
	int	len;

	len = 0;
	while (args[len] != NULL)
		len++;
	return (len);
}

/**
 * Joins up to 3 strings in single malloc returns a pointer to new string.
 * @param s1 Pointer to the first sting that should be joined.
 * @param s2 Pointer to the second sting that should be joined.
 * @param s3 Pointer to a third sting that should be joined.
 * @return New malloced  joined sting.
 */
char	*super_strjoin(char const *s1, char const *s2, char const *s3)
{
	char	*joinedstr;
	size_t	str1_l;
	size_t	str2_l;
	size_t	str3_l;

	if (!s1 && !s2 && !s3)
		return (NULL);
	str1_l = ft_strlen(s1);
	str2_l = ft_strlen(s2);
	str3_l = ft_strlen(s3);
	joinedstr = malloc(str1_l + str2_l + str3_l + 1);
	if (!joinedstr)
		return (NULL);
	ft_memcpy(joinedstr, s1, str1_l);
	ft_memcpy(joinedstr + str1_l, s2, str2_l);
	ft_memcpy(joinedstr + str1_l + str2_l, s3, str3_l);
	joinedstr[str1_l + str2_l + str3_l] = '\0';
	return (joinedstr);
}

/**
 * Calculates the number of relevant nodes in the environment list.
 * @param lst Pointer to the first node of environment list.
 * @param process Type of process being executed (EXECUTE or EXPORT)
 * @return Number of the relevant nodes in environment list.
 */
int	get_var_num(t_env *lst, bool process)
{
	t_env	*tmp;
	int		i;

	i = 0;
	tmp = lst;
	while (tmp)
	{
		if (process == EXPORT || tmp->assigned == true)
			i++;
		tmp = tmp->next;
	}
	return (i);
}

/**
 * Waits for the execution of pipe child processes to collect final
 * exit status.
 * @param pid  Id of the child process executing external cmd.
 * @param shell Pointer to the shell struct.
 * @return exit status of the child process executing cmd.
 */
int	wait_child(pid_t pid, t_shell *shell)
{
	int		status;
	pid_t	term_pid;
	int		exit_code;

	if (shell->is_parent == true)
		set_exec_parent_signals();
	exit_code = EXIT_FAILURE;
	term_pid = waitpid(pid, &status, 0);
	if (term_pid == -1)
		return (write_error_and_return("waitpd", EXIT_FAILURE));
	if (WIFEXITED(status))
		exit_code = WEXITSTATUS(status);
	else if (WIFSIGNALED(status))
		exit_code = 128 + WTERMSIG(status);
	return (exit_code);
}
