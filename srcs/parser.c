/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 18:05:25 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/04 17:56:56 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool	parse_tokens(t_shell *shell)
{
	if (!shell || !shell->token_list)
		return (FAILURE);
	shell->ast = parse_pipe(&shell->token_list, shell);
	return (SUCCESS);
}

t_ast	*parse_pipe(t_token **token_list, t_shell *shell)
{
	t_ast	*node;
	t_ast	*pipe_node;
	t_token	*pipe_token;

	node = parse_redirection(token_list, shell);
	if (*token_list && (*token_list)->type == PIPE)
	{
		pipe_node = add_ast_node(PIPE, shell);
		pipe_node->left = node;
		pipe_token = *token_list;
		*token_list = (*token_list)->next;
		free(pipe_token->content);
		free(pipe_token);
		pipe_node->right = parse_pipe(token_list, shell);
		return (pipe_node);
	}
	return (node);
}

t_ast	*parse_redirection(t_token **token_list, t_shell *shell)
{
	if (!*token_list)
		return (NULL);
	if ((*token_list)->type == WORD)
		return (parse_command_with_redirections(token_list, shell));
	if ((*token_list)->type == IN || (*token_list)->type == OUT
		|| (*token_list)->type == APPEND || (*token_list)->type == HEREDOC)
		return (parse_single_redirection(token_list, shell));
	return (NULL); // for now
}

