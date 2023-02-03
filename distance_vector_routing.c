#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <pthread.h>


struct AcceptedSocket
{
	int id;
	char ipAddress[100];
	int portNum;
};

int packets = 0;
int disabledServId = -1;

int currentServId;
int currentServPort;
char* currentServIP;

int numServs;
int numNeighbors;
int neighbors[3] = { -1, -1 ,-1 };
struct AcceptedSocket serversInfo[4];

int linkCost[4][4];
int nextHop[4] = { 0, -1, -1, -1};

char * ReadFile(char *filename);
void fillServerInfo(char * infoString);
struct sockaddr_in* createAddress(char *ip, int port);
void receiveIncomingMessagesOnSeparateThread(int * server_socketfd);
void * receiveAndPrintIncomingMessages(void * voidserver_socketfd);
void sendLinkUpdate(char* sourceId, char* destId, char* cost);
void Display();
void computeMyNewDV(int senderId, int status);
void sendCurrentVectorToNeighbor(int destId);
void * sendCurrentVectorToNeighbors();
void sendingPeriodicUpdatesOnSeparateThread(int delay);
void disableServer(int servId);
void crash();
void crashNotifyNeighbor(int destId);

int main(int argc, char * argv[])
{
	if(argc < 1)
	{
		printf("Input should be in this format: <filename.c>");
		exit(1);
	}

	char command[100];
	char *token;
	char *temp;
	char *topologyFile;
	char *updateInterval;

	//printf("Input command:\nserver -t <topology file name> -i <routing update interval>\n");          
	scanf("%[^\n]s", command);																	
	token = strtok(command, " ");		
	token = strtok(NULL, " ");
	token = strtok(NULL, " ");
	topologyFile = token;

	token = strtok(NULL, " ");
	token = strtok(NULL, " ");
	updateInterval = token;
	



	//reading in from the topology file-----------------------------------------------
	char * fileContents = ReadFile(topologyFile);														
	//char * fileContents = ReadFile(argv[1]);

	if(fileContents == NULL)
	{
		printf("Error reading the file.\n");
		return 1;
	}
	//printf("File Contents:\n%s\n\n", fileContents);
	fillServerInfo(fileContents);
	free(fileContents);
	//---------------------------------------------------------------------------------


	//Setting up the server socket to receive messages---------------------------------
	currentServPort = serversInfo[currentServId].portNum;
	currentServIP = serversInfo[currentServId].ipAddress;

	int server_socketfd;
	server_socketfd = socket(AF_INET, SOCK_DGRAM,0);
	if(server_socketfd < 0)
	{
		printf("Error when opening the socket");
		exit(1);
	}

	struct sockaddr_in *server_addr = createAddress(currentServIP, currentServPort);

	if(bind(server_socketfd, (struct sockaddr *) server_addr, sizeof(*server_addr)) < 0)
	{
		printf("ERROR, binding wasn't successful\n");
		exit(1);
	}

	int *pserver_socketfd = malloc(sizeof(int));
	*pserver_socketfd = server_socketfd;
	receiveIncomingMessagesOnSeparateThread(pserver_socketfd);
	//---------------------------------------------------------------------------------

	sendingPeriodicUpdatesOnSeparateThread(atoi(updateInterval));                           
	//sendingPeriodicUpdatesOnSeparateThread(10);
	//sendCurrentVectorToNeighbors();
	//send my distance vector to my neighbors

	//make a calculation and send it to neighbors

	//computeMyNewDV();
	//sendCurrentVectorToNeighbor(1);


	
	char *ip;
	char *sender;
	int servID;

	//printf("Please enter server id");
	//scanf("%d", &servID);
	//sendLinkUpdate(servID,4);
	while ((getchar()) != '\n');

	do
	{
		token = NULL;
		temp = NULL;
		//printf("Please enter a command.\n");
		scanf("%[^\n]s", command);
		token = strtok(command, " ");


		while ((getchar()) != '\n');

		if(!strcmp(token, "update"))
		{
			token = strtok(NULL, " ");
			sender = token;
			token = strtok(NULL, " ");
			temp = token;
			token = strtok(NULL, " ");
			printf("Update from %s to %s, New Cost: %s\n", sender, temp, token);
			sendLinkUpdate(sender, temp, token);
		}
		else if(!strcmp(token, "step"))
		{
			int i = 0;
			while(neighbors[i] != -1 && i <= 2)
			{
				printf("Neighbor: %d\n", neighbors[i]);
				sendCurrentVectorToNeighbor(neighbors[i]);
				i++;
			}
			printf("STEP SUCCESS\n");
		}
		else if(!strcmp(token, "packets"))
		{
			printf("Packets Received: %d\n", packets);
			packets = 0;
			printf("PACKETS SUCCESS\n");
		}
		else if(!strcmp(token, "display"))
		{	
			Display();
			printf("DISPLAY SUCCESS\n");
		}
		else if(!strcmp(token, "disable"))
		{
			token = strtok(NULL, " ");
			disableServer(atoi(token));
		}
		else if(!strcmp(token, "crash"))
		{
			crash();
		}
		else
		{
			printf("Not a valid command.\n");
		}

	}while(strcmp(token, "crash") != 0);



	return 0;
}


