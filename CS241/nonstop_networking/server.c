/**
 * nonstop_networking
 * CS 241 - Fall 2020
 */

 //Following flowchart at http://cs241.cs.illinois.edu/assignments/networking_mp
 //dict.h provided for connection state
 //use vector for file list
 //1024 byte buffer for file
 //mkdtemp() and print_temp_directory immediately
 //unlink() and rmdir() helpful for exit cleaning of directory
 //make sure that your template is exactly 6 X’s, as in XXXXXX      Not sure what this is yet?
 //Exit on sigint. sigaction may be useful
 //Do not store the newlines in your filenames. There will be no whitespace or slashes in filenames at all.
 //SIGPIPE - you should setup your program to ignore that signal. Just a blank function right?
 //This may also result in write() calls returning -1 with errno set to EPIPE. 
 //		If this happens, your server should also close the connection and clean up
#include <stdio.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/types.h> 
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#include "../includes/dictionary.h"
#include "../includes/vector.h"
#include "common.h"

#include <dirent.h>

static dictionary* sizeDict;//mapper
static dictionary* clientDict;//Dictionary mentioned above
static vector* files;//list of all files
static char* directory;//curr
static int epollFiledes;

// https://man7.org/linux/man-pages/man2/epoll_wait.2.html
// struct epoll_event {
	// uint32_t     events;    /* Epoll events */
	// epoll_data_t data;      /* User data variable */
// };

//Struct for maintaining connection info. This will change a lot
typedef struct clientReq {
	verb request;
	char file[1024];
	char headers[1024];
	size_t size;
	status stage;
} clientReq;

void printDirectory();//Prints out local directory (server's directory) for debugging only

void epollChangeSettings(int filedes);
void exitServer();

//To be called when pipe signal received
void blank(){
}

void processBody(int filedes, clientReq* reqStruct) {

	if (reqStruct->request == PUT) {// PUT
		write_all_to_socket(filedes, "OK\n", 3);
	} 
	if (reqStruct->request == GET) {//GET 
		size_t size = strlen(directory) + 1 + strlen(reqStruct->file) + 1;
		char fullPath[size];
		sprintf(fullPath, "%s/%s", directory, reqStruct->file);//terminator is automatically placed after
		FILE* readFile = fopen(fullPath, "r");
		if (!readFile) {
			reqStruct->stage = NO_SUCH_FILE;
			return;
		}
		write_all_to_socket(filedes, "OK\n", 3);
		size = *((size_t *)dictionary_get(sizeDict, reqStruct->file));
		write_all_to_socket(filedes, (char*)&size, sizeof(size_t));
		size_t bytesWrote = 0;
    	while (bytesWrote < size) {
      		size_t requestSize = size-bytesWrote;
      		if( requestSize > 1024){
				  requestSize = 1024;
			}
      		char buf[requestSize + 1];
      		fread(buf, 1, requestSize, readFile);
      		write_all_to_socket(filedes, buf, requestSize);
      		bytesWrote += requestSize;
    	}
		fclose(readFile);
	}
	else if (reqStruct->request == DELETE) {// DELETE
		size_t size = strlen(directory) + 1 + strlen(reqStruct->file) + 1;
		char fullPath[size];
		sprintf(fullPath, "%s/%s", directory, reqStruct->file);
		if (remove(fullPath) == -1) {
			reqStruct->stage = NO_SUCH_FILE;
			return;
		}
		size_t index = 0;
		VECTOR_FOR_EACH(files, file, {
	        if (!strcmp((char*) file, reqStruct->file)) {
				break;
			}
			index++;
	 	});
		if (index == vector_size(files)) {//file not found in vector
			reqStruct->stage = NO_SUCH_FILE;
			return;
		}
		vector_erase(files, index);
		dictionary_remove(sizeDict, reqStruct->file);
		write_all_to_socket(filedes, "OK\n", 3);
	} 
	else if (reqStruct->request == LIST) {// LIST
		write_all_to_socket(filedes, "OK\n", 3);
		size_t listPayloadLength = 0;
		//LOG("%zu", vector_size(files));
		VECTOR_FOR_EACH(files, file, {
	         listPayloadLength += strlen(file)+1;//file string length + newline
	    });
		write_all_to_socket(filedes, (char*)&listPayloadLength, sizeof(size_t));
		VECTOR_FOR_EACH(files, file, {
			write_all_to_socket(filedes, "\n", 1);
			write_all_to_socket(filedes, file, strlen(file));
		});
	}

	epoll_ctl(epollFiledes, EPOLL_CTL_DEL, filedes, NULL);
  	shutdown(filedes, SHUT_RDWR);
	free(dictionary_get(clientDict, &filedes));
	dictionary_remove(clientDict, &filedes);
	close(filedes);
}

