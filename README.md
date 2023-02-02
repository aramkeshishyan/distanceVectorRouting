# distanceVectorRouting
Simplified demonstration of the Distance Vector Routing Protocol.
Runs on top of 4 servers/laptops using UDP which immitate routers.

PREREQUISITES:
	If running on a windows machine
	Use WSL(Windows Subsystem for Linux)
	Use Ubuntu 20.04 GCC to compile
	Need to compile with pthreads linked, example: (gcc distance_vector_routing.c -o distance_vector_routing -lpthread)
	Run with ./  example: (./distance_vector_routing)
	After running, specify the topology file and update interval, example (server -t topology0.txt -i 20)
