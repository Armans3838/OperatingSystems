# CPSC380_Operating_Systems_Assignment5
Arman Siddiqui

# Sources/Help
For this assignment, I used the following resources:
- Lucus and Sophie helped me work through this assingment, hence the similarity in our submissions
- GitHub CoPilot helped with all of this assignment in terms of the syntax and logic. The code was of course conceptualized by myself and Lucus, but the syntax was aided by CoPilot
- GeeksforGeeks offered lots of insight into how to acutally use the shared memory and semaphores.

# Goal:
The Goal of this assignment was use shared memory and semaphores to create a program that would allow two processes to communicate with each other. The program would create a nitems buffer, and two processes would be able to write to and read from the buffer. The program would also use semaphores to ensure that the processes would not write to the buffer if it was full, and would not read from the buffer if it was empty.

# How to Run:
Compile the program using the following command:
```
gcc -o prodcon prodcon.cpp
```

Run the program using the following command:
```
./prodcon <nitems>
```