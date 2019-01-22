<center>

# अंतरण (Antaran)
> A multithreaded file transfer program implemented in C using UNIX Socket API over Transmission Control Protocol.

[![Build Status](https://travis-ci.com/apsknight/antaran.svg?branch=master)](https://travis-ci.com/apsknight/antaran)
</center>

<table>
<tr>
<td>
<img width="756" alt="screenshot 2019-01-23 at 12 53 10 am" src="https://user-images.githubusercontent.com/19551774/51559969-5b34bd00-1ea9-11e9-8600-b204a9614bab.png">
</td>
<td>
<img width="531" alt="" src="https://user-images.githubusercontent.com/19551774/51559968-5b34bd00-1ea9-11e9-885e-1b8f17829000.png">
</td>
</tr>
</table>

## Usage
```
./server [PORT]
```
```
./client [PORT] [SERVER_IPv4]
```
## Build & Test
- Clone repository
```bash
git clone https://github.com/apsknight/antaran.git && cd antaran/
```
- Build Executables
```bash
. ./build.sh
```
- Change to Server directory and Spawn Server
```bash
cd server_dir/
./server
```
- Create a new Terminal and Run client
```bash
cd client_dir/
./client
```
- Chose a file from available list of files and check if it gets downloaded to Client directory from server directory.

- To Clear executables, Run the following in the root directory.
```bash
. ./clean.sh
```

## Credits
- Brian “Beej Jorgensen” Hall for Beej's Guide to Network Programming

## LICENSE
MIT © [Aman Pratap Singh](https://aps.mit-license.org)
