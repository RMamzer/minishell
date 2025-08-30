/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 14:54:37 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/30 14:51:12 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// in singl qt does not work.

// TODO: var expansion done but recheck the flow and free, etc.
// TODO: heredocsituation, remove qotes,

// test function, remove later
// void	test_tokens(t_token *list)
// {
// 	while (list)
// 	{
// 		printf("[TYPE: %d] \"%s\"\n", list->type, list->content);
// 		list = list->next;
// 	}
// }

// void	test_env(t_env *envlist)
// {
// 	while (envlist)
// 	{
// 		// printf("KEY: %s VALUE:%s \n", envlist->key, envlist->value);
// 		printf("%s=%s\n", envlist->key, envlist->value);
// 		envlist = envlist->next;
// 	}
// }



// int	main(int ac, char **av, char **env)
// {
// 	t_shell	*data;
// 	char	*input;

// 	// TESTING-------------------------------
// 	t_ast	*node; // added for  EXEC test

// 	node = calloc(1, sizeof(t_ast));
// 	node->type = WORD;
// 	node->value[0] = "ls";
// 	// TESTING-----------------------------

// 	(void)ac; // addded for testing
// 	(void)av; // addded for testing
// 	data = init_data();
// 	create_env(data, env);
// 	test_env(data->env); // test
// 	while (1)            // addded for testing
// 	{
// 		input = readline("dirty_shell> ");
// 		if (!input)
// 			break ;
// 		if (input[0] != '\0')
// 			add_history(input);
// 		data->input_line = input;
// 		if (process_input(data->input_line, data) == SUCCESS)
// 		{
// 			test_tokens(data->token_list); // added for test
// 			free_list(&data->token_list);  // added for test
// 		}
// 		execute_ast(node, data);

// 	}
// }
// t_shell	*init_data(void)
// {
// 	t_shell	*data;

// 	data = malloc(sizeof(t_shell));
// 	if (data == NULL)
// 	{
// 		ft_putendl_fd("malloc: Memory allocation failed", 2);
// 		exit(FAILURE);
// 	}
// 	data->exit_code = -1; // for now
// 	data->input_line = NULL;
// 	data->token_list = NULL;
// 	data->env = NULL; // test
// 	return (data);
// }

// /*
//  *   Initial processing of an input line.
//  *   TO_TRIM chars removed from both ends of the line.
//  *   Checks validity of QUOTES
//  *   Checks validity of PIPE
//  *   Checks validity of REDIRECTIONS
//  */
// bool	process_input(char *input_line, t_shell *data)
// {
// 	char	*line;

// 	line = ft_strtrim(input_line, TO_TRIM);
// 	if (line == NULL || line[0] == '\0')
// 	{
// 		free(data->input_line);
// 		free(line);
// 		return (FAILURE);
// 	}
// 	if (check_quote(line, ft_strlen(line)) != 0)
// 	{
// 		free(data->input_line);
// 		free(line);
// 		return (show_error("syntax error: unclosed quote", 258), FAILURE);
// 	}
// 	free(data->input_line);
// 	data->input_line = line;
// 	lexer(data->input_line, data);
// 	if (check_syntax(data) == FAILURE)
// 	{
// 		printf("OSHIBKA V CHECK SYNTAX DETECTED\n");
// 		free_list(&data->token_list);
// 		return (FAILURE);
// 	}
// 	expander(data); // test
// 	return (SUCCESS);
// }

// /*
//  *
//  *   returns 0 if not inside quotes at position 'index',
//  *   or returns the quote character (' or ") if inside a quote.
//  */
// char	check_quote(char *line, int index)
// {
// 	int		i;
// 	char	quote;

// 	i = 0;
// 	quote = 0;
// 	while (i < index)
// 	{
// 		if (quote == 0 && (line[i] == '\'' || line[i] == '"'))
// 			quote = line[i];
// 		else if (quote != 0 && line[i] == quote)
// 			quote = 0;
// 		i++;
// 	}
// 	return (quote);
// }

