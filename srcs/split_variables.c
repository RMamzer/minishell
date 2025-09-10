/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   split_variables.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/01 17:26:38 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/10 18:52:34 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	split_variables(t_shell *data)
{
	t_token	*current;
	char	**split_result;

	current = data->token_list;
	while (current)
	{
		if (current->type == WORD && !current->quoted && current->expanded)
		{
			split_result = ft_split_IFS(current->content, " \t\n");
			if (!split_result)
				fatality(ERROR_MEM, data, 1);
			process_split_result(data, current, split_result);
			free_split(split_result);
		}
		current = current->next;
	}
}

void	process_split_result(t_shell *data, t_token *current,
		char **split_result)
{
	int	str_qty;
	int	i;

	i = 0;
	str_qty = check_qty(split_result);
	if (str_qty == 0)
		replace_token_content(current, "", data, split_result);
	else if (str_qty == 1)
		replace_token_content(current, split_result[0], data, split_result);
	else
	{
		replace_token_content(current, split_result[i], data, split_result);
		i++;
		while (i < str_qty)
		{
			current = add_expanded_token(current, WORD, split_result[i]);
			if (!current)
			{
				free_split(split_result);
				fatality(ERROR_MEM, data, 1);
			}
			i++;
		}
	}
}

void	replace_token_content(t_token *current, char *new_content,
		t_shell *data, char **split_result)
{
	free(current->content);
	current->content = ft_strdup(new_content);
	if (!current->content)
	{
		free_split(split_result);
		fatality(ERROR_MEM, data, 1);
	}
}

t_token	*add_expanded_token(t_token *current, t_token_type type, char *content)
{
	t_token	*new_token;

	if (!current || !content)
		return (NULL);
	new_token = malloc(sizeof(t_token));
	if (!new_token)
		return (NULL);
	new_token->expanded = false;
	new_token->quoted = false;
	new_token->type = type;
	new_token->content = ft_strdup(content);
	if (!new_token->content)
	{
		free(new_token);
		return (NULL);
	}
	new_token->next = current->next;
	current->next = new_token;
	return (new_token);
}

void	free_split(char **split)
{
	int	i;

	i = 0;
	if (!split)
		return ;
	while (split[i])
	{
		free(split[i]);
		i++;
	}
	free(split);
}

int	check_qty(char **split_result)
{
	int	i;

	i = 0;
	if (!split_result)
		return (0);
	while (split_result[i])
		i++;
	return (i);
}