void crash()
{

	for(int i=0; i < numNeighbors;i++)
	{
		if(neighbors[i] != -1)
		{
			crashNotifyNeighbor(neighbors[i]);
		}
		
	}
	
	printf("CRASH SUCCESS\n");
}

void crashNotifyNeighbor(int destId)
{
	char temp[100];
	char updateMessage[200] = "";
	int sendToFd = socket(AF_INET, SOCK_DGRAM,0);
	struct sockaddr_in *server_addr = createAddress(serversInfo[destId].ipAddress, serversInfo[destId].portNum);

	sprintf(temp, "%d", currentServId);

	//send to the neighbor
	strcat(updateMessage, "7 ");
	strcat(updateMessage, temp);
	//printf("%s\n", updateMessage);

	int amountWasSent = sendto(sendToFd, (const char*)updateMessage, strlen(updateMessage), 0, (const struct sockaddr*)server_addr, sizeof(*server_addr));
	if(amountWasSent == -1)
	{
		printf("Couldn't send the message.\n");
	}
	else
		//printf("Message sent to %d\n", destId);


	close(sendToFd);

}



void disableServer(int servId)
{
	//make sure this server is a neighbor
	if((servId == neighbors[0] || servId == neighbors[1] || servId == neighbors[2]) && servId != -1 && servId < 4)
	{
		disabledServId = servId;
		printf("DISABLE SUCCESS\n");
	}
	else
	{
		printf("disable FAILED: Not a Neighbor\n");
		return;
	}
	
}


void computeMyNewDV(int senderId, int status)
{	
	int newComp; 
	//printf("Computing the new DV using: %s\n", neighborsDV);

	if(status == 0)
	{
		for(int i = 0; i < numServs; i++)
		{
			newComp = linkCost[currentServId][senderId] + linkCost[senderId][i];

			if(newComp < linkCost[currentServId][i])
			{
				linkCost[currentServId][i] = newComp;
				nextHop[i] = senderId;
			}
		}
	}
	else //update path to non neighbors
	{
		for(int i = 3; i >= 0; i--)
		{
			if(i != neighbors[0] && i != neighbors[1] && i != neighbors[2] && i != currentServId)
			{
				linkCost[currentServId][i] = linkCost[currentServId][senderId] + linkCost[senderId][i];
				nextHop[i] = senderId;
				break;
			}

		}
		
	}
	
}


void sendingPeriodicUpdatesOnSeparateThread(int delay)
{
	pthread_t id;
	pthread_create(&id, NULL, sendCurrentVectorToNeighbors, (void *)&delay);

}



void * sendCurrentVectorToNeighbors(void * delay)
{
	//call sendcurrentvectortoneighbor function on all neighbors
	//this sends the update to all neighbors periodically

	int delayInSeconds = *((int *)delay);
	

	while (1)
    {
    	sleep(delayInSeconds);

    	int i = 0;
		while(neighbors[i] != -1 && i <= 2)
		{
			//printf("Neighbor: %d\n", neighbors[i]);
			sendCurrentVectorToNeighbor(neighbors[i]);
			i++;
		}
        
        
    }

}


