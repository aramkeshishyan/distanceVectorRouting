## Table of contents
* [General info](#general-info)
* [Prerequisites](#prerequisites)
* [Technologies](#technologies)
* [Setup](#setup)
* [Functionality](#functionality)
* [Input file format and examples](#input-file-format-and-examples)
* [Available Commands](#available-commands)

## General info
This project is a simplified version of the Distance Vector Routing Protocol.

## PREREQUISITES:
If running on a windows machine use WSL(Windows Subsystem for Linux)

## Technologies
Project is created with:
* C environment
* Linux Socket API
* POSIX TREADS(Pthreads) library

## Setup
To run this project using any Linux Terminal
```
$ cd ../distanceVectorRouting
```
```
$ gcc distance_vector_routing.c -o distance_vector_routing -lpthread
```
```
$ ./distance_vector_routing
```

## Functionality
* Run the program using 4 instances of terminals to immitate 4 routers
* Info input is accepted using txt files
* Neighboring routers exchange link updates periodically invoking the step function


## Available Commands
Input a router's initial topology configuration and specify the automatic message exchange(update) interval
* **server -t topology-file-name -i routing-update-interval**
  
The following commands can be invoked at any point 
* **update server-ID1 server-ID2 LinkCost** : Assigns a new link cost between neighboring routers
  - server-ID1, server-ID2: The link for which the cost is being updated
  - Link Cost: Specifies the new link cost between the source and the destination server
  -- Ex: update 1 2 8: Change the cost of the link to 8
* **step**: Send a routing update to neighbors right away
* **packets**: Display the number of packets received since the last invocation of the command
* **display**: Display the current routing table
* **disable <server-ID>**: Disable the link to a given server
* **crash**: Close all connections. Simulates a crash of the router
  
  
  
## Input file format and examples
**Routers have each others information beforehand**
1 number of servers
2 number of neighbors
3 

  
**Examples**
Router #0
```
4
3
0 172.24.20.171 11111
1 172.24.20.171 22222
2 172.24.20.171 33333
3 172.24.20.171 44444
0 1 7
0 2 4
0 3 6
```
Router #1
```
4
2
0 172.24.20.171 11111
1 172.24.20.171 22222
2 172.24.20.171 33333
3 172.24.20.171 44444
1 0 7
1 2 2
```
Router #2
```
4
3
0 172.24.20.171 11111
1 172.24.20.171 22222
2 172.24.20.171 33333
3 172.24.20.171 44444
2 0 4
2 1 2
2 3 1
```
Router #3
```
4
2
0 172.24.20.171 11111
1 172.24.20.171 22222
2 172.24.20.171 33333
3 172.24.20.171 44444
3 0 6
3 2 1
```

