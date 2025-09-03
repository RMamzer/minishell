/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_IFS.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 14:47:35 by mklevero          #+#    #+#             */
/*   Updated: 2025/09/03 15:32:35 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	separator(char c, const char *sep);
static int	number_of_words(const char *str, const char *sep);
static char	*separating_words(const char *str, const char *sep);
static void	*free_sp_ifs(char **arr);

char	**ft_split_IFS(char *str, char *charset)
{
	char	**arr;
	int		i;

	i = 0;
	if (!str || !charset || *charset == '\0')
		return (NULL);
	arr = malloc(sizeof(char *) * (number_of_words(str, charset) + 1));
	if (arr == NULL)
		return (NULL);
	while (*str)
	{
		if (!separator(*str, charset))
		{
			arr[i] = separating_words(str, charset);
			if (!arr[i])
				return (free_sp_ifs(arr));
			i++;
			while (*str && !separator(*str, charset))
				str++;
		}
		else
			str++;
	}
	arr[i] = NULL;
	return (arr);
}

static int	separator(char c, const char *sep)
{
	int	i;

	i = 0;
	while (sep[i] != '\0')
	{
		if (sep[i] == c)
			return (1);
		i++;
	}
	return (0);
}

static int	number_of_words(const char *str, const char *sep)
{
	int	i;
	int	counter;

	i = 0;
	counter = 0;
	while (str[i] != '\0')
	{
		if (!separator(str[i], sep))
		{
			counter++;
			while (str[i] != '\0' && !(separator(str[i], sep)))
				i++;
		}
		else
			i++;
	}
	return (counter);
}

static char	*separating_words(const char *str, const char *sep)
{
	char	*separated;
	int		i;
	int		k;

	i = 0;
	k = 0;
	while (str[i] != '\0' && !(separator(str[i], sep)))
		i++;
	separated = malloc(sizeof(char) * (i + 1));
	if (separated == NULL)
		return (NULL);
	while (k < i)
	{
		separated[k] = str[k];
		k++;
	}
	separated[k] = '\0';
	return (separated);
}

static void	*free_sp_ifs(char **arr)
{
	int i;

	i = 0;
	while (arr[i])
		free(arr[i++]);
	free(arr);
	return (NULL);
}