// void	show_error(char *msg, int exit_code)
// {
// 	(void)exit_code; // addded for testing
// 	ft_putendl_fd(msg, 2);
// 	// global exit status = exit_code
// }
// void	lexer_error(char *input_line, t_shell *data)
// {
// 	free(input_line);
// 	clear_history();
// 	free_list(&data->token_list);
// 	ft_putendl_fd("malloc: Memory allocation failed", 2);
// 	data->exit_code = 1;
// 	exit(data->exit_code);
// }
// void	free_list(t_token **list)
// {
// 	t_token	*temp;

// 	if (!list)
// 		return ;
// 	while (*list)
// 	{
// 		temp = *list;
// 		*list = (*list)->next;
// 		free(temp->content);
// 		temp->content = NULL;
// 		free(temp);
// 		temp = NULL;
// 	}
// 	*list = NULL;
// }

// void	lexer(char *input_line, t_shell *data)
// {
// 	size_t	i;

// 	i = 0;
// 	while (input_line[i])
// 	{
// 		while (is_delimiter(input_line[i]))
// 			i++;
// 		if (is_operator(input_line[i]) && check_quote(input_line, i) == 0)
// 			i += handle_operator(input_line, i, data);
// 		else
// 			i += handle_word(input_line, i, data);
// 	}
// }

// size_t	handle_word(char *input_line, size_t start, t_shell *data)
// {
// 	size_t	i;
// 	size_t	word_start;
// 	size_t	len;
// 	char	*temp;

// 	i = start;
// 	word_start = start;
// 	while (input_line[i])
// 	{
// 		if (check_quote(input_line, i) == 0)
// 		{
// 			if (is_delimiter(input_line[i]) || is_operator(input_line[i]))
// 				break ;
// 		}
// 		i++;
// 	}
// 	len = i - word_start;
// 	temp = malloc(len + 1);
// 	if (temp == NULL)
// 		lexer_error(input_line, data);
// 	ft_strlcpy(temp, &input_line[word_start], len + 1);
// 	add_token(data, WORD, temp);
// 	free(temp);
// 	return (len);
// }

// /*
//  *	Processes operator tokens.
//  *	Adds the corresponding token to the token list.
//  *	Returns the length of the operator token processed (1 or 2).
//  */
// size_t	handle_operator(char *input_line, size_t i, t_shell *data)
// {
// 	if (input_line[i] == '|')
// 	{
// 		add_token(data, PIPE, "|");
// 		return (1);
// 	}
// 	if (input_line[i] == input_line[i + 1])
// 	{
// 		if (input_line[i] == '<')
// 			add_token(data, HEREDOC, "<<");
// 		else
// 			add_token(data, APPEND, ">>");
// 		return (2);
// 	}
// 	else
// 	{
// 		if (input_line[i] == '<')
// 			add_token(data, IN, "<");
// 		else
// 			add_token(data, OUT, ">");
// 		return (1);
// 	}
// }

// /*
//  *	Creates a new token and appends it to the token list.
//  *	If token list is NULL, adds token as head node.
//  *	if token list is not NULL, adds token as the last one.
//  */
// void	add_token(t_shell *data, t_token_type type, char *content)
// {
// 	t_token	*token;
// 	t_token	*temp;

// 	token = malloc(sizeof(t_token));
// 	if (token == NULL)
// 		lexer_error(data->input_line, data);
// 	token->type = type;
// 	token->next = NULL;
// 	token->content = ft_strdup(content);
// 	if (token->content == NULL)
// 	{
// 		free(token);
// 		lexer_error(data->input_line, data);
// 	}
// 	if (data->token_list == NULL)
// 		data->token_list = token;
// 	else
// 	{
// 		temp = data->token_list;
// 		while (temp->next)
// 			temp = temp->next;
// 		temp->next = token;
// 	}
// }

// bool	is_delimiter(int i)
// {
// 	if (i == ' ' || i == '\n' || i == '\t')
// 		return (true);
// 	return (false);
// }

