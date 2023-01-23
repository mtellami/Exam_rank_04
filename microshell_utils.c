/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell_utils.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtellami <mtellami@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/22 19:53:10 by mtellami          #+#    #+#             */
/*   Updated: 2023/01/23 03:57:34 by mtellami         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

int	ft_strlen(char *str)
{
	int	i = 0;
	while (str[i])
		i++;
	return (i);
}

int	ft_tabsize(char **tab)
{
	int i = 0;
	while (tab[i])
		i++;
	return (i);
}

char 	*ft_strdup(char *str)
{
	char	*dup = malloc(sizeof(char) * (ft_strlen(str) + 1));
	if (!dup)
		exit_fatal();
	int i = 0;
	while (str[i])
	{
		dup[i] = str[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

void	ft_freearr(char **arr)
{
	int i = 0;
	while (arr[i])
		free(arr[i++]);
	free(arr);
}

char	**arr_concate(char **tab, char *str)
{
	if (!tab)
	{
		tab = malloc(sizeof(char *));
		tab[0] = NULL;
	}
	char **arr = malloc(sizeof(char *) * (ft_tabsize(tab) + 2));
	int		i = 0;
	if (!arr)
		exit_fatal();
	while (tab[i])
	{
		arr[i] = ft_strdup(tab[i]);
		i++;
	}
	arr[i++] = ft_strdup(str);
	arr[i] = NULL;
	ft_freearr(tab);
	return (arr);
}

void	exit_fatal(void)
{
	write(2, "error: fatal\n", 13);
	exit(1);
}
