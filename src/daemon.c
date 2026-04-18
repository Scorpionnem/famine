/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   daemon.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/18 14:44:45 by mbatty            #+#    #+#             */
/*   Updated: 2026/04/18 15:23:41 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "famine.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdint.h>
#include <sys/syscall.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <sys/file.h>

int	lock_lock(t_service_ctx *ctx, const char *path)
{
	ctx->lock_fd = open(path, O_RDWR | O_CREAT, 0777);
	if (ctx->lock_fd == -1)
		return (-1);
	if (flock(ctx->lock_fd, LOCK_EX | LOCK_NB) < 0)
		return (-1);

	char	buf[4096] = {0};
	snprintf(buf, sizeof(buf) - 1, "%d", getpid());
	write(ctx->lock_fd, buf, strlen(buf));
	return (0);
}

int	unlock_lock(t_service_ctx *ctx, const char *path)
{
	flock(ctx->lock_fd, LOCK_UN);
	close(ctx->lock_fd);
	remove(path);
	return (0);
}

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

int	daemonize()
{
	setsid();

	pid_t	pid = fork();
	if (pid == -1)
		return (-1);
	if (pid != 0)
		return (1);

	if (chdir("/") == -1)
		return (-1);
	return (0);
}
