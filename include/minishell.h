/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rmamzer <rmamzer@student.hive.fi>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/06 14:59:35 by rmamzer           #+#    #+#             */
/*   Updated: 2025/08/07 15:55:32 by rmamzer          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

#include <stdio.h> //perror, printf
#include <errno.h> // errno, CHECK IF NEEDEED<---------------
#include <stdlib.h> // malloc
#include <unistd.h> //fork, write
#include <sys/wait.h> //waitpid

# define SUCCESS 0
# define FAILURE 1

typedef struct s_shell
{
	int	exit_code;

}		t_shell;

// token type
typedef enum e_token_type
{
	PIPE,
	IN,
	OUT,
	WORD,
	HEREDOC,
	APPEND

}		t_token_type;


typedef struct s_ast
{
  t_token_type        type;
  char        **value;
  int          expand;
  int          status;
  int          fd[2];
  struct s_tree_node  *left;
  struct s_tree_node  *right;
}	t_ast;

// typedef struct s_tree_node
// {
//   t_token_type        type;
//   char        **value;
//   int          expand;
//   int          status;
//   int          fd[2];
//   struct s_tree_node  *left;
//   struct s_tree_node  *right;
// }  t_tree_node;


#endif