## Table of contents
* [General info](#general-info)
* [Technologies](#technologies)
* [Setup](#setup)

## General info
This project is a simplified version of the Distance Vector Routing Protocol.

##Technologies
Project is created with:
*C environment
*Linux Socket API
*POSIX TREADS(Pthreads) library

## Setup
To run this project using any Linux Terminal

```
$ cd ../distanceVectorRouting
```


PREREQUISITES:
	If running on a windows machine use WSL(Windows Subsystem for Linux)
	Need to compile with pthreads linked, example: (gcc distance_vector_routing.c -o distance_vector_routing -lpthread)
	When running, specify the topology file and update interval, example (server -t topology0.txt -i 20)
FUNCTIONALITY:
	
