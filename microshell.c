/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtellami <mtellami@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/02/08 07:28:57 by mtellami          #+#    #+#             */
/*   Updated: 2023/02/08 10:36:15 by mtellami         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct s_list
{
	char **args;
	char *sep;
	struct s_list *next;
}	t_list;

int ft_strlen(char *str)
{
	int i = -1;
	while (str[++i]);
	return (i);
}

int ft_tabsize(char **tab)
{
	int i = -1;
	while (tab[++i]);
	return (i);
}

void ft_freearr(char **arr)
{
	int  i = -1;
	while (arr[++i])
		free(arr[i]);
	free(arr);
}

void err_fatal(void)
{
	char *msg = "error: fatal\n";
	write(2, msg, ft_strlen(msg));
	exit(1);
}

char *ft_strdup(char *str)
{
	char *dup = malloc(ft_strlen(str) + 1);
	if (!dup)
		return (NULL);
	int i = -1;
	while (str[++i])
		dup[i] = str[i];
	dup[i] = 0;
	return (dup);
}

char **concate(char **tab, char *str)
{
	if (!tab)
	{
		tab = malloc(sizeof(char *));
		*tab = NULL;
	}
	char **buffer = malloc(sizeof(char *) * (ft_tabsize(tab) + 2));
	if (!buffer)
		return (NULL);
	int i = -1;
	while (tab[++i])
		buffer[i] = ft_strdup(tab[i]);
	buffer[i++] = ft_strdup(str);
	buffer[i] = NULL;
	ft_freearr(tab);
	return (buffer);
}

t_list *newnode(char **args, char *sep)
{
	t_list *new = malloc(sizeof(t_list));
	if (!new)
		return (NULL);
	new->args = args;
	new->sep = sep;
	new->next = NULL;
	return (new);
}

t_list *lastnode(t_list *lst)
{
	while (lst->next)
		lst = lst->next;
	return (lst);
}

void addback(t_list **lst, t_list *node)
{
	if (*lst)
		lastnode(*lst)->next = node;
	else
		*lst = node;
}

t_list *init_list(int ac, char **av)
{
	t_list *lst = NULL;
	int i = 1;
	while (i < ac)
	{
		char **buffer = NULL;
		while (i < ac && strcmp(av[i], ";") && strcmp(av[i], "|"))
			buffer = concate(buffer, av[i++]);
		addback(&lst, newnode(buffer, av[i++]));
		while (i < ac && !strcmp(av[i], ";"))
			i++;
	}
	return (lst);
}

void re_cd(char **args)
{
	if (ft_tabsize(args) != 2)
		write(2, "error: cd: bad arguments\n", 25);
	else
	{
		if (chdir(args[1]))
		{
			write(2, "error: cd: cannot change directory to ", 38);
			write(2, args[1], ft_strlen(args[1]));
			write(2, "\n", 1);
		}
	}
}

void execution(char **args, char **env)
{

	if (execve(args[0], args, env) == -1)
	{
		write(2, "error: cannot execute ", 22);
		write(2, args[0], ft_strlen(args[0]));
		write(2, "\n", 1);
		exit(1);
	}
}

void multi_pipe(t_list *lst, char **env)
{
	int fd[2];
	if (!strcmp(lst->args[0], "cd"))
		re_cd(lst->args);
	else
	{
		if (pipe(fd) == -1)
			err_fatal();
		int pid = fork();
		if (pid  == -1)
			err_fatal();
		else if (!pid)
		{
			close(fd[0]);
			if (dup2(fd[1], 1) == -1)
				err_fatal();
			close(fd[1]);
			execution(lst->args, env);
		}
		close(fd[1]);
		if (dup2(fd[0], 0) == -1)
			err_fatal();
		close(fd[0]);
		waitpid(pid, NULL, 0);
	}
}

void exec_last(t_list *lst, char **env)
{
	if (!strcmp(lst->args[0], "cd"))
		re_cd(lst->args);
	else
	{
		int pid = fork();
		if (pid == -1)
			err_fatal();
		else if (!pid)
			execution(lst->args, env);
		waitpid(pid, NULL, 0);
	}
}

int main(int ac, char **av, char **env)
{
	t_list *list = init_list(ac, av);
	t_list *tmp = list;
	int i = dup(0);
	while (tmp)
	{
		while (tmp->sep && !strcmp(tmp->sep, "|"))
		{
			multi_pipe(tmp, env);
			tmp = tmp->next;
		}
		exec_last(tmp, env);
		if (dup2(i, 0) == -1)
			err_fatal();
		tmp = tmp->next;
	}
	close(i);
	return (0);
}
