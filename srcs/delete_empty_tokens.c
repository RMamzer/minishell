/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   delete_empty_tokens.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/30 15:02:34 by mklevero          #+#    #+#             */
/*   Updated: 2025/10/13 16:19:09 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Removes empty tokens from the token list.
 * Empty tokens can result from variable expansion to empty strings.
 * Only removes unquoted empty WORD tokens.
 * This ensures redundant empty arguments are not passed to parsing.
 *
 * @param shell Pointer to the shell structure containing token list
 */
void	delete_empty_tokens(t_shell *shell)
{
	t_token	*curr;
	t_token	*prev;
	t_token	*temp;

	curr = shell->token_list;
	prev = NULL;
	while (curr)
	{
		if (curr->type == WORD && !curr->quoted && curr->content[0] == '\0')
		{
			temp = curr;
			if (prev == NULL)
				shell->token_list = curr->next;
			else
				prev->next = curr->next;
			curr = curr->next;
			free(temp->content);
			free(temp);
		}
		else
		{
			prev = curr;
			curr = curr->next;
		}
	}
}
