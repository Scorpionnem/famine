/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 11:30:42 by mbatty            #+#    #+#             */
/*   Updated: 2026/04/19 11:01:38 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "famine.h"

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>

int	check_elf_hdr(const char *path)
{
	int	fd = open(path, O_RDONLY);

	struct stat	stats;
	fstat(fd, &stats);

	size_t	size = stats.st_size;
	void	*map = mmap(NULL, size, PROT_READ, MAP_PRIVATE, fd, 0);

	Elf64_Ehdr	*hdr;
	int			class;
	int			is_little_endian;
	int			is_x64;
	int			data;

	if (size < sizeof(Elf64_Ehdr))
		goto _error;

	hdr = map;

	if (memcmp(hdr->e_ident, ELFMAG, sizeof(ELFMAG) - 1))
		goto _error;

	if (hdr->e_ident[EI_VERSION] != 1)
		goto _error;

	class = hdr->e_ident[EI_CLASS];
	if (class != ELFCLASS32 && class != ELFCLASS64)
		goto _error;
	is_x64 = class == ELFCLASS64;

	data = hdr->e_ident[EI_DATA];
	if (data != ELFDATA2LSB && data != ELFDATA2MSB)
		goto _error;
	is_little_endian = data == ELFDATA2LSB;

	int	type = is_little_endian ? le16toh(hdr->e_type) : be16toh(hdr->e_type);
	if (type != ET_REL && type != ET_EXEC && type != ET_DYN)
		goto _error;

	int	machine = is_little_endian ? le16toh(hdr->e_machine) : be16toh(hdr->e_machine);
	if ((!is_x64 && machine != EM_386) || (is_x64 && machine != EM_X86_64))
		goto _error;

	int version = is_little_endian ? le32toh(hdr->e_version) : be32toh(hdr->e_version);
	if (version != 1)
		goto _error;

	munmap(map, size);
	close(fd);
	return (0);
	_error:
	munmap(map, size);
	close(fd);
	return (-1);
}

int	check_signature(const char *path)
{
	t_footer	footer = get_footer(path);
	if (footer.magic == FOOTER_MAGIC)
		return (-1);
	return (0);
}
