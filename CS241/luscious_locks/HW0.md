# Hello, World! (system call style)
1.
```
#include <unistd.h>

int main() {
	write(1, "Hi! My name is Satwik\n", 22);
	return 0;
}
```
# Hello, Standard Error Stream!
2.
```
#include <unistd.h>

void write_triangle(int n) {
	int i;
	for(i=1; i<=n; i+=1){
		int j;
		for(j = 1; j<=i; j+=1){
			write(STDERR_FILENO, "*",1);
		}
		write(STDERR_FILENO, "\n",1);
	}
}
```

# Writing to files
3.
```
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	int filedes = open("hello_world.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
	write(filedes, "Hi! My name is Satwik\n", 22);
	close(filedes);
	return 0;
}
```
# Not everything is a system call
```
4.
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
	close(1);
	int filedes = open("hello_world.txt", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
	printf("Hi! My name is Satwik\n");
	close(filedes);
	return 0;
}
```
5. What are some differences between write() and printf()?
printf() always prints to the file descriptor with value 1, while the file descriptor is a parameter that is specified for write(). printf() does not require byte information unlike write(). printf() can print the value of data types unlike write(). For example, if int i= 4, printf can directly print i's value of 4.

# Not all bytes are 8 bits?

1. Depends on the architecture, though they are commonly 8 bits.
2. A char is 1 byte
3. int is 4 bytes. double is 8 bytes. float is 4 bytes. long is 4 bytes. long long is 8 bytes.

# Follow the int pointer
4. 0x7fbd9d50
5. data+3, 0x7fbd9d58

# sizeof character arrays, incrementing pointers

6. ptr points to a read-only memory block since it contains a string constant. Trying to overwrite this leads to the segfault.
7. 12
8. 15
9. X = "ab"
10. Y = 4

# Program arguments, argc, argv

1. The value of argc or iteratively counting items iteratively until null/nil is found.
2. First argument provided when calling the function

# Environment Variables

3. Somewhere else, in process memory above stack

# String searching (strings are just char arrays)

4. sizeof(ptr) return 8 because it is returning the size of the pointer type. sizeof(array) returns 6 because it copies over the entire string and performs sizeof() on it.

# Lifetime of automatic variables

5. stack

# Memory allocation using malloc, the heap, and time

1. heap. allocate the memory with malloc and return the pointer to the allocated memory
2. Heap memory is allocated and freed up manually during a process, while stack memory is automatically allocated and freed up
3. Yes, the memory layout has other things like the environment variables we discussed.
4. free
Heap allocation gotchas
5. Lack of free memory leading to overflow. Invalid size given as parameter to be allocated. 
6. time returns a time_t variable that represents a number of seconds. ctime takes in a pointer to a time_t variable and converts it into a String to represent that time and then returns a char* to that string.
7. double freeing can wipe bookkeeping done within the heap's memory
8. Shouldn't access a memory location once you've freed it since we have no information about it anymore.
9. Set ptr to null once freed.
struct, typedefs, and a linked list
10. 
```
struct Person{
	int age;
	char* name;
	struct Person* friends[];
};

typedef struct Person person;
```
11.
```
#include <string.h>
#include <stdlib.h>

struct Person{
	int age;
	char* name;
	struct Person*** friends;
};

typedef struct Person person;

int main() {
	person* p1 = (person*) malloc(sizeof(person));
	person* p2 = (person*) malloc(sizeof(person));
	char* name1 = (char*) malloc(sizeof("Agent Smith"));
	strcpy(name1, "Agent Smith");
	char* name2 = (char*) malloc(sizeof("Sonny Moore"));
	strcpy(name2,"Sonny Moore");
	p1->name = name1;
	p2->name = name2;
	p1->age = 128;
	p2->age = 256;
	
	person **arr1 = (person**) malloc(sizeof(person*));
	arr1[0] = p1;
	person*** friend1 = & arr1;
	person **arr2 = (person**) malloc(sizeof(person*));
	arr1[0] = p2;
	person*** friend2 = & arr2;
	p1-> friends = friend2;
	p2-> friends = friend1;
	
	return 0;
}
```
# Duplicating strings, memory allocation and deallocation of structures

12 & 13.
```
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct Person{
	int age;
	char* name;
	struct Person*** friends;
};

typedef struct Person person;

person* create(char* nameInput, int ageInput){
	person* result = (person*) malloc(sizeof(person));
	strcpy(result->name, nameInput);
	result->age = ageInput;
	person **arr = (person**) malloc(10*sizeof(person*));
	result->friends = & arr;
	return result;
}

void destroy(person* x){
	free(x->friends);
	free(x);
}
```
# Reading characters, trouble with gets

1. getchar(), putchar()
2. It can overwrite other variables if reading in more the data than the capacity of the variable being stored in
Introducing sscanf and friends
3.
```
#include <stdio.h>

int main() {
	char hello[6];
	int five;
	char world[6];
	sscanf("Hello 5 World", "%s%d%s", hello, & five, world);
	printf("%s %d %s", hello, five, world);
	return 0;
}
```
# getline is useful

4. the char* to allocate memory to and the capacity of the allocated memory the char* currently points to

5.
```
#include <stdio.h>

int main() {
	char* buffer = NULL;
	size_t capacity = 0;
	FILE* fp = fopen("message.txt", "r");
	int result;
	result = getline(&buffer, &capacity,fp);
	while(result >= 0){
		
		printf("%s", buffer);
		result = getline(&buffer, &capacity,fp);
		
	}
	capacity = 0;
	free(buffer);
	buffer = NULL;
	fclose(fp);
	return 0;
}
```
# C Development

1. -g

2. need to run a make clean because make will use the old artifacts with the same names instead of remaking them automatically unless you override your make to run a make clean first

3. tabs

4. commit saves changes to the  local repository. SHA is an algorithm used to generate git hashes

5. Displays past commits in the current repository/branch you are in

6. git status shows a summary of changes you have made to files in your local repository and differences between a tracked repository. Including a file in the .gitignore would make it so changes to these files would not be shown/tracked

7. git push attempts to advance the remote tracked or target repository to get up to date with the additional commits in your local repository.

8. It means there are merge conflicts caused by commits being pushed from other branches/repositories while you are making changes in your own. This can be dealt with by doing a fetch and merge or rebasing.






