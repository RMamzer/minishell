/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/27 18:05:25 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/09 13:40:32 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

bool    syntax_confirmed(t_token *token_list, t_shell *shell)
{
    t_token *current;

    current = token_list;
    while(current)
    {
        if(is_redir(current->type) && !validate_redirection(current))
        {
            show_error(NULL, shell, 2);
            return (FAILURE);
        }
        current = current->next;
    }
    return (SUCCESS);
}

bool	parse_tokens(t_shell *shell)
{
	if (!shell || !shell->token_list)
		return (FAILURE);
    
    if (syntax_confirmed(shell->token_list, shell) == FAILURE)
        return (FAILURE);
    
	shell->ast = parse_pipe(&shell->token_list, shell);
    
	return (SUCCESS);
}

t_ast	*parse_pipe(t_token **token_list, t_shell *shell)
{
	t_ast	*node;
	t_ast	*pipe_node;
	t_token	*pipe_token;

	node = parse_command_and_redirection(token_list, shell);
	if (*token_list && (*token_list)->type == PIPE)
	{
		pipe_node = add_ast_node(PIPE, shell);
		pipe_token = *token_list;
		*token_list = (*token_list)->next;
		free(pipe_token->content);
		free(pipe_token);
		pipe_node->left = node;
		pipe_node->right = parse_pipe(token_list, shell);
		return (pipe_node);
	}
	return (node);
}
bool	is_redir(t_token_type type)
{
	return (type == IN || type == OUT || type == APPEND || type == HEREDOC);
}
t_ast	*parse_command_and_redirection(t_token **token_list, t_shell *shell)
{
	t_ast	*root;
	t_ast	*tail;
	t_ast	*cmd_node;

	root = NULL;
	tail = NULL;
	cmd_node = NULL;
	while (*token_list && (is_redir((*token_list)->type)
			|| (*token_list)->type == WORD))
	{
		if ((*token_list)->type == WORD)
			cmd_node = handle_word_ast(token_list, cmd_node, shell);
		else
			handle_redir_ast(token_list, &root, &tail, shell);
	}
	if (tail)
	{
		tail->left = cmd_node;
		return (root);
	}
	return (cmd_node);
}

t_ast	*handle_word_ast(t_token **token_list, t_ast *cmd_node, t_shell *shell)
{
	if (!cmd_node)
		cmd_node = add_ast_node((*token_list)->type, shell);
	append_arg(cmd_node, (*token_list)->content, shell);
	move_and_free(token_list);
	return (cmd_node);
}

t_ast	*handle_redir_ast(t_token **token_list, t_ast **root, t_ast **tail,
		t_shell *shell)
{
	t_token	*redir_token;
	t_token	*file_token;
	t_ast	*node;
	t_token	*next;

	redir_token = *token_list;
	file_token = redir_token->next;
	next = file_token->next;
	node = add_ast_node(redir_token->type, shell);
	node->right = add_file_node(file_token, shell);
	*token_list = next;
	free(redir_token->content);
	free(redir_token);
	if (!*root)
	{
		*root = node;
		*tail = node;
	}
	else
	{
		(*tail)->left = node;
		*tail = node;
	}
	return (node);
}

void	append_arg(t_ast *cmd_node, const char *str, t_shell *shell)
{
	char	**new;
	size_t	i;

	i = 0;
	if (!cmd_node->value)
		return (init_arg(cmd_node, str, shell));
	while (cmd_node->value[i])
		i++;
	new = load_arg(cmd_node->value, i, shell);
	new[i] = ft_strdup(str);
	if (!new[i])
		fatality(ERROR_MEM, shell, 1);
	new[i + 1] = NULL;
	cmd_node->value = new;
}
void	init_arg(t_ast *cmd_node, const char *str, t_shell *shell)
{
	cmd_node->value = ft_calloc(2, sizeof(char *));
	if (!cmd_node->value)
		fatality(ERROR_MEM, shell, 1);
	cmd_node->value[0] = ft_strdup(str);
	if (!cmd_node->value[0])
		fatality(ERROR_MEM, shell, 1);
	cmd_node->value[1] = NULL;
}
char	**load_arg(char **old, size_t count, t_shell *shell)
{
	char	**new;
	size_t	i;

	i = 0;
	new = ft_calloc(count + 2, sizeof(char *));
	if (!new)
		fatality(ERROR_MEM, shell, 1);
	while (i < count)
	{
		new[i] = old[i];
		i++;
	}
	free(old);
	return (new);
}

t_ast	*add_file_node(t_token *token, t_shell *shell)
{
	t_ast	*file_node;

	file_node = add_ast_node(token->type, shell);
	file_node->value = ft_calloc(2, sizeof(char *));
	if (file_node == NULL)
		fatality(ERROR_MEM, shell, 1);
	file_node->value[0] = ft_strdup(token->content);
	if (file_node->value[0] == NULL)
		fatality(ERROR_MEM, shell, 1);
	free(token->content);
	free(token);
	return (file_node);
}

t_ast	*add_ast_node(t_token_type type, t_shell *shell)
{
	t_ast	*node;

	node = ft_calloc(1, sizeof(t_ast));
	if (!node)
		fatality(ERROR_MEM, shell, 1);
	node->type = type;
	node->fd[0] = -1;
	node->fd[1] = -1;
	return (node);
}

void	move_and_free(t_token **token_list)
{
	t_token	*temp;

	if (!token_list || !*token_list)
		return ;
	temp = *token_list;
	*token_list = (*token_list)->next;
	free(temp->content);
	free(temp);
}

bool	validate_redirection(t_token *redirection)
{
	t_token	*token;

	token = redirection->next;
	if (!redirection || !redirection->next)
	{
		ft_putendl_fd("minishell: syntax error near unexpected token `newline'",
			2);
		return (false);
	}
	if (token->type != WORD)
	{
		ft_putstr_fd("minishell: syntax error near unexpected token `", 2);
		ft_putstr_fd(token->content, 2);
		ft_putendl_fd("'", 2);
		return (false);
	}
	if (token->expanded && token->content[0] == '\0')
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(token->content, 2);
		ft_putendl_fd(": ambiguous redirect", 2);
		return (false);
	}
	if (token->expanded && token->next && token->next->type == WORD)
	{
		ft_putstr_fd("minishell: ", 2);
		ft_putstr_fd(token->content, 2);
		ft_putendl_fd(": ambiguous redirect", 2);
		return (false);
	}
	return (true);
}

/*
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
*/

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
/*
t_ast	*parse_command_with_redirections(t_token **token_list, t_shell *shell)
{
	t_ast	*node;
	t_ast	*redirect_node;
	t_token	*redirect_token;
	t_token	*file_token;
	t_ast	*root;
	t_ast	*last;

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
	}
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
*/
