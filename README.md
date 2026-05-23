# 🦠 Famine

> **⚠️ Educational purposes only. Do not deploy on systems you do not own or have explicit permission to test.**

A Linux ELF virus written in C as part of the **42 Post-CC** curriculum. Famine spreads by appending itself to ELF binaries, persists as a disguised systemd service, and exposes a password-protected remote C2 shell over TCP.

Made by **[mbatty](https://github.com/scorpionnem)** & **[pboucher](https://github.com/pgbmax)**.

---

## Table of Contents

- [How It Works](#how-it-works)
- [Infection Format](#infection-format)
- [Features](#features)
- [Architecture](#architecture)
- [C2 Shell](#c2-shell)
- [Build](#build)

---

## How It Works

When an infected binary is executed, three things happen concurrently:

```
Infected binary runs
        │
        ├──► [Parent fork]
        │       └─ Extracts embedded payload from footer → writes to memfd → execve()
        │          (the original binary runs transparently in RAM, nothing touches disk)
        │
        └──► [Child fork] mute stdout/stderr → setsid() → fork() again
                  │
                  ├──► [Main process] crawl /tmp/test, /tmp/test2 → infect ELF files
                  │
                  └──► [Service process]
                            ├─ Acquire lock file (prevent duplicate instances)
                            ├─ If root: copy self to /bin/famine + install systemd service
                            └─ Open C2 TCP server on port 6942
```

### Step by step

1. **Transparent execution** — the parent forks, reads the original binary out of the footer, drops it into an anonymous `memfd` (never touches disk), then `execve`s it with the original `argv`/`envp`. From the user's perspective the program just runs normally.

2. **Daemonize** — the child calls `setsid()` to detach from the controlling terminal, then forks again. stdout and stderr are silenced by redirecting to `/dev/null` first.

3. **Crawl & infect** — the main daemon process walks `/tmp/test` and `/tmp/test2` recursively. For each file it:
   - Checks the ELF magic bytes and validates the header (supports x86 and x86_64, all endiannesses, `ET_REL`/`ET_EXEC`/`ET_DYN`)
   - Checks whether a Famine footer is already present — if so, skips the file
   - Packs `[virus bytes][original file bytes][footer]` into a `.tmp` file then renames it over the target

4. **Persistence** — if running as root, the service process copies the binary to `/bin/famine` and writes `/etc/systemd/system/famine.service` (disguised as *"Fitness app to track eating habits"*), then enables and starts it. The service is configured with `Restart=always`, so it survives reboots and kills.

5. **C2 server** — a `poll(2)`-based multi-client TCP server starts on port **6942**. Incoming clients must authenticate with a password before issuing commands.

---

## Infection Format

The packed binary layout on disk looks like this:

```
┌─────────────────────┬──────────────────────┬──────────────────────────────┐
│   Virus binary      │   Original binary    │          Footer              │
│   (self)            │   (payload)          │  magic | signature | size    │
└─────────────────────┴──────────────────────┴──────────────────────────────┘
                                              ↑
                             0x4242424242424242  ← magic sentinel
```

- **Magic**: `0x4242424242424242` — used to detect already-infected files and to locate the footer at runtime
- **Signature**: `"Famine version 1.0 (c)oded by mbatty-pboucher"` — human-readable marker
- **Payload size**: size in bytes of the embedded original binary

At runtime, `extract_payload()` seeks to `-(sizeof(footer) + payload_size)` from the end of the file to read back the original binary.

---

## Features

| Feature | Details |
|---|---|
| **Append-based infection** | Packs virus + original binary together; no ELF section patching |
| **Double-infection guard** | Footer magic check skips already-infected files |
| **In-memory execution** | Original binary runs via `memfd_create` + `execve(/proc/self/fd/N)` — never written to disk |
| **Anti-duplicate** | `flock(LOCK_EX\|LOCK_NB)` on a lock file prevents multiple service instances |
| **Persistence** | Copies to `/bin/famine` + systemd unit with `Restart=always` (root only) |
| **C2 server** | `poll(2)` TCP server, up to 3 simultaneous clients, port **6942** |
| **SHA-256 auth** | Password verified against a hardcoded SHA-256 hash (hand-rolled implementation) |
| **File encryption** | XOR each byte with `SHA-256(password)[i % 32]` — same operation encrypts and decrypts |
| **ELF validation** | Validates magic, version, class (ELF32/ELF64), machine (x86/x86_64), and type |

---

## Architecture

```
src/
├── main.c               Entry point: fork parent/child, dispatch crawl vs service
├── daemon.c             setsid + fork daemonize, lock file helpers, mute outputs
├── crawl.c              Recursive directory walker (targets /tmp/test, /tmp/test2)
├── infect.c             Per-file infection: ELF check → signature check → pack
├── check.c              ELF header validation + footer signature check
├── payload.c            pack_payload(), extract_payload(), exec_payload() via memfd
├── service.c            Lock, systemd install, C2 server bootstrap, command handler
├── sha256.c             Hand-rolled SHA-256 (auth + encryption key derivation)
├── utils.c              strjoin
├── server/
│   ├── server.c         Socket open/close, send helpers, hook registration
│   ├── server_clients.c Client accept, disconnect, list management
│   ├── server_update.c  poll() loop, line-based and raw (file transfer) read modes
│   └── server_utils.c   Internal string helpers
└── list/
    ├── list.c           Generic doubly-linked list
    └── list_node.c      Node alloc / free
```

---

## C2 Shell

Connect from anywhere with:

```bash
nc <target_ip> 6942
```

On connect the server sends a greeting and a password prompt. The password is verified by comparing `SHA-256(input)` against a hardcoded hash.

```
Lets do fitness! 🏃
Password 🐈: <your password>
Yay you get a cookie! 🍪
$Famine 🍖>
```

### Available commands

| Command | Description |
|---|---|
| `help` | Print all available commands |
| `getcwd` | Print the service's current working directory |
| `cd <path>` | Change the service's working directory |
| `encrypt <password> <file>` | XOR-encrypt a file in-place (key = SHA-256 of password) |
| `decrypt <password> <file>` | XOR-decrypt a file in-place (same operation, symmetric) |
| `quit` | Stop the C2 server |

> The server also supports a raw `transfer:<size>` mode for receiving binary files from the client.

---

## Build

```bash
make          # build
make re       # clean rebuild
make clean    # remove objects
make fclean   # remove objects + binary
```

Requires `cc` and standard POSIX/Linux headers. No external libraries.

---

## Disclaimer

This project was developed strictly within the **42 school** educational framework to study low-level binary formats, process management, and network programming. The authors take no responsibility for any misuse. Running this software against systems without explicit authorization is **illegal**.
