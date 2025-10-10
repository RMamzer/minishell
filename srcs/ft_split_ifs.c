/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_ifs.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 14:47:35 by mklevero          #+#    #+#             */
/*   Updated: 2025/10/10 15:40:22 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static int	separator(char c, const char *sep);
static int	number_of_words(const char *str, const char *sep);
static char	*separating_words(const char *str, const char *sep);
static void	*free_sp_ifs(char **arr);

/**
 * Splits a string using IFS (Internal Field Separator) characters.
 * Each word is defined as a sequence of characters not present
 * in the separator set.
 *
 * @param str String to split
 * @param charset The set of separator characters (IFS: space, tab, newline)
 * @return Array of strings or NULL on error
 */
char	**ft_split_ifs(char *str, char *charset)
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

/**
 * Checks if a character is one of the separator characters.
 *
 * @param c Character to check
 * @param sep String containing all separator characters
 * @return 1 if character is a separator, 0 otherwise
 */
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

/**
 * Counts the number of words in the string using given separators.
 *
 * @param str String to analyze
 * @param sep String containing separator characters
 * @return Number of words found
 */
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

/**
 * Extracts a single word from the string starting at current position.
 * Allocates and returns a new null-terminated string.
 *
 * @param str Pointer to the start of the word in the input string
 * @param sep String containing separator characters
 * @return Newly allocated string containing the word, or NULL on error
 */
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

/**
 * Frees all memory allocated for an array of strings.
 * Used for cleanup in case of allocation failure.
 *
 * @param arr Array to free
 * @return Always returns NULL
 */
static void	*free_sp_ifs(char **arr)
{
	int	i;

	i = 0;
	while (arr[i])
		free(arr[i++]);
	free(arr);
	return (NULL);
}