void sendCurrentVectorToNeighbor(int destId)
{
	int myCurrentDV[4];

	for(int i = 0; i < numServs; i++)
	{
		myCurrentDV[i] = linkCost[currentServId][i];
	}

	char updateMessage[200] = "";
	char temp[100];

	//Putting together a string with the following  message format: messageType sourceIP sourcePort destIP destPort costTo0 CostTo1 CostTo2 CostTo3

	strcat(updateMessage, "8");
	strcat(updateMessage, " ");

	strcat(updateMessage,serversInfo[currentServId].ipAddress);
	strcat(updateMessage, " ");

	sprintf(temp, "%d", serversInfo[currentServId].portNum);
	strcat(updateMessage,temp);
	strcat(updateMessage, " ");

	strcat(updateMessage,serversInfo[destId].ipAddress);
	strcat(updateMessage, " ");

	sprintf(temp, "%d", serversInfo[destId].portNum);
	strcat(updateMessage, temp);
	strcat(updateMessage, " ");

	for(int i = 0; i < numServs; i++)
	{
		sprintf(temp, "%d", myCurrentDV[i]);
		strcat(updateMessage, temp);
		strcat(updateMessage, " ");
	}

	//printf("%s\n", updateMessage);
	

	int sendToFd = socket(AF_INET, SOCK_DGRAM,0);
	struct sockaddr_in *server_addr = createAddress(serversInfo[destId].ipAddress, serversInfo[destId].portNum);

	//send the message in the format above to a neighbor
	int amountWasSent = sendto(sendToFd, (const char*)updateMessage, strlen(updateMessage), 0, (const struct sockaddr*)server_addr, sizeof(*server_addr));
	if(amountWasSent == -1)
	{
		printf("Couldn't send the message.\n");
	}
	else
		//printf("Update Message sent to %d\n", destId);


	close(sendToFd);
	
}


void sendLinkUpdate(char* sourceId, char* destId, char* cost)
{
	//modify my table
	linkCost[currentServId][atoi(destId)] = atoi(cost);
	nextHop[atoi(destId)] = atoi(destId);

	//char * hello = "hello from client";
	char updateMessage[200] = "";
	int sendToFd = socket(AF_INET, SOCK_DGRAM,0);
	struct sockaddr_in *server_addr = createAddress(serversInfo[atoi(destId)].ipAddress, serversInfo[atoi(destId)].portNum);

	//send to the neighbor

	strcat(updateMessage, "3 ");
	strcat(updateMessage, sourceId);
	strcat(updateMessage, " ");
	strcat(updateMessage, destId);
	strcat(updateMessage, " ");
	strcat(updateMessage, cost);

	//printf("SENDING THIS: %s\n", updateMessage);


	int amountWasSent = sendto(sendToFd, (const char*)updateMessage, strlen(updateMessage), 0, (const struct sockaddr*)server_addr, sizeof(*server_addr));
	if(amountWasSent == -1)
	{
		printf("Couldn't send the message.\n");
	}
	else
		printf("Message sent to %s\n", destId);


	close(sendToFd);
}



void receiveIncomingMessagesOnSeparateThread(int * server_socketfd)
{
	pthread_t id;
	pthread_create(&id, NULL, receiveAndPrintIncomingMessages, (void *)server_socketfd);
}

void * receiveAndPrintIncomingMessages(void * voidserver_socketfd)
{
	int server_socketfd = *((int*)voidserver_socketfd);
	socklen_t len = 0;
	char buffer[150];


	char *token;
	char *temp;
	char *senderIp;
	char *senderPort;
	char *whosCost;
	int senderId;
	char *crashedId;

	//MessageType 8 is regular DV updates
	//MessageType 3 is link cost change update

	while(1)
	{	
		//message format: messageType sourceIP sourcePort destIP destPort costTo0 CostTo1 CostTo2 CostTo3

		int n = recvfrom(server_socketfd, (char*)buffer, 150, MSG_WAITALL, 0, &len);
		//printf("RECEIVED THIS: %s\n", buffer);
		//printf("n is: %d\n", n);
		token = strtok(buffer, " ");
		//printf("First received token is: %s", token);

		if(!strcmp(token, "8"))	//if a regular DV update do this...
		{	
			token = strtok(NULL, " ");
			senderIp = token;
			
			token = strtok(NULL, " ");
			senderPort = token;

			token = strtok(NULL, " ");
			token = strtok(NULL, " ");


			//printf("Sender's IP: %s\n", senderIp);
			//printf("Sender's Port: %s\n", senderPort);
			
			//find senders ID
			for(int i = 0; i < numServs;i++)
			{
				if(!strcmp(senderIp, serversInfo[i].ipAddress) && atoi(senderPort) == serversInfo[i].portNum)
				{
					senderId = serversInfo[i].id;
					break;
				}
			
			}

			//add senders DV to the DV table and call computeMyNewDV 
			for(int i = 0; i < numServs;i++)
			{
				token = strtok(NULL, " ");
				linkCost[senderId][i] = atoi(token);
			}
			

			printf("RECEIVED A MESSAGE FROM SERVER: %d\n", senderId);

			computeMyNewDV(senderId, 0);
			buffer[n] = 0;
			memset(buffer, 0, sizeof(buffer));
			packets++;
		}
		else if(!strcmp(token, "3")) //if a cost change update do this...
		{
			token = strtok(NULL, " ");
			whosCost = token;
			//printf("RECEIVED whosCost is: %s\n", whosCost);
			token = strtok(NULL, " ");
			token = strtok(NULL, " ");


			//printf("RECEIVED Cost is: %s\n", token);
			linkCost[currentServId][atoi(whosCost)] = atoi(token);
			nextHop[atoi(whosCost)] = atoi(whosCost);
			computeMyNewDV(atoi(whosCost), 1);


			buffer[n] = 0;
			memset(buffer, 0, sizeof(buffer));
			packets++;
		}
		else if(!strcmp(token, "7"))// if crash notification
		{
			token = strtok(NULL, " ");
			crashedId = token;
			printf("Received a crash notification from: %s\n", crashedId);

		
			linkCost[currentServId][atoi(crashedId)] = 9999;

			for(int i = 0; i < numServs; i++)
			{
				linkCost[atoi(crashedId)][i] = 9999;
			}
		}

		
	}

}