bool putHelper(int filedes, clientReq *reqStruct) {
	size_t size = strlen(directory) + 1 + strlen(reqStruct->file) + 1;//two string + '/' + '\0'
	char fullPath[size];
	sprintf(fullPath, "%s/%s", directory, reqStruct->file);
	FILE* readFile = fopen(fullPath, "r");
	FILE* writeFile = fopen(fullPath, "w");
	//LOG("%s", fullPath);
	if (!writeFile) {
		perror(NULL);
		return true;
	}
	read_all_from_socket(filedes, (char*) & size, sizeof(size_t));
	
	size_t bytesRead = 0;
	while (bytesRead < (5 + size)) {
		size_t headerSize = size + 5 - bytesRead;
    	if (headerSize > 1024) {
			headerSize = 1024;
		}
		char buf[1025];
		memset(buf, 0, 1025);
		ssize_t numRead = read_all_from_socket(filedes, buf, headerSize);
		// LOG("%zd", numRead);
		if (numRead == -1){
			continue;
		}
		if( numRead==0 ) {
			break;
		}
		bytesRead += numRead;
		fwrite(buf, 1, numRead, writeFile);
	}
	fclose(writeFile);
	// LOG("%zu", size);
	// LOG("%zd",bytesRead);
	//printDirectory();
	if (checkErrors(size, bytesRead)) {
		remove(fullPath);
		return true;
	}

	if (!readFile) {
		vector_push_back(files, reqStruct->file);//If the file doesn't exist, add it to the vector
	} 
	else {
		fclose(readFile);
	}

	dictionary_set(sizeDict, reqStruct->file, &size);
	return false;//no errors
}

size_t read_all_header(int socket, char *buffer, size_t count){
	size_t bytesRead = 0;
	while (bytesRead < count) {
		ssize_t numRead = read(socket, buffer + bytesRead, 1);
		if (numRead == 0 || buffer[strlen(buffer) - 1] == '\n') {
			break;
		}
		if (numRead == -1){
			if (errno == EINTR){
				continue;
			}
			else {
				perror(NULL);
				exit(1);
			}
    	}
		bytesRead += numRead;
	}
	return bytesRead;
}

void processHeader(int filedes, clientReq *reqStruct) {
	size_t bytesRead = read_all_header(filedes, reqStruct->headers, 1024);
	if (bytesRead == 1024) {
		reqStruct->stage = BAD_REQUEST;
		epollChangeSettings(filedes);
		return;
	}
	//CHECKING FOR WHITESPACES AND NEW LINES TO CHECK FOR BAD REQUESTS
	if ( !strncmp("LIST\n", reqStruct->headers, 5) ) {//CHANGING THIS TO LIST\n and 5 to more thouroughly catch bad request
		reqStruct->request = LIST;
	}
	else if ( !strncmp("GET ", reqStruct->headers, 4) ) {
		reqStruct->request = GET;
		strcpy(reqStruct->file, reqStruct->headers + 4);//skip request and space
		reqStruct->file[strlen(reqStruct->file) - 1] = '\0';//remove newline
	}
	else if ( !strncmp("DELETE ", reqStruct->headers, 7) ) {
		reqStruct->request = DELETE;
		strcpy(reqStruct->file, reqStruct->headers + 7);//skip request and space
		reqStruct->file[strlen(reqStruct->file) - 1] = '\0';//remove newline
	}
	else if ( !strncmp("PUT ", reqStruct->headers, 4) ) {
		reqStruct->request = PUT;
		strcpy(reqStruct->file, reqStruct->headers + 4);//skip request and space
		reqStruct->file[strlen(reqStruct->file)-1] = '\0';//remove newline
		if (putHelper(filedes, reqStruct)) {
			epollChangeSettings(filedes);
			reqStruct->stage = BAD_FILE_SIZE;
			return;
		}
	}
	else {
		print_invalid_response();
		epollChangeSettings(filedes);
		reqStruct->stage = BAD_REQUEST;
		return;
	}
	epollChangeSettings(filedes);
	reqStruct->stage = BODY;
	return;
}

void doClientStuff(int filedes) {
	clientReq* reqStruct = dictionary_get(clientDict, &filedes);
  	int stage = reqStruct->stage;//stage the reqStruct is in used to determine next action
	if (stage == HEADER) {
		processHeader(filedes, reqStruct);
	}
	else if (stage == BODY) {
		processBody(filedes, reqStruct);
	}
	else{//errored
		write_all_to_socket(filedes, "ERROR\n", 6);
		switch(stage) {
			case BAD_REQUEST:
				write_all_to_socket(filedes, err_bad_request, strlen(err_bad_request));
				break;
			case BAD_FILE_SIZE:
				write_all_to_socket(filedes, err_bad_file_size, strlen(err_bad_file_size));
				break;
			case NO_SUCH_FILE:
				write_all_to_socket(filedes, err_no_such_file, strlen(err_no_such_file));
				break;
			default:
			break;
		}
		epoll_ctl(epollFiledes, EPOLL_CTL_DEL, filedes, NULL);
		dictionary_remove(clientDict, &filedes);
		shutdown(filedes, SHUT_RDWR);//Shutdown both receptions and transmissions
		free(reqStruct);
		close(filedes);
	}
}

