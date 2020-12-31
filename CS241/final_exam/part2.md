In general, programming has the challenge of designing while meeting requirements of correctness, performance, and complexity. Systems programming frequently involves creating utilities with widespread and frequent use cases making it so that those requirements are even more demanding and critical.

Striving for near optimal implementations of utilities requires using parallel programming techniques where possible and using proper practices to utilize (get it?) the full potential of the utility. While using powerful parallel programming techniques to benefit performance, many dangers are introduced with potentially catastrophic effects. Improper practices can create race conditions whose detrimental effects may not be uncovered in simple testing leading to devestating results in production code. Incorrect design can make a system deadlock and cascade through the rest of the system.

Race conditions can be prevented by making mindful decisions and following proper general coding practices, but most importantly using synchronization primitives where necessary. The most common of them, being a mutex lock. All shared resources should have the necessary synchronization primitives used along with them to circumvent the destructive effects of race conditions. Using these synchronization can introduce other unwanted behavior like the previously mentioned deadlocking. 

Systems programmers may employ several different methods to avoid deadlocking (or similar pitfalls such as livelocking). These involve deadlock detection, deadlock avoidance, and deadlock prevention. While they are somewhat self-explanatory, deadlock detection is when a system detects a deadlock (surprise!), this can be done through a resource allocation graph or another such algorithm. Once the deadlock is detected a recovery method can be employed to reconcile it. Deadlock avoidance is when the system does not make decision which would lead to a deadlock and deadlock prevention is when the systems programmer ensures a deadlock cannot be reached by violating a Coffman condition. Usually the condition that is violated is circular-wait which can be violated using a rank ordering system when acquiring locks.

While mindful programming is more critical for systems programmers, they also have to perform extensive load/stress testing to catch these phenomenon, even when they are unlikely to occur. Actually, especially when they are unlikely to occur.

While using synchronization primatives it is also to important to note that more complexity is introduced since they can also drag down the performance of a system if overused. Yes, this is the life of a Systems Programmer; even the wins are losses. In order to avoid unecessarily hurting the performance of a system, only the minimum amount of computation required to avoid the mentioned pitfalls should be encompassed by the synchronization primative. Doing this will decrease the time a thread is occupying it, making it available more quickly, resulting in less idle wait time.

The makefile utility is one of the most frequently used systems programming utilities out there. Often it is the first of such utilities a student becomes aware of. Parallelizing it is crucial for large projects builds. The following will be a discussion of my initial approach to implementing a parallelized version of the makefile and how I improved upon my original pitfalls.

The two previously mentioned concepts this assignment focused on were detecting potential circular-waits and determining an order with which to process the makefile that avoid busy-waiting.

The original approach I used was using recursion to detect cycles in the dependency graph in a pre-processing phase.

~~~c
bool checkCycleHelper(char* target) {
    bool result = false;
    vector * neighbors = NULL;
    if (set_contains(visited, target)) {
        return true;
    } 
    else {
        set_add(visited, target);
        neighbors = graph_neighbors(g,target);
        VECTOR_FOR_EACH(neighbors, neighbor, {
            if (checkCycleHelper(neighbor)) {
                result = true;
                break;
            }
        });
    }
    vector_destroy(neighbors);
    set_remove(visited, target);
    return result;
}
~~~

The cycle detection performed as expected and is a commonly used approach to detecting cycles in a graph. This part of the approach was effective.

The processing phase also used recursion to add rules to a provided thread-safe queue in an order determined by the recursion. Worker threads pulled from the queue and executed the rules. 

Ordering:

~~~c
bool checkFailed(char* target) {
    ...
    vector *neighbors = graph_neighbors(g, target);

    for (size_t i = 0; i < vector_size(neighbors); i++) {
        char *neighbor = vector_get(neighbors, i);

        ...
        rule_t *neighborRule = (rule_t *) graph_get_vertex_value(g, neighbor);
        if ( !(neighborRule->state) && checkFailed(neighbor) ) {//recurse into neighbors
            return true;//Recursively return without doing anything else
        }
    }

    *** This is the sub-optimal behavior ***
    //wait for runners to finish dependents
    pthread_mutex_lock(&m);
    while (!dependencies(neighbors)) {
        pthread_cond_wait(&cv, &m);
    }
    pthread_mutex_unlock(&m);
    *** This is the sub-optimal behavior ***

    ...
}
~~~

As marked within the above code snippet, attempting to figure out an order in which to process rules recursively led to idle-waiting when the dependencies of the rule at the current point in the recursion were not finished but another rule's dependencies were. This occurred because there was no way to know which dependencies would finish execution first. This compromised the performance and I fixed this using a revised approach to the ordering which will be detailed further below.

Worker threads:

~~~c
void *runner(void *unused) {
    while (true) {
        pthread_mutex_lock(&mQ);
        rule_t *rule = queue_pull(q);
        if (!rule) {
            queue_push(q, NULL);
            pthread_mutex_unlock(&mQ);
            break;
        }
        ...
    }
    ...
}
~~~

