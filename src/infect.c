/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   infect.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 11:29:15 by mbatty            #+#    #+#             */
/*   Updated: 2026/04/13 17:57:53 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "famine.h"
#include <fcntl.h>
#include <unistd.h>

#define DIRENT_DIRECTORY 4

int	check_signature(const char *path);

int	infect_file(const char *path)
{
	if (check_signature(path) == -1)
		return (-1);

	int	fd = open(path, O_WRONLY | O_APPEND);
	if (fd == -1)
		return (-1);

	write(fd, SIGNATURE_STR, sizeof(SIGNATURE_STR));

	close(fd);
	return (0);
}
