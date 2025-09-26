/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 14:15:55 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/26 17:52:44 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int check_sig_hook(void)
{
	if (g_sig == SIGINT)
		rl_done = 1;
	return (0);
}



bool	process_heredoc(t_shell *shell)
{
	size_t	i;
	t_token	*current;

	i = 0;
	current = shell->token_list;
	while (current)
	{
		if (current->type == HEREDOC && current->next)
		{
			if (process_heredoc_token(shell, current, i) == FAILURE)
				return (FAILURE);
			i++;
		}
		current = current->next;
	}
	return (SUCCESS);
}

bool	process_heredoc_token(t_shell *shell, t_token *current, size_t i)
{
	int		fd;
	char	*file;

	file = NULL;
	process_delim(current->next, shell);
	update_file_name(&file, &i, shell);
	fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (fd == -1)
	{
		free(file);
		//return (FAILURE);
		brutality("minishell: open", shell, 1);
	}
	if (read_heredoc(&fd, current->next, shell, file) == FAILURE)
	{
		close(fd);
		unlink(file);
		free(file);
		return (FAILURE);
	}
	close(fd);
	update_heredoc_token(current, file, shell);
	save_heredoc_file(shell, file);
	free(file);
	return (SUCCESS);
}



void	process_delim(t_token *delim, t_shell *shell)
{
	char	*new_content;
	size_t	i;
	size_t	j;

	new_content = ft_calloc(ft_strlen(delim->content) + 1, sizeof(char));
	if (!new_content)
		fatality(ERROR_MEM, shell, 1);
	i = 0;
	j = 0;
	while (delim->content[i])
	{
		if (delim->content[i] == '\'' || delim->content[i] == '"')
		{
			i++;
			continue ;
		}
		new_content[j++] = delim->content[i++];
	}
	free(delim->content);
	delim->content = new_content;
}

void	update_file_name(char **file, size_t *i, t_shell *shell)
{
	char	*file_num;

	while (1)
	{
		file_num = ft_itoa(*i++);
		if (!file_num)
			fatality(ERROR_MEM, shell, 1);
		*file = ft_strjoin("/tmp/kunteynir", file_num);
		free(file_num);
		if (!*file)
			fatality(ERROR_MEM, shell, 1);
		if (access(*file, F_OK) != 0)
			break ;
		free(*file);
	}
}

bool	read_heredoc(int *fd, t_token *delim, t_shell *shell, char *file)
{
	char	*line;

	set_heredoc_signal();
	rl_event_hook = check_sig_hook;
	while (1)
	{
		line = readline("> ");
		if (g_sig == SIGINT)
		{
			if(line)
				free(line);
			g_sig = 0;
			rl_event_hook = NULL;
			return (FAILURE);
		 } // for now
		if (!line)
		{
			ft_putendl_fd(ERROR_EOF, 2);
			break ;
		}
		if (ft_strcmp(line, delim->content) == 0)
		{
			free(line);
			break ;
		}
		if (delim->quoted == false)
			expand_heredoc(&line, shell, file);
		if (write(*fd, line, ft_strlen(line)) == -1)
		{
			free(line);
			brutality("minishell: write",  shell, 1);
		}
		if (write(*fd, "\n", 1) == -1)
		{
			free(line);
			brutality("minishell: write",  shell, 1);
		}
		free(line);
	}
	rl_event_hook = NULL;
	return (SUCCESS);
}

void	expand_heredoc(char **line, t_shell *shell, char *file)
{
	char	*expanded;

	if (!line || !*line)
		return ;
	expanded = heredoc_expander(*line, shell, file);
	free(*line);
	*line = expanded;
}

char	*heredoc_expander(char *line, t_shell *shell, char *file)
{
	char	*new_content;

	new_content = ft_strdup("");
	if (!new_content)
	{
		free(file);
		free(line);
		fatality(ERROR_MEM, shell, 1);
	}
	if (expand_line(line, &new_content, shell) == FAILURE)
	{
		free(file);
		free(new_content);
		free(line);
		fatality(ERROR_MEM, shell, 1);
	}
	return (new_content);
}

bool	expand_line(char *line, char **new_content, t_shell *shell)
{
	size_t	i;
	char	*temp;

	i = 0;
	while (line[i])
	{
		temp = get_new_content(line, &i, shell);
		if (!temp)
			return (FAILURE);
		*new_content = strjoin_free(*new_content, temp);
		if (!*new_content)
			return (FAILURE);
	}
	return (SUCCESS);
}

