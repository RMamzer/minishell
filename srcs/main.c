/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 14:54:37 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/25 15:11:02 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// FOR MAXIM BELOW
// TODO: test var expansion. // echo $"PATH"
//	->PATH in bash vs dirty_shell> $"PATH"
//[TYPE: 3] "$PATH"
// TODO: new branch for stuff below.
// TODO: plan for heredoc(probably should be handeled before the expansion)
// TODO: remove quotes.
// TODO: AST situation.
// TODO: ERROR MANAGEMENT

// test function, remove later
void	test_tokens(t_token *list)
{
	while (list)
	{
		printf("[TYPE: %d] \"%s\"\n", list->type, list->content);
		list = list->next;
	}
}

void	test_env(t_env *envlist)
{
	while (envlist)
	{
		// printf("KEY: %s VALUE:%s \n", envlist->key, envlist->value);
		printf("%s=%s\n", envlist->key, envlist->value);
		envlist = envlist->next;
	}
}
// remove 
void    print_ast(t_ast *node, int depth)
{
    int i;

    if (!node)
        return;

    // indent for readability
    for (i = 0; i < depth; i++)
        printf("  ");

    // print node type
    if (node->type == PIPE)
        printf("NODE: PIPE\n");
    else if (node->type == IN)
        printf("NODE: IN\n");
    else if (node->type == OUT)
        printf("NODE: OUT\n");
    else if (node->type == APPEND)
        printf("NODE: APPEND\n");
    else if (node->type == HEREDOC)
        printf("NODE: HEREDOC\n");
    else if (node->type == WORD)
    {
        printf("NODE: CMD [");
        if (node->value)
        {
            char **args = (char **)node->value;
            while (*args)
            {
                printf("%s ", *args);
                args++;
            }
        }
        printf("]\n");
    }
    else
        printf("NODE: UNKNOWN\n");

    // recurse into left/right children
    if (node->left)
        print_ast(node->left, depth + 1);
    if (node->right)
        print_ast(node->right, depth + 1);
}
// remove 
void    free_ast(t_ast *node)
{
    char **args;
    int i;

    if (!node)
        return;

    // free left and right children first
    if (node->left)
        free_ast(node->left);
    if (node->right)
        free_ast(node->right);

    // free node->value (if it exists)
    if (node->value)
    {
        args = (char **)node->value;
        i = 0;
        while (args[i])
        {
            free(args[i]);    // free each strdup’ed string
            i++;
        }
        free(args);           // free the array itself
    }

    // finally free the node itself
    free(node);
}


int	main(int ac, char **av, char **env)
{
	t_shell	*data;
	char	*input;

	(void)ac; // addded for testing
	(void)av; // addded for testing
	data = init_data();
	create_env(data, env);
	test_env(data->env); // test
	while (1)            // addded for testing
	{
		input = readline("dirty_shell> ");
		if (!input)
			break ;
		if (input[0] != '\0')
			add_history(input);
		data->input_line = input;
		if (process_input(data->input_line, data) == SUCCESS)
		{
			test_tokens(data->token_list); // added for test
			//free_list(&data->token_list);  // added for test
		}
		if (parse_tokens(data->node) == SUCCESS)
		{
            print_ast(data->node, 0);
            free_ast(data->node);
            data->node = NULL;
		}
	}
}

bool	parse_tokens(t_shell *data)
{
	if (!data || !data->token_list)
		return (FAILURE);
	data->node = parse_pipe(&data->token_list);
	if (!data->node)
		return (FAILURE);
	return (SUCCESS);
}

t_ast	*parse_pipe(t_token **token_list)
{
	t_token	*pipe_token;
	t_token	*left_side;
	t_token	*right_side;
	t_ast	*pipe_node;
    
	pipe_token = find_pipe(*token_list);
	if (pipe_token)
	{
		pipe_node = add_ast_node(PIPE);
		left_side = *token_list;
		right_side = pipe_token->next;
		pipe_token->next = NULL;
		pipe_node->left = parse_redirection(&left_side);
		pipe_node->right = parse_pipe(&right_side);
		return (pipe_node);
	}
	return (parse_redirection(token_list));
}