// bool	is_operator(char c)
// {
// 	if (c == '|' || c == '<' || c == '>')
// 		return (true);
// 	return (false);
// }

// bool	check_syntax(t_shell *data)
// {
// 	t_token	*current;

// 	if (data->token_list == NULL)
// 		return (SUCCESS);
// 	current = data->token_list;
// 	while (current)
// 	{
// 		if (current->type == PIPE)
// 		{
// 			if (current == data->token_list || current->next == NULL)
// 				return (show_error("syntax error near unexpected token '|'",
// 						258), FAILURE);
// 			if (current->next->type == PIPE)
// 				return (show_error("syntax error near unexpected token '|'",
// 						258), FAILURE);
// 		}
// 		if (current->type == IN || current->type == OUT
// 			|| current->type == APPEND || current->type == HEREDOC)
// 		{
// 			if (current->next == NULL || current->next->type != WORD)
// 				return (show_error("syntax error near unexpected token", 258),
// 					FAILURE);
// 		}
// 		current = current->next;
// 	}
// 	return (SUCCESS);
// }

// /**
//  *  Expands all environment variables in the shell's token list.
//  *  @param data Pointer to the shell struct containing tokens.
//  */
// void	expander(t_shell *data)
// {
// 	t_token	*current;

// 	current = data->token_list;
// 	while (current)
// 	{
// 		if (current->type == WORD && ft_strchr(current->content, '$'))
// 			current->content = expand_content(current->content, data);
// 		current = current->next;
// 	}
// }

// /**
//  * Expands environment variables, the $? variable,
//  * and handels quotes in the string.
//  * @param content The string to expand.
//  * @param data Pointer to the shell struct(conteins env and exit code).
//  * @return Newly allocated expanded string.
//  */
// char	*expand_content(char *content, t_shell *data)
// {
// 	size_t	i;
// 	char	*new_content;
// 	char	*temp;
// 	char	quote;

// 	i = 0;
// 	quote = 0;
// 	new_content = ft_strdup("");
// 	if (new_content == NULL)
// 		lexer_error(content, data);
// 	while (content[i])
// 	{
// 		quote = update_quote(quote, content[i]);
// 		temp = process_content(content, &i, quote, data);
// 		if (temp == NULL)
// 		{
// 			free(new_content);
// 			lexer_error(content, data);
// 		}
// 		new_content = strjoin_free(new_content, temp);
// 		if (new_content == NULL)
// 			lexer_error(content, data);
// 	}
// 	free(content);
// 	return (new_content);
// }

// /**
//  * Updates the current quote state beased on the character.
//  * @param quote Current quote state (0, '\'' or '"').
//  * @param c Character to process.
//  * @return Updated quote state.
//  */
// char	update_quote(char quote, char c)
// {
// 	if (c == '\'' || c == '"')
// 	{
// 		if (quote == 0)
// 			return (c);
// 		else if (quote == c)
// 			return (0);
// 	}
// 	return (quote);
// }

// /**
//  * Determens wether to handle a dollar variable or normal characters.
//  * @param content The string being processed.
//  * @param i Pointer to the current index in the string.
//  * @param quote Current quote state.
//  * @param data Pointer to the shell struct.
//  * @return Newly allocated string for the processed part.
//  */
// char	*process_content(char *content, size_t *i, char quote, t_shell *data)
// {
// 	if (content[*i] == '$' && quote != '\'')
// 	{
// 		(*i)++;
// 		return (handle_dollar(content, i, data));
// 	}
// 	else
// 		return (handle_characters(content, i));
// }

// /**
//  * Expands a variable starting with '$'.
//  * @param content The string being processed.
//  * @param i Pointer to the index after '$' in the string.
//  * @param data Pointer to the shell struct(conteins env and exit code).
//  * @return Newly allocated string for the processed part.
//  */
// char	*handle_dollar(char *content, size_t *i, t_shell *data)
// {
// 	char	*expanded;

