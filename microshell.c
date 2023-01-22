/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   microshell.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mtellami <mtellami@student.1337.ma>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/01/11 13:36:00 by mtellami          #+#    #+#             */
/*   Updated: 2023/01/23 00:02:34 by mtellami         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "microshell.h"

int	count_lines(int ac, char **av)
{
	int	i = 1;
	int	size = 1;
	while (i < ac)
	{
		if (!strcmp(av[i], ";"))
			size++;
		i++;
	}
	return (size);
}

char	***init_buffer(int ac, char **av)
{
	int size = count_lines(ac, av);
	char	***lines = malloc(sizeof(char **) * (size + 1));
	if (!lines)
		exit_fatal();
	char	**buffer = NULL;
	int i = 1;
	int j = 0;
	while (i < ac)
	{
		while (i < ac && strcmp(av[i], ";"))
			buffer = arr_concate(buffer, av[i++]);
		lines[j++] = buffer;
		buffer = NULL;
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
	char	**args = NULL;
	int	i = 0;
	while (buffer[i])
	{
		while (buffer[i] && strcmp(buffer[i], "|"))
			args = arr_concate(args, buffer[i++]);
		t_data	*node = newnode(args);
		addback(&lst, node);
		if (buffer[i])
			i++;
	}
	return (lst);
}

void	exec_line(t_data *data, char **env)
{
	int id = fork();
	int pid;
	if (!id)
	{
		while (data->next)
		{
			int fd[2];
			if (pipe(fd) == -1)
				exit_fatal();
			pid = fork();
			if (pid == -1)
				exit_fatal();
			if (!pid)
			{
				close(fd[0]);
				if (dup2(fd[1], STDOUT_FILENO) == -1)
					exit_fatal();
				close(fd[1]);
				execve(data->path, data->args, env);
			}
			close(fd[1]);
			if (dup2(fd[0], STDIN_FILENO) == -1)
				exit_fatal();
			close(fd[0]);
			waitpid(pid, NULL, 0);
			data = data->next;
		}
		pid = fork();
		if (pid == -1)
			exit_fatal();
		if (!pid)
			execve(data->path, data->args, env);
		waitpid(pid, NULL, 0);
	}
	waitpid(id, NULL, 0);
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
		// clear_list(head);
	}
	return (0);
}