t_ast	*parse_redirection(t_token **token_list)
{
	t_token	*temp;
    t_token *next_token;
	t_ast	*redirect_node;
    
	temp = *token_list;
	while (*token_list && (*token_list)->next)
	{
        next_token = (*token_list)->next;
		if ((*token_list)->next->type == IN || (*token_list)->next->type == OUT
			|| (*token_list)->next->type == APPEND || (*token_list)->next->type == HEREDOC)
		{
            redirect_node = add_ast_node((*token_list)->next->type);
            (*token_list)->next = next_token->next->next;
            redirect_node->left = parse_redirection(&temp);
            redirect_node->right = add_file_node(next_token->next);
            return (redirect_node);
		}
		*token_list = next_token;
	}
	return (parse_command(&temp));
}
t_ast   *parse_command(t_token **token_list)
{
    t_token *current;
    t_ast *command_node;
    int ac;
    int i;

    current = *token_list;
    ac = 0;
    i = 0;
    while (current && current->type == WORD)
    {
        ac++;
        current = current->next;
    }
    command_node = add_ast_node(WORD);
    command_node->value = malloc(sizeof(char *) * (ac + 1));
    if(command_node->value == NULL)
        return (NULL); // for now, probably we need another exit.
    current = *token_list;
    while(i < ac)
    {
        command_node->value[i] = ft_strdup(current->content);
        {
            if(command_node->value[i] == NULL)
            {
                // free the hell out of this array of strings;
                return (NULL); // for now, probably we need another exit.
            }
        }
        current = current->next;
        i++;
    }
    command_node->value[ac] = NULL;
    *token_list = current;
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
		return (NULL); // for now, probably we need another exit.
	}
	file_node->value[0] = ft_strdup(token->content);
	file_node->value[1] = NULL;
	if (file_node->value[0] == NULL)
	{
		free(file_node);
		return (NULL); // for now, probably we need another exit.
	}
	return (file_node);
}

t_token	*find_pipe(t_token *token_list)
{
	while (token_list)
	{
		if (token_list->type == PIPE)
			return (token_list);
		token_list = token_list->next;
	}
	return (NULL);
}

