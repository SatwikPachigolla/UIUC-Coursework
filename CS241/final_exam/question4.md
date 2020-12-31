1. Virtual file systems in /dev and /proc

Our smart shoes will utilize the /dev and /proc directories using the same treat everything as files approach as linux does. While representing everything as a file the /dev directory will contain all peripheral devices connected to the shoes. These smart shoes are replacing those pesky smartphones people had to worry about cracking and leaving on a table. Due to this, all peripherals for charging and flashing firmware are managed in the /dev directory. The /proc directory contains all the processes the shoes are running. This is where all the NLP, NLG, and other ML algorithms will be running to cater many of the needs of the user. Oh and also, this is where the process connecting to the wireless chip in the user's brain will be running to determine which direction the shoes will move. You didn't think we would still have to physically move our legs to walk did you?

2. zfs features that are not available in the ext filesystems

We are still talking about ext filesystems after all these years?  We will be using the increased capacity of zfs being able to hold 1 billion terabytes of data. ext's highest capacity of 18 terabytes will be exhausted in hours if not minutes with the amount of data being circulated these days. You don't want your shoes to stop working while you're out running errands (or they are running your errands for you). Zfs will also allow for much better organization of data and improve reliability through snapshots and checksums.

3. Network protocols

Well somehow there hasn't been anew revolutionary network protocol introduced in the last two decades and engineers still think of TCP, UDP, and HTTP first. Network protocols determine the structure in which data is formatted and sent, with HTTP being how browsers started doing it in the early years of the century. While UDP might have some performance benefits they are insignifcant to us compared to the reliability benefits involved with TCP over UDP including recovery and re-transmission. We take reliability over performance for such critical personal devices.

4. TCP sockets

The shoes will be able to open multiple TCP sockets with their associated file descriptors. These sockets will establish two-way communication with multiple services to implement the entire suite of IoT functionality the shoes will be able to perform.

5. Threads or processes

Since the shoes will be manageing many parts of the users life, many processes will be running on them. These processes will also be heavily parallelized through the use of threads to bolster the performance. Many years ago, people used to be concerned about Amdahl's law since they did not have many CPU cores. This is no longer a concern with today's heptacore processors.

6. Pipes

Pipes are used to speed up IPC communication by circumventing interactions with disk memory when transferring data between files. We have so much more RAM these days that one might even think there is no reason for disc memory but while our RAM capactiy has increased so has our overall storage capacity and amount of data collection, so pipes serve the same purpose that they did decades ago.

7. Signal handling

Since the shoes will be interacting with our environment a lot they will need to be able to react quickly to changes in the environment. In order to make this as fast as possible we use signals at the system level. These can be caught so that any data that needs to be perserved can be handled and then any change in desired behavior will be executed rapidly. This will be useful in the case when the shoes are walking for the user but then they detect a moving presence nearby (one they cannot communicate with through IoT, like an animal) and need to quickly let the process that is handling the walking to stop moving in the direction it is.

8. Write brief code that uses read(2) write(2) writes the contents of a file to a pipe. Your code
should check for EINTR and continue to send data if interrupted. However if there are no
more readers you should stop trying to send bytes to the pipe. Your function should return 0
if all bytes were sent, 1 otherwise.

~~~c
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, char** argv) {
    if(argc!=2) {
        printf("Usage: ./executable-name path-name\n");
        exit(1);
    }
    char* pathname = argv[1];
    int pipefd[2];
    pipe(pipefd);
    int file;
    if ( !(file = open(pathname, S_IRUSR)) ) {
        return 1;
    }

    char buf[1];
    while ( read(file, buf, 1) == 1 ) {
        if (write(pipefd[1], buf, 1) < 1) {
            return 1;
        }
    }

    return 0;
}
~~~

9. (okay okay this is not a code question) When would you need and choose a symbolic link?
When would you need and choose a hard link?

Hard links are pointer to an inode while soft links are pointer to another file/shortcut. Deleting an original file can make it so the symbolic link is pointing to a non-existant file while the hard link will still be pointing to the inode. In this case, the hard link can be used for recovery. Additionally, a hard link cannot be used across file systems while a symbolic link can be, so if cross file system linking is desired, it is better to go with a symbolic link.


10. Using a version of stat, write code to determine a file's size and return -1 if the file does
not exist, return -2 if the file is a directory or -3 if it is a symbolic link.

~~~c
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int helper(char* pathname){
    struct stat buf;
    if (stat(pathname, &buf) == -1) { 
        return -1;
    }
    else if (S_ISDIR(buf.st_mode)) {
        return -2;
    }
    else if (S_ISLNK(buf.st_mode)) {
        return -3;
    }
    return buf.st_size;
}

int main(int argc, char** argv) {
    if(argc!=2) {
        printf("Usage: ./executable-name path-name\n");
        exit(1);
    }
    int result = helper(argv[1]);
    printf("Code returned: %d\n", result);
    return 0;
}
~~~
