/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execute.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 15:58:25 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/06 18:16:21 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	execute_command(t_ast *node, t_shell *shell)
{
	// check builtins
	// otherwise, run established command
	// check other shells logic and figure out malloc
}


/*
external command:

*/

int execute_ast(t_ast *node, t_shell *shell)
{
	if (!node || !shell)
		return(0);
	if (node->type == PIPE)
		execute_pipe(node, shell);
	if (node->type == COMMAND)
		execute_command(node, shell);

	//check redicrection

	return (shell->exit_code);
}