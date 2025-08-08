/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 14:59:35 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/08 18:10:13 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "libft.h"
# include <readline/history.h>  // add_history
# include <readline/readline.h> // readline
# include <stdbool.h>
# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>

# define SUCCESS 0
# define FAILURE 1

# define TO_TRIM " \t\n" // check this

// core
typedef struct s_shell
{
	int		exit_code;
	char	*input_line;

}			t_shell;

// token type
typedef enum e_token_type
{
	PIPE,
	IN,
	OUT,
	WORD,
	HEREDOC,
	APPEND

}			t_token_type;

int			main(int ac, char **av, char **env);
t_shell		*init_data(void);


// pre processing of input
bool		process_input(char *input_line, t_shell *data);
bool		valid_pipe_usage(char *line);
bool		valid_redirection_usage(char *line);
char		check_quote(char *line, int index);
void		show_error(char *msg, int exit_code);

#endif
