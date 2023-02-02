/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtellami <mtellami@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/11 13:36:00 by mtellami          #+#    #+#             */
/*   Updated: 2023/02/02 16:16:15 by mtellami         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

t_data *newnode(char **args, char *sep)
{
	t_data  *new = malloc(sizeof(t_data));
	if (!new)
		err_fatal();
	new->args = args;
	new->sep = sep;
	new->next = NULL;
	return (new);
}

t_data	*lastnode(t_data *lst)
{
	while (lst->next)
		lst = lst->next;
	return (lst);
}

void	addback(t_data **lst, t_data  *node)
{
	if (*lst)
		lastnode(*lst)->next = node;
	else
		*lst = node;
}

t_data	*init_list(int ac, char **av)
{
	t_data *lst = NULL;
	int i = 1;
	while (i < ac)
	{
		char **buffer = NULL;
		while (i < ac && strcmp(av[i], "|") && strcmp(av[i], ";"))
			buffer = arr_concate(buffer, av[i++]);
		t_data *node = newnode(buffer, av[i]);
		addback(&lst, node);
		i++;
	}
	return (lst);
}

void	re_cd(t_data *data)
{
	if (ft_tabsize(data->args) != 2)
		write(2, "error: cd: bad arguments\n", 26);
	else
	{
		if ((chdir(data->args[1])) == -1)
		{
			write(2, "error: cd: cannot change directory to ", 38);
			write(2, data->args[1], ft_strlen(data->args[1]));
			write(2, "\n", 1);
		}
	}
}

void	execution(t_data *lst, char **env)
{
	if ((execve(lst->args[0], lst->args, env)) == -1)
	{
		write(2, "error: cannot execute ", 22);
		write(2, lst->args[0], ft_strlen(lst->args[0]));
		write(2, "\n", 1);
		exit(1);
	}
}

void	multi_pipe(t_data *lst, char **env)
{
	int	fd[2];
	if (!strcmp(lst->args[0], "cd"))
		re_cd(lst);
	else
	{
		if (pipe(fd) == -1)
			err_fatal();
		int pid = fork();
		if (pid == -1)
			err_fatal();
		if (!pid)
		{
			close(fd[0]);
			if (dup2(fd[1], 1) == -1)
				err_fatal();
			close(fd[1]);
			execution(lst, env);
		}
		close(fd[1]);
		if (dup2(fd[0], 0) == -1)
			err_fatal();
		close(fd[0]);
		waitpid(pid, NULL, 0);
	}
}

void	exec_last(t_data *lst, char **env)
{
	int stat;
	if (!strcmp(lst->args[0], "cd"))
		re_cd(lst);
	else
	{
		int pid = fork();
		if (pid == -1)
			err_fatal();
		if (!pid)
			execution(lst, env);
		waitpid(pid, &stat, 0);
		if (stat)
			exit(stat);
	}
}

int	main(int ac, char **av, char **env)
{
	t_data *lst = init_list(ac, av);
	t_data *tmp = lst;
	while (tmp)
	{
		int in = dup(0);
		if (in == -1)
			err_fatal();
		while (tmp->sep && !strcmp(tmp->sep, "|"))
		{
			multi_pipe(tmp, env);
			tmp = tmp->next;
		}
		if ((dup2(0, in)) == -1)
			err_fatal();
		close(in);
		exec_last(tmp, env);
		tmp = tmp->next;
	}
	return (0);
}
