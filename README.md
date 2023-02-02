# distanceVectorRouting
Simplified demonstration of the Distance Vector Routing Protocol.
Runs on top of 4 servers/laptops using UDP which immitate routers.

## Table of contents
* [General info](#general-info)
* [Technologies](#technologies)
* [Setup](#setup)


PREREQUISITES:
	If running on a windows machine use WSL(Windows Subsystem for Linux)
	Need to compile with pthreads linked, example: (gcc distance_vector_routing.c -o distance_vector_routing -lpthread)
	When running, specify the topology file and update interval, example (server -t topology0.txt -i 20)
FUNCTIONALITY:
	
