/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtellami <mtellami@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/11 13:36:00 by mtellami          #+#    #+#             */
/*   Updated: 2023/01/23 07:40:58 by mtellami         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

int	count_lines(int ac, char **av)
{
	int	i = 0;
	int	size = 1;
	while (++i < ac)
		if (!strcmp(av[i], ";"))
			size++;
	return (size);
}

char	***init_buffer(int ac, char **av)
{
	char ***lines = malloc(sizeof(char **) * (count_lines(ac, av) + 1));
	if (!lines)
		exit_fatal();
	char **buffer = NULL;
	int i = 1;
	int j = 0;
	while (i < ac)
	{
		while (i < ac && strcmp(av[i], ";"))
			buffer = arr_concate(buffer, av[i++]);
		lines[j++] = buffer;
		buffer = NULL;
		if (i < ac)
			i++;
	}
	lines[j] = NULL;
	return (lines);
}

t_data	*newnode(char **args)
{
	t_data	*new = malloc(sizeof(t_data));
	if (!new)
		exit_fatal();
	new->args = args;
	new->path = args[0];
 	new->next = NULL;
	return (new);
}

t_data	*lastnode(t_data *head)
{
	while (head->next)
		head = head->next;
	return (head);
}

void	addback(t_data 	**lst, t_data *node)
{
	t_data	*tmp = *lst;
	if (*lst)
	{
		tmp = lastnode(tmp);
		tmp->next = node;
	}
	else
		*lst = node;
}

t_data	*init_list(char **buffer)
{
	t_data	*lst = NULL;
	char	**args;
	int	i = 0;
	while (buffer[i])
	{
		args = NULL;
		while (buffer[i] && strcmp(buffer[i], "|"))
			args = arr_concate(args, buffer[i++]);
		t_data	*node = newnode(args);
		addback(&lst, node);
		if (buffer[i])
			i++;
	}
	return (lst);
}

void	execution(t_data *data, char **env)
{
	if (!strcmp(data->path, "cd"))
	{
		if (ft_tabsize(data->args) != 2)
		{
			write(2, "error: cd: bad arguments\n", 26);
			exit(1);
		}
		else
		{
			if ((chdir(data->args[1])) == -1)
			{
				write(2, "error: cd: cannot change directory to ", 38);
				write(2, data->args[1], ft_strlen(data->args[1]));
				write(2, "\n", 1);
				exit(1);
			}
			exit(0);
		}
	}
	else
	{
		if ((execve(data->path, data->args, env)) == -1)
		{
			write(2, "error: cannot execute ", 22);
			write(2, data->path, ft_strlen(data->path));
			write(2, "\n", 1);
			exit(0);
		}
	}
}

void	multi_pipes(t_data *data, char **env)
{
	while (data->next)
	{
		int fd[2];
		if (pipe(fd) == -1)
			exit_fatal();
		int pid = fork();
		if (pid == -1)
			exit_fatal();
		if (!pid)
		{
			close(fd[0]);
			if (dup2(fd[1], 1) == -1)
				exit_fatal();
			close(fd[1]);
			execution(data, env);
		}
		close(fd[1]);
		if (dup2(fd[0], 0) == -1)
			exit_fatal();
		close(fd[0]);
		int stat;
		waitpid(pid, &stat, 0);
		if (stat)
			exit(stat);
		data = data->next;
	}
}

void	exec_line(t_data *data, char **env)
{
	int stat;
	int id = fork();
	if (id == -1)
		exit_fatal();
	if (!id)
	{
		multi_pipes(data, env);
		data = lastnode(data);
		int pid = fork();
		if (pid == -1)
			exit_fatal();
		if (!pid)
			execution(data, env);
		waitpid(pid, &stat, 0);
		if (stat)
			exit(stat);
		exit(0);
	}
	waitpid(id, &stat, 0);
	if (stat)
		exit(stat);
}

void	clear_list(t_data *lst)
{
	t_data *ptr = lst;
	while (ptr)
	{
		ft_freearr(ptr->args);
		ptr = ptr->next;
	}
	t_data	*tmp;
	while (lst)
	{
		tmp = lst;
		lst = lst->next;
		free(tmp);
		tmp = NULL;
	}
}

int	main(int argc, char **argv, char **env)
{
	char	***buffer = init_buffer(argc, argv);
	int	i = 0;
	while (buffer[i])
	{
		t_data	*head = init_list(buffer[i++]);
		exec_line(head, env);
		clear_list(head);
	}
	return (0);
}
