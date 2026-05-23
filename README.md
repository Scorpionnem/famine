# 🦠 Famine

> **⚠️ Educational purposes only. Do not deploy on systems you do not own or have explicit permission to test.**

A Linux ELF virus written in C as part of the **42 Post-CC** curriculum. Famine infects ELF binaries, persists as a disguised systemd service, and exposes a password-protected remote C2 shell over TCP.

Made by **[mbatty](https://github.com/scorpionnem)** & **[pboucher](https://github.com/pgbmax)**.

---

## Table of Contents

- [How It Works](#how-it-works)
- [Features](#features)
- [Architecture](#architecture)
- [C2 Shell Commands](#c2-shell-commands)
- [Build](#build)
- [Usage](#usage)

---

## How It Works

When an infected binary is executed, Famine:

1. **Forks** — the parent extracts and executes any embedded payload (the original host binary), so execution looks normal to the user.
2. **Daemonizes** — the child detaches from the terminal and runs silently in the background.
3. **Crawls** the filesystem recursively, appending itself to every valid ELF binary it can reach, while skipping already-infected files (signature check).
4. **Persists** by installing itself as a systemd service disguised as `"Fitness app to track eating habits"` (yes, really).
5. **Listens** on a TCP port for remote C2 connections.

```
Infected binary executed
        │
        ├──► [Parent] Extract & exec original payload → normal program runs
        │
        └──► [Child] Daemonize
                  │
                  ├──► [Main process] Crawl & infect ELF binaries
                  │
                  └──► [Service]  C2 TCP server
```

---

## Features

| Feature | Details |
|---|---|
| **ELF infection** | Appends the virus + original binary as a footer payload; detects & skips already-infected files |
| **Signature check** | Magic number `0x4242424242424242` + human-readable signature string prevent double-infection |
| **Anti-duplicate** | Lock file (`/var/lock/famine.lock` or `/tmp/famine.lock`) prevents multiple instances |
| **Persistence** | Installs `/bin/famine` and registers a systemd service with `Restart=always` |
| **C2 server** | Multi-client TCP server (up to 3 simultaneous connections) using `poll(2)` |
| **SHA-256 auth** | Connection password is hashed with a hand-rolled SHA-256 implementation |
| **File encryption** | XOR-based encrypt/decrypt over arbitrary files, keyed by a user-supplied password |
| **Remote shell** | Spawns a `/bin/sh` child on the target and bridges its I/O to the connected client |
| **Privilege-aware** | Runs on port **4242** as root, **6967** as a regular user |

---

## Architecture

```
src/
├── main.c          — Entry point: fork, daemonize, dispatch
├── daemon.c        — Double-fork daemonize logic
├── crawl.c         — Recursive directory crawler
├── infect.c        — ELF infection (append virus + pack payload)
├── check.c         — Signature & ELF header validation
├── payload.c       — Payload extraction & exec
├── service.c       — Lock file, systemd service install, C2 bootstrap
├── sha256.c        — SHA-256 implementation (used for auth)
├── utils.c         — strjoin and misc helpers
├── server/
│   ├── server.c         — Socket init, poll loop
│   ├── server_clients.c — Client accept / disconnect
│   ├── server_update.c  — Command dispatch per client
│   └── server_utils.c   — Send helpers, prompt
└── list/
    ├── list.c       — Generic doubly-linked list
    └── list_node.c  — Node alloc / free
```

---

## C2 Shell Commands

Connect with `nc <target> 4242` (root) or `nc <target> 6967` (user), then enter the password.

```
$Famine 🍖> help
```

| Command | Description |
|---|---|
| `help` | List all available commands |
| `getcwd` | Print the current working directory of the service |
| `cd <path>` | Change the service's working directory |
| `encrypt <password> <file>` | XOR-encrypt a file in-place |
| `decrypt <password> <file>` | XOR-decrypt a file in-place |
| `quit` | Gracefully close the virus |

---

## Build

```bash
# Build
make

# Rebuild from scratch
make re

# Clean objects
make clean

# Clean everything
make fclean
```

> Requires `gcc` / `cc` and standard POSIX headers. No external dependencies.

---

## Usage

```bash
# Pack a payload (embed target_binary inside virus)
./Famine --pack <target_binary> <output>

# Run an infected binary — it behaves normally while Famine runs in the background
./<infected_binary>

# Connect to the C2 shell (from another machine or localhost)
nc <target_ip> 4242
```

---

## Disclaimer

This project was developed strictly within the **42 school** educational framework to study low-level binary formats, process management, and network programming. The authors take no responsibility for any misuse. Running this software against systems without explicit authorization is **illegal**.