// 	if (ft_isalpha(content[*i]) || content[*i] == '_')
// 	{
// 		expanded = expand_env_var(content, i, data->env);
// 		return (expanded);
// 	}
// 	else if (content[*i] == '?')
// 	{
// 		(*i)++;
// 		// or functon which updates exitcode idk yet
// 		expanded = ft_itoa(data->exit_code);
// 		return (expanded);
// 	}
// 	else
// 	{
// 		expanded = ft_strdup("$");
// 		return (expanded);
// 	}
// }

// /**
//  * Reads consecutive non-variable characters.
//  * @param content The string being processed.
//  * @param i Pointer to the current index (updated to the end of read chars.)
//  * @return Newly allocated string containing the characters, or NULL.
//  */
// char	*handle_characters(char *content, size_t *i)
// {
// 	size_t	start;
// 	char	*chars;

// 	start = *i;
// 	if (content[*i] == '$') // think about it
// 		(*i)++;
// 	while (content[*i] && content[*i] != '$')
// 		(*i)++;
// 	chars = ft_substr(content, start, *i - start);
// 	if (chars == NULL)
// 		return (NULL);
// 	return (chars);
// }
// /**
//  * Extracts a variable name and returns its value from the env.
//  * @param content The string being processed.
//  * @param i Pointer to the current index (updated to end of variable name).
//  * @param env Pointer to th evironment linked list.
//  * @return Newly allocated string with variabl's value, or NULL.
//  */
// char	*expand_env_var(char *content, size_t *i, t_env *env)
// {
// 	size_t	start;
// 	char	*name;
// 	char	*value;

// 	start = *i;
// 	while (ft_isalnum(content[*i]) || content[*i] == '_')
// 		(*i)++;
// 	name = ft_substr(content, start, *i - start);
// 	if (name == NULL)
// 		return (NULL);
// 	value = get_env_value(name, env, ALLOC);
// 	if (value == NULL)
// 	{
// 		free(name);
// 		return (NULL);
// 	}
// 	free(name);
// 	return (value);
// }
// /**
//  * Finds the value of an environment variable.
//  * @param name Name of the environment variable.
//  * @param env Pointer to the environment list.
//  * @return Newly allocated string with value, or an empty string
//  * if not found or NULL.
//  */

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
// /**
//  * Joins two strings and frees the originals.
//  * @param new_content First string to join (freed inside).
//  * @param temp Second string to join (freed inside).
//  * @return Newly allocated concatenated string or NULL.
//  */
// char	*strjoin_free(char *new_content, char *temp)
// {
// 	char	*result;

// 	result = ft_strjoin(new_content, temp);
// 	free(new_content);
// 	free(temp);
// 	if (result == NULL)
// 		return (NULL);
// 	return (result);
// }

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

















// ROMAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAASS (ONE NODE)

t_ast *create_single_word_node(char *cmd, char **args);
t_ast *create_ast_node(t_token_type type, char **value, t_ast *left, t_ast *right);
void free_ast(t_ast *node);


// This is a minimal, self-contained main function to test node creation.
int main(int argc,char **argv,char **envp)
{
    t_shell *data;
    t_ast   *node;
    char    *args[] = {NULL};
	//char    *args2[] = {NULL};

	data = calloc(1, sizeof(t_shell));
	(void)argc;
	(void)argv;
    // Initialize the shell struct
    data->env = NULL;
    data->exit_code = 0;
    data->paths_array = NULL;

    // Create the AST node for "ls -l -a"
    printf("Creating AST node for command: ls -l -a\n");
    node = create_single_word_node("export", args);
    if (!node)
    {
        perror("Failed to create AST node");
        return (1);
    }

    // You can now use the 'node' pointer to call your execute_ast function
    // For example:
	create_env(data, envp);
	// execute_builtin_env(args2, data);
	printf("\n\n\n\n");
    data->exit_code = execute_ast(node, data);

    // Free the allocated memory when done
	// execute_builtin_env(args2, data);
    free_ast(node);

    printf("Node creation and initialization complete.\n");

    return (data->exit_code);
}

