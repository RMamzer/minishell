/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/11 14:15:55 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/12 17:21:19 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// TODO: sskobki chtob ostavalis' vnutri herdoca
// TODO: syntax check for << <<
// TODO: EOF "da" dolzhen bit da

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

bool	process_heredoc(t_shell *shell)
{
	int		fd;
	size_t	i;
	char	*file;
	t_token	*current;

	i = 0;
	file = NULL;
	current = shell->token_list;
	while (current)
	{
		if (current->type == HEREDOC && current->next) // think here
		{
			process_delim(current->next, shell);
			update_file_name(&file, &i, shell);
			fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
			if (fd == -1)
			{
				free(file);
				return (FAILURE);
			}
			if (read_heredoc(&fd, current->next, shell) == FAILURE)
			{
				close(fd);
				free(file);
				return (FAILURE);
			}
			close(fd);
			update_heredoc_token(current, file);
			i++;
		}
		current = current->next;
	}
	return (SUCCESS);
}

bool	read_heredoc(int *fd, t_token *delim, t_shell *shell)
{
	char	*line;

	while (69)
	{
		line = readline("> ");
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
			expand_heredoc(&line, shell);
		write(*fd, line, ft_strlen(line));
		write(*fd, "\n", 1);
		free(line);
	}
	return (SUCCESS);
}

void	update_file_name(char **file, size_t *i, t_shell *shell)
{
	char	*file_num;

	while (69)
	{
		file_num = ft_itoa(*i);
		if (!file_num)
			fatality(ERROR_MEM, shell, 1);
		*file = ft_strjoin("/tmp/kunteynir", file_num);
		free(file_num);
		if (!file)
			fatality(ERROR_MEM, shell, 1);
		if (access(*file, F_OK) == 0)
		{
			free(*file);
			(*i)++;
		}
		else
			break ;
	}
}

void	expand_heredoc(char **line, t_shell *shell)
{
	char	*expanded;

	if (!line || !*line)
		return ;
	expanded = heredoc_expander(*line, shell);
	free(*line);
	*line = expanded;
}

void	update_heredoc_token(t_token *current, char *file)
{
	free(current->content);
	current->type = IN;
	current->content = ft_strdup("<");
	if (!current->content)
		// free smth
		free(current->next->content);
	current->next->type = WORD;
	current->next->content = file;
}
char	*heredoc_expander(char *line, t_shell *shell)
{
	size_t i;
	size_t start;
	char *new_content;
	char *temp;

	i = 0;
	new_content = ft_strdup("");
	if (new_content == NULL)
		fatality(ERROR_MEM, shell, 1);
	while (line[i])
	{
		if (line[i] == '$')
		{
			i++;
			if (ft_isalpha(line[i]) || line[i] == '_')
				temp = expand_env_var(line, &i, shell->env);
			else if (line[i] == '?')
			{
				i++;
				temp = ft_itoa(shell->exit_code);
			}
			else
				temp = ft_strdup("$");
		}
		else
		{
			start = i;
			while (line[i] && line[i] != '$')
				i++;
			temp = ft_substr(line, start, i - start);
		}
		if (!temp)
		{
			free(new_content);
			fatality(ERROR_MEM, shell, 1);
		}
		new_content = strjoin_free(new_content, temp);
		if (!new_content)
			fatality(ERROR_MEM, shell, 1);
	}
	return (new_content);
}