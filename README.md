# udp-chat-client

Simple two-way terminal based chat client I wrote in C for a job interview. Some UDP socket programming, I use poll to avoid multiple threads and blocking recv calls. Lacks documentation and edge-case testing as it's more of a proof-of-concept thing.
Takes the following parameters: local-port remote-host remote-port chat-alias.
Run the compiled binary or compile for yourself with the makefile/gcc.
Developed on Ubuntu 22.04.1 LTS virtualmachine, tested on localhost with two terminal sessions and on different virtual machines on bridged host network.

Example use: ./client 8080 127.0.0.1 9090 alias1
              ./client 9090 127.0.0.1 8080 alias2