/**
 * @brief Creates a single WORD node for testing.
 *
 * This function is a wrapper around create_ast_node, specifically for
 * creating a simple command node (a leaf in the AST).
 *
 * @param cmd The command to execute (e.g., "ls").
 * @param args A null-terminated array of arguments (e.g., {"-l", NULL}).
 * @return A pointer to the newly created t_ast node, or NULL on error.
 */
t_ast *create_single_word_node(char *cmd, char **args)
{
    int         i;
    char        **value;
    int         arg_count;
    t_ast       *node;

    arg_count = 0;
    if (args)
    {
        while (args[arg_count])
            arg_count++;
    }
    value = (char **)malloc(sizeof(char *) * (arg_count + 2));
    if (!value)
        return (NULL);
    value[0] = strdup(cmd);
    if (!value[0])
    {
        free(value);
        return (NULL);
    }
    i = 0;
    while (i < arg_count)
    {
        value[i + 1] = strdup(args[i]);
        if (!value[i + 1])
        {
            while (i >= 0)
                free(value[i--]);
            free(value);
            return (NULL);
        }
        i++;
    }
    value[i + 1] = NULL;
    node = create_ast_node(WORD, value, NULL, NULL);
    if (!node)
    {
        i = 0;
        while (value[i])
            free(value[i++]);
        free(value);
        return (NULL);
    }
    return (node);
}

/**
 * @brief Creates a fake t_ast node.
 * This function allocates a new t_ast node and initializes its fields.
 * It is particularly useful for creating mock syntax trees to test the
 * execution logic of a shell without a working parser.
 * @param type The type of the node (e.g., WORD, PIPE).
 * @param value A null-terminated array of strings (like argv).
 * @param left A pointer to the left child node.
 * @param right A pointer to the right child node.
 * @return A pointer to the newly created t_ast node, or NULL on error.
 */
t_ast *create_ast_node(t_token_type type, char **value, t_ast *left, t_ast *right)
{
    t_ast *node;

    node = (t_ast *)malloc(sizeof(t_ast));
    if (!node)
        return (NULL);
    node->type = type;
    node->value = value;
    node->expand = 0;
    node->status = -1;
    node->fd[0] = -1;
    node->fd[1] = -1;
    node->left = left;
    node->right = right;
    return (node);
}

/**
 * @brief Frees the memory allocated for an AST.
 * This function recursively frees the entire abstract syntax tree,
 * starting from the given root node.
 * @param node The root node of the AST to free.
 */
void free_ast(t_ast *node)
{
    int i;

    if (!node)
        return;
    if (node->left)
        free_ast(node->left);
    if (node->right)
        free_ast(node->right);
    if (node->value)
    {
        i = 0;
        while (node->value[i])
            free(node->value[i++]);
        free(node->value);
    }
    free(node);
}





// NEXT---------------------------------------------------------(with pipe)

// t_ast *create_single_word_node(char *cmd, char **args);
// t_ast *create_ast_node(t_token_type type, char **value, t_ast *left, t_ast *right);
// void free_ast(t_ast *node);

// void *ft_memcpy(void *dst, const void *src, size_t n);
// char *ft_strdup(const char *s1);
// char **ft_split(char const *s, char c);
// void create_env(t_shell *data, char **envp);
// int execute_ast(t_ast *node, t_shell *shell);

// // This is a minimal, self-contained main function to test node creation.
// int main(int argc, char **argv, char **envp)
// {
//     t_shell *data;
//     t_ast *ls_node;
//     t_ast *wc_node;
//     t_ast *pipe_node;
//     char *ls_args[] = {"-l", NULL};
//     char *wc_args[] = {"-l", NULL};

//     data = calloc(1, sizeof(t_shell));
//     (void)argc;
//     (void)argv;

//     // Initialize the shell struct
//     data->env = NULL;
//     data->exit_code = 0;
//     data->paths_array = NULL;

//     create_env(data, envp);

//     printf("Creating AST for: ls -l | wc -l\n");

