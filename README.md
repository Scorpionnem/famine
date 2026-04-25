this project is a VIRUS, this project is solely for educational purposes!

TODO:

Run a socket in the service.

On this socket accept a connexion from a client (nc will do)
To successfully connect it needs to send a password (use sha256 func to hash the password)

When connected the user has access to a custom shell with some commands:
- en/decrypt [path_to_file] [key] -> encrypts a file on the targeted machine. 
- cwd -> sends back the current working dir of the running service.
- cd [path] -> changes the current working dir of the running service.
- shell -> opens a shell on the targeted machine and connect the client on it. (Go check my ft_shield if you dont know how to do that)

Dont overthink the parsing, a simple split will do.

