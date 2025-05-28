# PhoneCall-Graph
## Description
Write a C program PhoneCallGraph.c with a Makefile to process files listing pairs of phone PhoneCallGraph. Track call counts between numbers, answer queries on direct PhoneCallGraph or shortest connection paths via BFS in an undirected graph. Handle errors gracefully and free all memory. Compile with make PhoneCallGraph.


## What I learned: 
    - How to read and process multiple input files in C
    - How to build and represent an undirected graph using adjacency lists with linked lists
    - How to track weighted edges (call counts) between nodes in a graph
    - Implementing and using Breadth-First Search (BFS) to find shortest paths
    - Handling input validation and error reporting gracefully
    - Managing dynamic memory allocation and freeing it properly to avoid leaks
    - Writing a Makefile to automate compilation with warnings enabled (-Wall)
    - Practical experience with real-world data mining concepts applied to phone call networks

## How To run it:
### Compile the program using a C compiler, for example:
    - gcc PhoneCallGraph.c -o PhoneCallGraph -Wall
    
### Run the executable from the command line:
    - ./PhoneCallGraph inFile1 [inFile2 ...]
        - Each input file contains pairs of phone numbers representing PhoneCallGraph.

### Once running
    - Type a pair of phone numbers separated by space and press Enter.
    - The program will print either:
        - Talked n times if the two numbers called each other directly,
        - Connected through n numbers if connected indirectly,
        - or Not connected if no connection exists.
    - To end queries, send an EOF (Ctrl+D on Unix/macOS, Ctrl+Z on Windows).


## Future Improvements
    - Add a -l option to print the full connection path (not just the number of connecting phone numbers).
    - Support more flexible phone number formats or validation.
    - Improve error handling with detailed messages and logging.
    - Allow queries from a file instead of interactive stdin.