/*
t_ast	*parse_command_with_redirections(t_token **token_list, t_shell *shell)
{
	t_ast	*node;
	t_ast	*redirect_node;
	t_token	*redirect_token;
	t_token	*file_token;

	node = parse_command(token_list, shell);
	while (*token_list && ((*token_list)->type == IN
			|| (*token_list)->type == OUT || (*token_list)->type == APPEND
			|| (*token_list)->type == HEREDOC))
	{
		if (!validate_redirection(*token_list))
			fatality(NULL, shell, 2);
		redirect_node = add_ast_node((*token_list)->type, shell);
		redirect_token = *token_list;
		file_token = (*token_list)->next;
		*token_list = file_token->next;
		redirect_node->right = add_file_node(file_token, shell);
		free(redirect_token->content);
		free(redirect_token);
		redirect_node->left = node;
		node = redirect_node;
	}
	return (node);
}
t_ast	*parse_single_redirection(t_token **token_list, t_shell *shell)
{
	t_ast	*redirect_node;
	t_token	*redirect_token;
	t_token	*file_token;

	if (!validate_redirection(*token_list))
		fatality(NULL, shell, 2);
	redirect_node = add_ast_node((*token_list)->type, shell);
	redirect_token = *token_list;
	file_token = (*token_list)->next;
	*token_list = file_token->next;
	redirect_node->right = add_file_node(file_token, shell);
	free(redirect_token->content);
	free(redirect_token);
	redirect_node->left = parse_redirection(token_list, shell);
	return (redirect_node);
}
*/
t_ast	*parse_command_with_redirections(t_token **token_list, t_shell *shell)
{
	t_ast	*node;
	t_ast	*redirect_node;
	t_token	*redirect_token;
	t_token	*file_token;
	t_ast	*root;
	t_ast	*last;
	t_ast	*root;
	t_ast	*last;

	root = NULL;
	last = NULL;
	root = NULL;
	last = NULL;
	node = parse_command(token_list, shell);
	while (*token_list && ((*token_list)->type == IN
			|| (*token_list)->type == OUT || (*token_list)->type == APPEND
			|| (*token_list)->type == HEREDOC))
	{
		if (!validate_redirection(*token_list))
			fatality(NULL, shell, 2);
		redirect_node = add_ast_node((*token_list)->type, shell);
		redirect_token = *token_list;
		file_token = (*token_list)->next;
		*token_list = file_token->next;
		redirect_node->right = add_file_node(file_token, shell);
		free(redirect_token->content);
		free(redirect_token);
		if (!root)
		{
			root = redirect_node;
			last = redirect_node;
		}
		else
		{
			last->left = redirect_node;
			last = redirect_node;
		}
		if (!root)
		{
			root = redirect_node;
			last = redirect_node;
		}
		else
		{
			last->left = redirect_node;
			last = redirect_node;
		}
	}
	if (last)
		last->left = node;
	if (root)
		return (root);
	if (last)
		last->left = node;
	if (root)
		return (root);
	return (node);
}
t_ast	*parse_single_redirection(t_token **token_list, t_shell *shell)
{
	t_ast	*redirect_node;
	t_token	*redirect_token;
	t_token	*file_token;
	t_ast	*root;
	t_ast	*last;
	t_ast	*root;
	t_ast	*last;

	root = NULL;
	last = NULL;
	while (*token_list && ((*token_list)->type == IN
			|| (*token_list)->type == OUT || (*token_list)->type == APPEND
			|| (*token_list)->type == HEREDOC))
	{
		if (!validate_redirection(*token_list))
			fatality(NULL, shell, 2);
		redirect_node = add_ast_node((*token_list)->type, shell);
		redirect_token = *token_list;
		file_token = (*token_list)->next;
		*token_list = file_token->next;
		redirect_node->right = add_file_node(file_token, shell);
		free(redirect_token->content);
		free(redirect_token);
		if (!root)
		{
			root = redirect_node;
			last = redirect_node;
		}
		else
		{
			last->left = redirect_node;
			last = redirect_node;
		}
	}
	return (root);
	root = NULL;
	last = NULL;
	while (*token_list && ((*token_list)->type == IN
			|| (*token_list)->type == OUT || (*token_list)->type == APPEND
			|| (*token_list)->type == HEREDOC))
	{
		if (!validate_redirection(*token_list))
			fatality(NULL, shell, 2);
		redirect_node = add_ast_node((*token_list)->type, shell);
		redirect_token = *token_list;
		file_token = (*token_list)->next;
		*token_list = file_token->next;
		redirect_node->right = add_file_node(file_token, shell);
		free(redirect_token->content);
		free(redirect_token);
		if (!root)
		{
			root = redirect_node;
			last = redirect_node;
		}
		else
		{
			last->left = redirect_node;
			last = redirect_node;
		}
	}
	return (root);
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
void	load_args(t_ast *command_node, t_token **token_list, int ac,
		t_shell *shell)
{
	int		i;
	t_token	*temp;

	command_node->value = ft_calloc(ac + 1, sizeof(char *));
	if (!command_node->value)
		fatality("malloc: Memory allocation failed", shell, 1);
	i = 0;
	while (i < ac)
	{
		command_node->value[i] = ft_strdup((*token_list)->content);
		if (!command_node->value[i])
			fatality("malloc: Memory allocation failed", shell, 1);
		temp = *token_list;
		*token_list = (*token_list)->next;
		free(temp->content);
		free(temp);
		i++;
	}
}

t_ast	*parse_command(t_token **token_list, t_shell *shell)
{
	t_ast	*command_node;
	int		ac;

	command_node = add_ast_node((*token_list)->type, shell);
	ac = count_args(*token_list);
	load_args(command_node, token_list, ac, shell);
	return (command_node);
}

t_ast	*add_file_node(t_token *token, t_shell *shell)
{
	t_ast	*file_node;

	file_node = add_ast_node(token->type, shell);
	file_node->value = ft_calloc(2, sizeof(char *));
	if (file_node == NULL)
		fatality("malloc: Memory allocation failed", shell, 1);
	file_node->value[0] = ft_strdup(token->content);
	if (file_node->value[0] == NULL)
		fatality("malloc: Memory allocation failed", shell, 1);
	free(token->content);
	free(token);
	return (file_node);
}

t_ast	*add_ast_node(t_token_type type, t_shell *shell)
{
	t_ast	*node;

	node = ft_calloc(1, sizeof(t_ast));
	if (!node)
		fatality("malloc: Memory allocation failed", shell, 1);
	node->type = type;
	node->fd[0] = -1;
	node->fd[1] = -1;
	return (node);
}

// free token list
void	free_list(t_token **list)
{
	t_token	*temp;

	if (!list)
		return ;
	while (*list)
	{
		temp = *list;
		*list = (*list)->next;
		free(temp->content);
		temp->content = NULL;
		free(temp);
		temp = NULL;
	}
	*list = NULL;
}
// free ast
void	free_ast(t_ast **ast)
{
	t_ast	*temp;

	if (!ast || !*ast)
		return ;
	temp = *ast;
	free_ast(&temp->left);
	free_ast(&temp->right);
	free_array(temp->value);
	free(temp);
	*ast = NULL;
}
// free shell shell
void	free_shell_data(t_shell *shell)
{
	if (!shell)
		return ;
	free(shell->input_line);
	shell->input_line = NULL;
	free_list(&shell->token_list);
	free_ast(&shell->ast);
	// probably free_env
}

void	free_shell(t_shell *shell)
{
	if (!shell)
		return ;
	free_shell_data(shell);
	free(shell);
}

// free array of strings
void	free_array(char **arr)
{
	int	i;

	i = 0;
	if (!arr)
		return ;
	while (arr[i])
		free(arr[i++]);
	free(arr);
}

void	fatality(char *msg, t_shell *shell, int exit_code)
{
	if (msg)
		ft_putendl_fd(msg, 2);
	if (shell)
		free_shell(shell);
	clear_history();
	exit(exit_code);
}
