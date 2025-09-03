/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mklevero <mklevero@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 14:47:35 by mklevero          #+#    #+#             */
/*   Updated: 2025/03/13 18:04:11 by mklevero         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <stdlib.h>

char	*separating_words(char *str, char *sep);
int		number_of_words(char *str, char *sep);
int		separator(char c, char *sep);

char	**ft_split(char *str, char *charset)
{
	char	**arr;
	int		i;

	i = 0;
	arr = malloc(sizeof(char *) * (number_of_words(str, charset) + 1));
	if (arr == NULL)
		return (NULL);
	while (*str != '\0')
	{
		if (!separator(*str, charset))
		{
			arr[i] = separating_words(str, charset);
			i++;
			while (*str != '\0' && !separator(*str, charset))
				str++;
		}
		else
			str++;
	}
	arr[i] = NULL;
	return (arr);
}

int	separator(char c, char *sep)
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

int	number_of_words(char *str, char *sep)
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

char	*separating_words(char *str, char *sep)
{	
	char		*separated;
	int			i;
	int			k;

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
