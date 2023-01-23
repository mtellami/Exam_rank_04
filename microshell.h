/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.h                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtellami <mtellami@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/11 13:35:38 by mtellami          #+#    #+#             */
/*   Updated: 2023/01/23 09:50:53 by mtellami         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MICROSHELL_H
# define MICROSHELL_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

typedef struct s_data
{
	char			**args;
	char			*sep;
	struct s_data	*next;
}	t_data;

int		ft_strlen(char *str);
char 	*ft_strdup(char *str);
int		ft_tabsize(char **tab);
char 	*ft_strdup(char *str);
void	ft_freearr(char **arr);
char	**arr_concate(char **tab, char *str);
void	err_fatal(void);

#endif