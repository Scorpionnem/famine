/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 12:30:09 by mbatty            #+#    #+#             */
/*   Updated: 2026/03/04 11:52:05 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infect.h"

#include <fcntl.h>
#include <unistd.h>

int	mute_outputs()
{
	int	fd = open("/dev/null", O_RDONLY);
	if (fd == -1)
		return (-1);
	
	dup2(fd, STDOUT_FILENO);
	dup2(fd, STDERR_FILENO);

	close(fd);
	return (0);
}

int	main(void)
{
	mute_outputs();
	infect_dir("/tmp/test");
	infect_dir("/tmp/test2");
	return (0);
}
