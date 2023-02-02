## Table of contents
* [General info](#general-info)
* [Prerequisites](#prerequisites)
* [Technologies](#technologies)
* [Setup](#setup)
* [Functionality](#functionality)
* [Example Inputs](#example-inputs)

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

## Example txt file inputs
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