//     // Create the left-side command: ls -l
//     ls_node = create_single_word_node("ls", ls_args);
//     if (!ls_node)
//     {
//         perror("Failed to create ls node");
//         return (1);
//     }

//     // Create the right-side command: wc -l
//     wc_node = create_single_word_node("wc", wc_args);
//     if (!wc_node)
//     {
//         perror("Failed to create wc node");
//         free_ast(ls_node);
//         return (1);
//     }

//     // Create the pipe node connecting the two commands
//     pipe_node = create_ast_node(PIPE, NULL, ls_node, wc_node);
//     if (!pipe_node)
//     {
//         perror("Failed to create pipe node");
//         free_ast(ls_node);
//         free_ast(wc_node);
//         return (1);
//     }

//     // Execute the full AST
//     data->exit_code = execute_ast(pipe_node, data);

//     // Free the entire AST
//     free_ast(pipe_node);
//     // Free shell data

//     printf("AST execution complete.\n");

//     return (data->exit_code);
// }

// /**
//  * @brief Creates a single WORD node for testing.
//  *
//  * This function is a wrapper around create_ast_node, specifically for
//  * creating a simple command node (a leaf in the AST).
//  *
//  * @param cmd The command to execute (e.g., "ls").
//  * @param args A null-terminated array of arguments (e.g., {"-l", NULL}).
//  * @return A pointer to the newly created t_ast node, or NULL on error.
//  */
// t_ast *create_single_word_node(char *cmd, char **args)
// {
//     int i;
//     char **value;
//     int arg_count;
//     t_ast *node;

//     arg_count = 0;
//     if (args)
//     {
//         while (args[arg_count])
//             arg_count++;
//     }
//     value = (char **)malloc(sizeof(char *) * (arg_count + 2));
//     if (!value)
//         return (NULL);
//     value[0] = strdup(cmd);
//     if (!value[0])
//     {
//         free(value);
//         return (NULL);
//     }
//     i = 0;
//     while (i < arg_count)
//     {
//         value[i + 1] = strdup(args[i]);
//         if (!value[i + 1])
//         {
//             while (i >= 0)
//                 free(value[i--]);
//             free(value);
//             return (NULL);
//         }
//         i++;
//     }
//     value[i + 1] = NULL;
//     node = create_ast_node(WORD, value, NULL, NULL);
//     if (!node)
//     {
//         i = 0;
//         while (value[i])
//             free(value[i++]);
//         free(value);
//         return (NULL);
//     }
//     return (node);
// }

// /**
//  * @brief Creates a fake t_ast node.
//  * This function allocates a new t_ast node and initializes its fields.
//  * It is particularly useful for creating mock syntax trees to test the
//  * execution logic of a shell without a working parser.
//  * @param type The type of the node (e.g., WORD, PIPE).
//  * @param value A null-terminated array of strings (like argv).
//  * @param left A pointer to the left child node.
//  * @param right A pointer to the right child node.
//  * @return A pointer to the newly created t_ast node, or NULL on error.
//  */
// t_ast *create_ast_node(t_token_type type, char **value, t_ast *left, t_ast *right)
// {
//     t_ast *node;

//     node = (t_ast *)malloc(sizeof(t_ast));
//     if (!node)
//         return (NULL);
//     node->type = type;
//     node->value = value;
//     node->expand = 0;
//     node->status = -1;
//     node->fd[0] = -1;
//     node->fd[1] = -1;
//     node->left = left;
//     node->right = right;
//     return (node);
// }

// /**
//  * @brief Frees the memory allocated for an AST.
//  * This function recursively frees the entire abstract syntax tree,
//  * starting from the given root node.
//  * @param node The root node of the AST to free.
//  */
// void free_ast(t_ast *node)
// {
//     int i;

//     if (!node)
//         return;
//     if (node->left)
//         free_ast(node->left);
//     if (node->right)
//         free_ast(node->right);
//     if (node->value)
//     {
//         i = 0;
//         while (node->value[i])
//             free(node->value[i++]);
//         free(node->value);
//     }
//     free(node);
// }