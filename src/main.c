/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 14:54:37 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/06 18:30:33 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	main(int ac, char **av, char **env)
{
	char	*input;
	t_shell	*data;

	input = NULL;
	data = init_data();
	process_input(input, data);
}
t_shell	*init_data(void)
{
	t_shell	*data;

	data = malloc(sizeof(t_shell));
	if (data == NULL)
		return (NULL);    // for now null
	data->exit_code = -1; // for now
}
bool	process_input(char *input, t_shell *data)
{
	char	*trimmed_input;

	trimmed_input = ft_strtrim(input, TO_TRIM);
	if (trimmed_input == NULL)
		return (NULL); // for now
	free(input);
	lexer(trimmed_input, data);
}

void	lexer(char *trimmed_input, t_shell *data)
{
	// loop through the input
	// find smth
	// extract
	// create node, add type, add to list
}