Using the thread-safe queue made it so that the previously mentioned protection against race conditions are protected against and only encompassing the interaction with the queue in a mutex follows the previously mentioned practice to maximize the performance of the parallel computation. This part of the approach was effective. One other complication was that using a queue led to issues keeping track of the terminating NULL pointer at the end of the queue since pushing new rules to the queue would be pushed after the terminating NULL pointer. This was easily fixed by using a vector as a stack and encompassing pops and pushes in a mutex dedicated to the vector.

As mentioned above, I did not have much trouble avoiding race conditions and preventing deadlocking or was able to do so with very little debugging since I used the techniques to effectively write code to avoid such situation that I outlined above. The one major issue that I still needed to solve was the idle-waiting when ordering rules that I explained above. The new approach to the ordering I used was to figure out all the leaf rules in the dependency subgraph we needed to process while doing the cycle detection. Once those "base rules" were populated, the worker threads could pop from the stack and execute a rule. Upon finishing execution of the rule the thread checks all its antidependencies to see if they are part of our execution subgraph and have all their dependencies completed. Any such rules are pushed onto the stack so that if there is ever a rule to be executed it will be available for a free thread to pop it from the thread-safe stack. This ensures no idle-waiting occurs.

~~~c
void checkCycle_createStack() {
    ...
    VECTOR_FOR_EACH(goalRules, goal, {
        temp = shallow_vector_create();//temporarily holds base rules found to be added to the stack
        if (helper(goal)) {//helper defined below
            print_cycle_failure(goal);
        }
        else{
            VECTOR_FOR_EACH(temp, elem, {
                char* target = (char*)elem;
                vector_push_back(stack, target);
            });
        }
        vector_destroy(temp);
        temp = NULL;
    });
    ...
}

bool helper(char* target) {
    ...
    if ( vector_size(neighbors) ) {
        VECTOR_FOR_EACH(neighbors, neighbor, {
            if ( helper(neighbor) ) {
                set_add(cycles, target);
                result = true;
                break;
            }
        });
    }
    else {//detects if the rule is a leaf rule
        if ( !set_contains(totalSet, target) ) {
            vector_push_back(temp, target);
        }
    }
    ...
}
~~~

The approach to populating the initial base rules was very effective and I verified this through creating some functions to debug any unexpected behavior like the following to print out the base rules and all the eventual rules to be executed:

~~~c
void debug() {
    char* toPrint;
    printf("Base Rules: ");
    while( (toPrint = queue_pull(q)) ) {
        printf("%s ",toPrint);
        queue_push(q, toPrint);
    }
    queue_push(q, NULL);
    printf("\n");
    printf("All Rules: ");
    vector* allElements = set_elements(totalSet);
    VECTOR_FOR_EACH(allElements, elem, {
        toPrint = elem;
        printf("%s ",toPrint);
    });
    printf("\n");
    vector_destroy(allElements);
}
~~~

The only other significant change was to the runner threads checking to see when new rules are ready to be added to the execution stack and how the threads waited when there were no rules ready to be executed.

~~~c
void *runner(void *unused) {
    while (true) {
        //concurrency control
        pthread_mutex_lock(&mS);
        char *target;
        while( !(target = vector_pop(stack)) ) {
            vector_push_back(stack, NULL);
            waiting++;
            if (waiting == numThreads) {
                pthread_cond_broadcast(&cv);
                pthread_mutex_unlock(&mS);
                return NULL;
            }
            pthread_cond_wait(&cv, &mS);
            waiting--;
        }
        pthread_mutex_unlock(&mS);

        ...

        if ( !failed ) {
            ...
            //determine the other rules to be added
            VECTOR_FOR_EACH(antineighbors, elem, {
                char* antineighbor = (char*)elem;
                if (set_contains(totalSet, antineighbor)) {
                    pthread_mutex_lock(&mG);
                    vector* checkNeighbors = graph_neighbors(g, antineighbor);
                    bool ready = dependencies(checkNeighbors);
                    pthread_mutex_unlock(&mG);
                    if (ready) {
                        pthread_mutex_lock(&mS);
                        vector_push_back(stack, antineighbor);
                        pthread_cond_signal(&cv);
                        pthread_mutex_unlock(&mS);
                    }
                ...
            ...
        ...
    ...
}
~~~

These revisions were effective and the process no longer suffered from race conditions, deadlocks, or idle-waiting.

To summarize the final implementation:  
Implemented cycle detection to avoid deadlocks by violating circular-wait.  
Parallelized rule processing using threads to increase performance.  
Implemented a thread safe stack to avoid race conditions when processing rules.  
Used a reverse traversal from leaf rules up to goal rules during rule processing to avoid idle-waiting.  

For rules:  
A: C D  
B: E  
C:  
D: F  
E:  
F:  

The following dependency graph is created:
~~~
    A    B
   /  \   \
  C    D   E
      /
     F
~~~

If the goal rules were A and D, the base rules populated in the pre-processing would be C and F. Once F is processed, D can be processed before, after, or at the same time as C and once C and D are processed, A can be processed. Succesfully processing all goal rules.

Reviewing the improper practices in my original approach and revising my approach to follow proper parallel programming practices allowed me to implement the parallel make utility in a closer to optimal manner.