//Reads data from the string(that has info read in from the topology file) and stores info in variables
void fillServerInfo(char * infoString)
{	
	for(int i = 0; i < 4;i++)
	{
		for(int j = 0; j < 4;j++)
		{
			linkCost[i][j] = 999;
		}
		printf("\n");
	}


	char* token;

	//Using tokens to fill in the data about the server from the file contents that were put in the string infoString
	token = strtok(infoString, "\n");
	numServs = atoi(token);

	token = strtok(NULL, "\n");
	numNeighbors = atoi(token);

	for(int i = 0; i < 4;i++)
	{
		token = strtok(NULL, " ");
		serversInfo[i].id = atoi(token);

		token = strtok(NULL, " ");
		strcpy(serversInfo[i].ipAddress, token);

		token = strtok(NULL, "\n");
		serversInfo[i].portNum = atoi(token);
	}

	/*
	printf("Server Num: %d\n", numServs);
	printf("Server nei: %d\n", numNeighbors);
	for(int i = 0; i < 4;i++)
	{
		printf("Server %d id: %d  ", i, serversInfo[i].id);
		printf("ip: %s  ", serversInfo[i].ipAddress);
		printf("port: %d\n", serversInfo[i].portNum);
	}
	*/

	int index = 0;

	token = strtok(NULL, " ");
	currentServId = atoi(token);
	linkCost[currentServId][currentServId] = 0;

	int row = currentServId;
	int column;

	token = strtok(NULL, " ");
	column = atoi(token);
	token = strtok(NULL, "\n");
	linkCost[row][column] = atoi(token);
	neighbors[index] = column; //storing who's the first neightbor
	nextHop[column] = column; //get to server x by hopping to server x (since we don't have others DV's yet)
	index++;

	token = strtok(NULL, " ");
	token = strtok(NULL, " ");
	column = atoi(token);
	token = strtok(NULL, "\n");
	linkCost[row][column] = atoi(token);

	neighbors[index] = column; //storing second neighbor #
	nextHop[column] = column;
	index++;

	token = strtok(NULL, " ");
	if(token != NULL)
	{
		token = strtok(NULL, " ");
		column = atoi(token);
		token = strtok(NULL, "\n");
		linkCost[row][column] = atoi(token);

		neighbors[index] = column; //storing third neighbor #
		nextHop[column] = column;
	}

}

//reads from the file, stores in a string and returns
char * ReadFile(char *filename)
{
	FILE *file;

	file = fopen(filename, "r");
	if(file == NULL)
		return NULL;

	fseek(file, 0, SEEK_END);
	int length = ftell(file);
	fseek(file, 0, SEEK_SET);

	char *string = malloc(sizeof(char) * (length+1));

	char c;
	int i = 0;
	while( (c = fgetc(file)) != EOF)
	{
		string[i] = c;
		i++;
	}

	string[i] = '\0';
	fclose(file);

	return string;
}

struct sockaddr_in* createAddress(char *ip, int port)
{
	struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
	address->sin_family = AF_INET;
	address->sin_port = htons(port);


	if(strlen(ip) == 0)
		address->sin_addr.s_addr = INADDR_ANY;
	else
		inet_pton(AF_INET, ip, &address->sin_addr.s_addr);


	return address;
}


void Display()
{
	/*
	printf("    0     1       2       3\n");
	printf("---------------------------\n");

	for(int i = 0; i < 4;i++)
	{
		printf("%d|", i);
		for(int j = 0; j < 4;j++)
		{
			printf("%3d\t", linkCost[i][j]);
		}
		printf("\n");
	}
	*/

	printf("\nDest   NextHop   Cost\n");

	for(int i = 0; i < numServs;i++)
	{
		if(i != currentServId && i != disabledServId)
			printf("%3d\t %2d\t %3d\t\n", i, nextHop[i], linkCost[currentServId][i]);
	}



}