char	*get_new_content(char *line, size_t *i, t_shell *shell)
{
	size_t	start;
	char	*temp;

	if (line[*i] == '$')
	{
		(*i)++;
		if (ft_isalpha(line[*i]) || line[*i] == '_')
			temp = expand_env_var(line, i, shell->env);
		else if (line[*i] == '?')
		{
			(*i)++;
			temp = ft_itoa(shell->exit_code);
		}
		else
			temp = ft_strdup("$");
	}
	else
	{
		start = *i;
		while (line[*i] && line[*i] != '$')
			(*i)++;
		temp = ft_substr(line, start, *i - start);
	}
	return (temp);
}
void	update_heredoc_token(t_token *current, char *file, t_shell *shell)
{
	free(current->content);
	current->type = IN;
	current->content = ft_strdup("<");
	if (!current->content)
	{
		free(file);
		fatality(ERROR_MEM, shell, 1);
	}
	if (current->next)
	{
		free(current->next->content);
		current->next->type = WORD;
		current->next->content = ft_strdup(file);
		if (!current->next->content)
		{
			free(file);
			fatality(ERROR_MEM, shell, 1);
		}
	}
}

void	save_heredoc_file(t_shell *shell, char *file)
{
	char	**new;
	size_t	count;
	size_t	i;

	i = 0;
	new = allocate_heredoc_array(shell, file, &count);
	while (i < count)
	{
		new[i] = shell->heredoc_files[i];
		i++;
	}
	new[count] = ft_strdup(file);
	if (!new[count])
	{
		free(new);
		free(file);
		fatality(ERROR_MEM, shell, 1);
	}
	free(shell->heredoc_files);
	shell->heredoc_files = new;
}

char	**allocate_heredoc_array(t_shell *shell, char *file, size_t *count)
{
	char	**new;

	*count = 0;
	if (shell->heredoc_files)
		while (shell->heredoc_files[*count])
			(*count)++;
	new = ft_calloc(*count + 2, sizeof(char *));
	if (!new)
	{
		free(file);
		fatality(ERROR_MEM, shell, 1);
	}
	return (new);
}

void	free_heredoc_files(t_shell *shell)
{
	size_t	i;

	if (!shell->heredoc_files)
		return ;
	i = 0;
	while (shell->heredoc_files[i])
	{
		unlink(shell->heredoc_files[i]);
		free(shell->heredoc_files[i]);
		i++;
	}
	free(shell->heredoc_files);
	shell->heredoc_files = NULL;
}

/*
char	*heredoc_expander(char *line, t_shell *shell)
{
	size_t	i;
	char	*new_content;
	char	*temp;

	i = 0;
	new_content = ft_strdup("");
	if (new_content == NULL)
	{
		free(line); // Free the input line before exiting
		fatality(ERROR_MEM, shell, 1);
	}
	while (line[i])
	{
		temp = get_new_content(line, &i, shell);
		if (!temp)
		{
			free(new_content);
			free(line); // Free the input line before exiting
			fatality(ERROR_MEM, shell, 1);
		}
		new_content = strjoin_free(new_content, temp);
		if (!new_content)
		{
			free(line); // Free the input line before exiting
			fatality(ERROR_MEM, shell, 1);
		}
	}
	return (new_content);
}
*/

/*
void	save_heredoc_file(t_shell *shell, char *file)
{
	char	**new;
	size_t	count;
	size_t	i;

	count = 0;
	i = 0;
	if (shell->heredoc_files)
		while (shell->heredoc_files[count])
			count++;
	new = ft_calloc(count + 2, sizeof(char *));
	if (!new)
	{
		free(file); // Free the file parameter before exiting
		fatality(ERROR_MEM, shell, 1);
	}
	while (i < count)
	{
		new[i] = shell->heredoc_files[i];
		i++;
	}
	new[count] = ft_strdup(file);
	if (!new[count])
	{
		free(new);  // Free the allocated array
		free(file); // Free the file parameter before exiting
		fatality(ERROR_MEM, shell, 1);
	}
	free(shell->heredoc_files);
	shell->heredoc_files = new;
}
*/