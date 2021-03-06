## HTTP/1.1 Web Server
_Project web server for processing HTTP/1.0 and HTTP/1.1 requests_

### Usage
_Tested using GCC v.7.4.0 && GNU make v.4.1_

#### Compiling
1. In terminal, `cd` to project root
2. Compile with `$ make`
3. `$ make clean` for cleaning the project directory

#### Example
* With the project compiled, from the project root start the server with `./server`
* Once running, requests can now be made to the server:
```
$ telnet {hostname} 4077
  GET {filename} HTTP/1.1
  Host: {hostname}
  Expect: 100-continue  
```