void doServerStuff(char* port){
  	
  	int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	struct addrinfo* result;
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));//clear all unused flags
	hints.ai_family = AF_INET;       
	hints.ai_socktype = SOCK_STREAM; 
	hints.ai_flags = AI_PASSIVE;
	int check = getaddrinfo(NULL, port, &hints, &result);
	if (check) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(check));
		exit(1);
	}
	int optval = 1;//true / false value. need true for this
	if ( setsockopt(serverSocket, SOL_SOCKET,  SO_REUSEADDR, &optval, sizeof(optval)) ){
    	perror(NULL);
		exit(1);
	}
	if ( bind(serverSocket, result->ai_addr, result->ai_addrlen) ){
    	perror(NULL);
		exit(1);
	}
	if ( listen(serverSocket, 128) ){
    	perror(NULL);
		exit(1);
	}
  	freeaddrinfo(result);

	epollFiledes = epoll_create(128);//size param ignored but must be greater than 0
	if ( !(epollFiledes>0) ){
		perror(NULL);
		exit(1);
	}

	struct epoll_event temp;
	memset(&temp, 0, sizeof(struct epoll_event));
	temp.data.fd = serverSocket;
  	temp.events = EPOLLIN;
	epoll_ctl(epollFiledes, EPOLL_CTL_ADD, serverSocket, &temp);
	struct epoll_event epolls[128];

	while (true) {
		int numDescriptors = epoll_wait(epollFiledes, epolls, 128, 4096);
		if (numDescriptors == 0) {
			continue; // nothing available in timeout time
		}
		if (numDescriptors == -1) {
			perror(NULL);
			exit(1);
		}

		for (int i = 0; i < numDescriptors; i++) {
			if (epolls[i].data.fd == serverSocket) {
				int filedes = accept(serverSocket, NULL, NULL);
				if (filedes == -1) {
					perror(NULL);
					exit(1);
				}
				struct epoll_event clientEvent;
				memset(&clientEvent, 0, sizeof(struct epoll_event));
				clientEvent.data.fd = filedes;
        		clientEvent.events = EPOLLIN;
				epoll_ctl(epollFiledes, EPOLL_CTL_ADD, filedes, &clientEvent);
				clientReq* reqStruct = malloc(sizeof(clientReq));
				memset(reqStruct, 0, sizeof(clientReq));
        		dictionary_set(clientDict, &filedes, reqStruct);
			} 
			else {
				doClientStuff(epolls[i].data.fd);
			}
		}
	}
}

int main(int argc, char **argv) {
	if(argc != 2){
		print_server_usage();
		exit(1);  
	}

	signal(SIGPIPE, blank);

//https://stackoverflow.com/questions/45477254/how-sigaction-differs-from-signal
//Why do we care that much about the restart flag? Ask in office hours
	struct sigaction act;
	memset(&act, 0, sizeof(act));//Clear all flags that could be set from random memory vals
	act.sa_handler = exitServer;
	if (sigaction(SIGINT, &act, NULL) != 0) {//https://man7.org/linux/man-pages/man2/sigaction.2.html
		perror(NULL);
		exit(1);
	}

	files = string_vector_create();
	//dictionary.h stuff
	clientDict = int_to_shallow_dictionary_create();
	sizeDict = string_to_unsigned_long_dictionary_create();

	char template[] = "XXXXXX";
	directory = mkdtemp(template);//https://man7.org/linux/man-pages/man3/mkdtemp.3.html
	print_temp_directory(directory);
	
	doServerStuff(argv[1]);
}

void cleanDirectory() {
	VECTOR_FOR_EACH(files, file, {
		char fullPath[strlen(directory) + 1 + strlen(file) + 1];
		sprintf(fullPath, "%s/%s", directory, file);
		unlink(fullPath);
	});
	rmdir(directory);//like rm in command line
	vector_destroy(files);
}

void exitServer() {
	close(epollFiledes);
	vector* clients = dictionary_values(clientDict);
	VECTOR_FOR_EACH(clients, client, {
    	free(client);
	});
	vector_destroy(clients);
	cleanDirectory();
	dictionary_destroy(sizeDict);
	dictionary_destroy(clientDict);
	exit(1);
}

// https://man7.org/linux/man-pages/man2/epoll_ctl.2.html
void epollChangeSettings(int filedes) {
	struct epoll_event temp;
	memset(&temp, 0, sizeof(struct epoll_event));
	temp.data.fd = filedes;
  	temp.events = EPOLLOUT;
	epoll_ctl(epollFiledes, EPOLL_CTL_MOD, filedes, &temp);
}

void printDirectory() {
	DIR *d;
    struct dirent *dir;
    d = opendir(directory);
    if (d)
    {
        while ((dir = readdir(d)) != NULL)
        {
            printf("%s\n", dir->d_name);
        }
        closedir(d);
    }
}