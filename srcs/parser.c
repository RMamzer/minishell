/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 18:05:25 by mklevero          #+#    #+#             */
/*   Updated: 2025/08/27 18:30:25 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	parse_tokens(t_shell *data)
{
	if (!data || !data->token_list)
		return (FAILURE);
	data->node = parse_pipe(&data->token_list);
	if (!data->node)
		return (FAILURE); // think here
	return (SUCCESS);
}

t_ast	*parse_pipe(t_token **token_list)
{
	t_ast	*node;
	t_ast	*pipe_node;
	t_token	*pipe_token;

	node = parse_redirection(token_list);
	// if (!node)
	// 	return (NULL); // think here
	if (*token_list && (*token_list)->type == PIPE)
	{
		pipe_node = add_ast_node(PIPE);
		pipe_node->left = node;
		pipe_token = *token_list;
		*token_list = (*token_list)->next;
		free(pipe_token->content);
		free(pipe_token);
		pipe_node->right = parse_pipe(token_list);
		return (pipe_node);
	}
	return (node);
}

t_ast	*parse_redirection(t_token **token_list)
{
	if (!*token_list)
		return (NULL);
	if ((*token_list)->type == WORD)
		return (parse_command_with_redirections(token_list));
	if ((*token_list)->type == IN || (*token_list)->type == OUT
		|| (*token_list)->type == APPEND || (*token_list)->type == HEREDOC)
		return (parse_single_redirection(token_list));
	return (NULL); // for now
}
t_ast	*parse_command_with_redirections(t_token **token_list)
{
	t_ast	*node;
	t_ast	*redirect_node;
	t_token	*redirect_token;
	t_token	*file_token;

	node = parse_command(token_list);
	while (*token_list && ((*token_list)->type == IN
			|| (*token_list)->type == OUT || (*token_list)->type == APPEND
			|| (*token_list)->type == HEREDOC))
	{
		redirect_node = add_ast_node((*token_list)->type);
		redirect_token = *token_list;
		file_token = (*token_list)->next;
		*token_list = file_token->next;
		redirect_node->right = add_file_node(file_token);
		free(redirect_token->content);
		free(redirect_token);
		redirect_node->left = node;
		node = redirect_node;
	}
	return (node);
}
t_ast	*parse_single_redirection(t_token **token_list)
{
	t_ast	*redirect_node;
	t_token	*redirect_token;
	t_token	*file_token;

	redirect_node = add_ast_node((*token_list)->type);
	redirect_token = *token_list;
	file_token = (*token_list)->next;
	*token_list = file_token->next;
	redirect_node->right = add_file_node(file_token);
	free(redirect_token->content);
	free(redirect_token);
	redirect_node->left = parse_redirection(token_list);
	return (redirect_node);
}

int	count_args(t_token *tokens)
{
	int	counter;

	counter = 0;
	while (tokens && tokens->type == WORD)
	{
		counter++;
		tokens = tokens->next;
	}
	return (counter);
}
void	load_args(t_ast *command_node, t_token **token_list, int ac)
{
	int		i;
	t_token	*temp;

	i = 0;
	while (i < ac)
	{
		command_node->value[i] = ft_strdup((*token_list)->content);
		if (command_node->value[i] == NULL)
		{
			// free the hell out of this array of strings;
			// free ast node
			// free token list
		}
		temp = *token_list;
		*token_list = (*token_list)->next;
		free(temp->content);
		free(temp);
		i++;
	}
	command_node->value[ac] = NULL;
}

t_ast	*parse_command(t_token **token_list)
{
	t_ast	*command_node;
	int		ac;

	command_node = add_ast_node((*token_list)->type);
	ac = count_args(*token_list);
	command_node->value = malloc(sizeof(char *) * (ac + 1));
	if (command_node->value == NULL)
	{
		// free this
		// free ast nodes
		// free token list
		// exit
	}
	load_args(command_node, token_list, ac);
	return (command_node);
}

t_ast	*add_file_node(t_token *token)
{
	t_ast	*file_node;

	file_node = add_ast_node(token->type);
	file_node->value = malloc(sizeof(char *) * 2);
	if (file_node == NULL)
	{
		free(file_node);
		return (NULL);
		// for now, probably we need another exit.
		// free ast nodes
		// free token list
		// exit
	}
	file_node->value[0] = ft_strdup(token->content);
	file_node->value[1] = NULL;
	if (file_node->value[0] == NULL)
	{
		free(file_node);
		return (NULL);
		// for now, probably we need another exit.
		// free ast nodes
		// free token list
		// exit
	}
	free(token->content);
	free(token);
	return (file_node);
}

t_ast	*add_ast_node(t_token_type type)
{
	t_ast	*node;

	node = malloc(sizeof(t_ast));
	if (node == NULL)
	{
		return (NULL);
		// free ast nodes
		// free token list
		// exit
	}
	node->type = type;
	node->value = NULL;
	node->expand = 0;
	node->status = 0;
	node->fd[0] = -1;
	node->fd[1] = -1;
	node->left = NULL;
	node->right = NULL;
	return (node);
}