t_ast	*add_ast_node(t_token_type type)
{
	t_ast	*node;
	node = malloc(sizeof(t_ast));
	if (node == NULL)
		return (NULL); // probably need exit function
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
 

t_shell	*init_data(void)
{
	t_shell	*data;

	data = malloc(sizeof(t_shell));
	if (data == NULL)
	{
		ft_putendl_fd("malloc: Memory allocation failed", 2);
		exit(FAILURE);
	}
	data->exit_code = -1; // for now
	data->input_line = NULL;
	data->token_list = NULL;
	data->env = NULL;
	data->node = NULL;
	return (data);
}

/*
 *   Initial processing of an input line.
 *   TO_TRIM chars removed from both ends of the line.
 *   Checks validity of QUOTES
 *   Checks validity of PIPE
 *   Checks validity of REDIRECTIONS
 */
bool	process_input(char *input_line, t_shell *data)
{
	char	*line;

	line = ft_strtrim(input_line, TO_TRIM);
	if (line == NULL || line[0] == '\0')
	{
		free(data->input_line);
		free(line);
		return (FAILURE);
	}
	if (check_quote(line, ft_strlen(line)) != 0)
	{
		free(data->input_line);
		free(line);
		return (show_error("syntax error: unclosed quote", 258), FAILURE);
	}
	free(data->input_line);
	data->input_line = line;
	lexer(data->input_line, data);
	check_heredoc(data);
	if (check_syntax(data) == FAILURE)
	{
		printf("OSHIBKA V CHECK SYNTAX DETECTED\n");
		free_list(&data->token_list);
		return (FAILURE);
	}
	expander(data); // test
	return (SUCCESS);
}

/*
 *
 *   returns 0 if not inside quotes at position 'index',
 *   or returns the quote character (' or ") if inside a quote.
 */
char	check_quote(char *line, int index)
{
	int		i;
	char	quote;

	i = 0;
	quote = 0;
	while (i < index)
	{
		if (quote == 0 && (line[i] == '\'' || line[i] == '"'))
			quote = line[i];
		else if (quote != 0 && line[i] == quote)
			quote = 0;
		i++;
	}
	return (quote);
}

void	show_error(char *msg, int exit_code)
{
	(void)exit_code; // addded for testing
	ft_putendl_fd(msg, 2);
	// global exit status = exit_code
}
void	lexer_error(char *input_line, t_shell *data)
{
	free(input_line);
	clear_history();
	free_list(&data->token_list);
	ft_putendl_fd("malloc: Memory allocation failed", 2);
	data->exit_code = 1;
	exit(data->exit_code);
}
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

void	lexer(char *input_line, t_shell *data)
{
	size_t	i;

	i = 0;
	while (input_line[i])
	{
		while (is_delimiter(input_line[i]))
			i++;
		if (is_operator(input_line[i]) && check_quote(input_line, i) == 0)
			i += handle_operator(input_line, i, data);
		else
			i += handle_word(input_line, i, data);
	}
}

size_t	handle_word(char *input_line, size_t start, t_shell *data)
{
	size_t	i;
	size_t	word_start;
	size_t	len;
	char	*temp;

	i = start;
	word_start = start;
	while (input_line[i])
	{
		if (check_quote(input_line, i) == 0)
		{
			if (is_delimiter(input_line[i]) || is_operator(input_line[i]))
				break ;
		}
		i++;
	}
	len = i - word_start;
	temp = malloc(len + 1);
	if (temp == NULL)
		lexer_error(input_line, data);
	ft_strlcpy(temp, &input_line[word_start], len + 1);
	add_token(data, WORD, temp);
	free(temp);
	return (len);
}

/*
 *	Processes operator tokens.
 *	Adds the corresponding token to the token list.
 *	Returns the length of the operator token processed (1 or 2).
 */
size_t	handle_operator(char *input_line, size_t i, t_shell *data)
{
	if (input_line[i] == '|')
	{
		add_token(data, PIPE, "|");
		return (1);
	}
	if (input_line[i] == input_line[i + 1])
	{
		if (input_line[i] == '<')
			add_token(data, HEREDOC, "<<");
		else
			add_token(data, APPEND, ">>");
		return (2);
	}
	else
	{
		if (input_line[i] == '<')
			add_token(data, IN, "<");
		else
			add_token(data, OUT, ">");
		return (1);
	}
}

/*
 *	Creates a new token and appends it to the token list.
 *	If token list is NULL, adds token as head node.
 *	if token list is not NULL, adds token as the last one.
 */
void	add_token(t_shell *data, t_token_type type, char *content)
{
	t_token	*token;
	t_token	*temp;

	token = malloc(sizeof(t_token));
	if (token == NULL)
		lexer_error(data->input_line, data);
	token->type = type;
	token->next = NULL;
	token->content = ft_strdup(content);
	if (token->content == NULL)
	{
		free(token);
		lexer_error(data->input_line, data);
	}
	if (data->token_list == NULL)
		data->token_list = token;
	else
	{
		temp = data->token_list;
		while (temp->next)
			temp = temp->next;
		temp->next = token;
	}
}

bool	is_delimiter(int i)
{
	if (i == ' ' || i == '\n' || i == '\t')
		return (true);
	return (false);
}

bool	is_operator(char c)
{
	if (c == '|' || c == '<' || c == '>')
		return (true);
	return (false);
}

bool	check_syntax(t_shell *data)
{
	t_token	*current;

	if (data->token_list == NULL)
		return (SUCCESS);
	current = data->token_list;
	while (current)
	{
		if (current->type == PIPE)
		{
			if (current == data->token_list || current->next == NULL)
				return (show_error("syntax error near unexpected token '|'",
						258), FAILURE);
			if (current->next->type == PIPE)
				return (show_error("syntax error near unexpected token '|'",
						258), FAILURE);
		}
		if (current->type == IN || current->type == OUT
			|| current->type == APPEND || current->type == HEREDOC)
		{
			if (current->next == NULL || (current->next->type != WORD
					&& current->next->type != HEREDOC_DELIM_QT
					&& current->next->type != HEREDOC_DELIM_UQ))
				return (show_error("syntax error near unexpected token", 258),
					FAILURE);
		}
		current = current->next;
	}
	return (SUCCESS);
}

/**
 *  Expands all environment variables in the shell's token list.
 *  @param data Pointer to the shell struct containing tokens.
 */
void	expander(t_shell *data)
{
	t_token	*current;

	current = data->token_list;
	while (current)
	{
		if (current->type == WORD) //&& ft_strchr(current->content, '$')
			current->content = expand_content(current->content, data);
		current = current->next;
	}
}

/**
 * Expands environment variables, the $? variable,
 * and handels quotes in the string.
 * @param content The string to expand.
 * @param data Pointer to the shell struct(conteins env and exit code).
 * @return Newly allocated expanded string.
 */
char	*expand_content(char *content, t_shell *data)
{
	size_t	i;
	char	*new_content;
	char	*temp;

	i = 0;
	new_content = ft_strdup("");
	if (new_content == NULL)
		lexer_error(content, data);
	while (content[i])
	{
		temp = process_content(content, &i, data);
		if (temp == NULL)
		{
			free(new_content);
			lexer_error(content, data);
		}
		new_content = strjoin_free(new_content, temp);
		if (new_content == NULL)
			lexer_error(content, data);
	}
	free(content);
	return (new_content);
}

/**
 * Determens wether to handle a dollar variable or normal characters.
 * @param content The string being processed.
 * @param i Pointer to the current index in the string.
 * @param data Pointer to the shell struct.
 * @return Newly allocated string for the processed part.
 */
char	*process_content(char *content, size_t *i, t_shell *data)
{
	if (content[*i] == '\'')
		return (handle_single_quote(content, i));
	else if (content[*i] == '"')
		return (handle_double_quote(content, i, data));
	else if (content[*i] == '$')
		return (handle_dollar(content, i, data));
	else
		return (handle_characters(content, i, NO_QUOTE));
}

char	*handle_single_quote(char *content, size_t *i)
{
	size_t	start;
	char	*temp;

	(*i)++;
	start = *i;
	while (content[*i] && content[*i] != '\'')
		(*i)++;
	temp = ft_substr(content, start, *i - start);
	if (temp == NULL)
		return (NULL);
	if (content[*i] == '\'')
		(*i)++;
	return (temp);
}
char	*handle_double_quote(char *content, size_t *i, t_shell *data)
{
	char	*temp;
	char	*result;

	(*i)++;
	result = ft_strdup("");
	if (result == NULL)
		return (NULL);
	while (content[*i] && content[*i] != '"')
	{
		if (content[*i] == '$')
			temp = handle_dollar(content, i, data);
		else
			temp = handle_characters(content, i, IN_DOUBLE_QUOTE);
		if (temp == NULL)
		{
			free(result);
			return (NULL);
		}
		result = strjoin_free(result, temp);
	}
	if (content[*i] == '"')
		(*i)++;
	return (result);
}

/**
 * Expands a variable starting with '$'.
 * @param content The string being processed.
 * @param i Pointer to the index after '$' in the string.
 * @param data Pointer to the shell struct(conteins env and exit code).
 * @return Newly allocated string for the processed part.
 */
char	*handle_dollar(char *content, size_t *i, t_shell *data)
{
	char	*expanded;

	(*i)++;
	if (ft_isalpha(content[*i]) || content[*i] == '_')
	{
		expanded = expand_env_var(content, i, data->env);
		return (expanded);
	}
	else if (content[*i] == '?')
	{
		(*i)++;
		// or functon which updates exitcode idk yet
		expanded = ft_itoa(data->exit_code);
		return (expanded);
	}
	else
	{
		expanded = ft_strdup("$");
		return (expanded);
	}
}

/**
 * Reads consecutive non-variable characters.
 * @param content The string being processed.
 * @param i Pointer to the current index (updated to the end of read chars.)
 * @return Newly allocated string containing the characters, or NULL.
 */
char	*handle_characters(char *content, size_t *i, bool in_dq)
{
	size_t	start;
	char	*chars;

	start = *i;
	if (in_dq)
	{
		while (content[*i] && content[*i] != '$' && content[*i] != '"')
			(*i)++;
	}
	else
	{
		while (content[*i] && content[*i] != '$' && content[*i] != '\''
			&& content[*i] != '"')
			(*i)++;
	}
	chars = ft_substr(content, start, *i - start);
	if (chars == NULL)
		return (NULL);
	return (chars);
}

/**
 * Extracts a variable name and returns its value from the env.
 * @param content The string being processed.
 * @param i Pointer to the current index (updated to end of variable name).
 * @param env Pointer to th evironment linked list.
 * @return Newly allocated string with variabl's value, or NULL.
 */
char	*expand_env_var(char *content, size_t *i, t_env *env)
{
	size_t	start;
	char	*name;
	char	*value;

	start = *i;
	while (ft_isalnum(content[*i]) || content[*i] == '_')
		(*i)++;
	name = ft_substr(content, start, *i - start);
	if (name == NULL)
		return (NULL);
	value = get_env_value(name, env, ALLOC);
	if (value == NULL)
	{
		free(name);
		return (NULL);
	}
	free(name);
	return (value);
}
/**
 * Finds the value of an environment variable.
 * @param name Name of the environment variable.
 * @param env Pointer to the environment list.
 * @return Newly allocated string with value, or an empty string
 * if not found or NULL.
 */

// char	*get_env_value(char *name, t_env *env, bool alloc)
// {
// 	t_env	*current;
// 	char	*value;

// 	current = env;
// 	while (current)
// 	{
// 		if (ft_strcmp(current->key, name) == 0)
// 		{
// 			if (alloc == ALLOC)
// 			{
// 				value = ft_strdup(current->value);
// 				if (value == NULL)
// 					return (NULL);
// 				return (value);
// 			}
// 			else
// 				return (current->value);
// 		}
// 		current = current->next;
// 	}
// 	value = ft_strdup("");
// 	if (value == NULL)
// 		return (NULL);
// 	return (value);
// }

char	*get_env_value(char *name, t_env *env, bool alloc)
{
	t_env	*current;
	char	*value;

	current = env;
	while (current)
	{
		if (ft_strcmp(current->key, name) == 0)
		{
			if (alloc == NO_ALLOC)
				return (current->value);
			value = ft_strdup(current->value);
			if (value == NULL)
				return (NULL);
			return (value);
		}
		current = current->next;
	}
	if (alloc == NO_ALLOC)
		return (NULL);
	value = ft_strdup("");
	if (value == NULL)
		return (NULL);
	return (value);
}
/**
 * Joins two strings and frees the originals.
 * @param new_content First string to join (freed inside).
 * @param temp Second string to join (freed inside).
 * @return Newly allocated concatenated string or NULL.
 */
char	*strjoin_free(char *new_content, char *temp)
{
	char	*result;

	result = ft_strjoin(new_content, temp);
	free(new_content);
	free(temp);
	if (result == NULL)
		return (NULL);
	return (result);
}

int	ft_strcmp(const char *s1, const char *s2)
{
	size_t	i;

	i = 0;
	while (s1[i] != '\0' || s2[i] != '\0')
	{
		if ((unsigned char)s1[i] != (unsigned char)s2[i])
			return ((unsigned char)s1[i] - (unsigned char)s2[i]);
		i++;
	}
	return (0);
}
// maybe add count for heredocs here ?
void	check_heredoc(t_shell *data)
{
	t_token	*current;
	int		count;

	count = 0;
	current = data->token_list;
	while (current)
	{
		if (current->type == HEREDOC && current->next)
		{
			count++;
			if (current->next->content[0] == '\''
				|| current->next->content[0] == '"')
				current->next->type = HEREDOC_DELIM_QT;
			else
				current->next->type = HEREDOC_DELIM_UQ;
		}
		current = current->next;
	}
	if (count > 16)
		lexer_error("heredoc max count", data);
}
