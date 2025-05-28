# include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/*
 * File: calls.c
 * Author: Chance Krueger
 * Purpose: Defines a set of functions for building and manipulating a graph
 *          of phone numbers and call records. The graph is constructed
 *          from a file containing phone call pairs, and the program supports
 *          various graph operations such as adding edges, printing the graph,
 *          and checking phone number formats.
 */



int parseFile(char[]);



/*
* phoneNode -- represents a node in the phone-call graph.
* pNumber: stores the phone number (format ddd-ddd-dddd, 12 chars + '\0').
* calls: pointer to a linked list of edges (outgoing calls from this number).
* next: pointer to the next phoneNode in the overall linked list.
* level: used to record BFS depth when searching the graph.
* queued: flag indicating whether this node is currently in the BFS queue.
*/
struct phoneNode {
	char pNumber[13];
	struct edges *calls;
	struct phoneNode *next;
	int level;
	int queued;
};


/*
* edges -- represents an edge in the phone-call graph.
* to: pointer to the destination phoneNode for this call.
* next: pointer to the next edge in the linked list of calls.
* totalCalls: count of how many calls have occurred along this edge.
*/
struct edges {
	struct phoneNode *to;
	struct edges *next;
	int totalCalls;
};


/*
* queue -- node for a simple linked-list queue used in BFS.
* pNode: pointer to the phoneNode stored in this queue entry.
* next: pointer to the next entry in the queue.
*/
struct queue {
	struct phoneNode *pNode;
	struct queue *next;
};



/*
* dequeue(head) -- removes and returns the front phoneNode from the queue.
* head: pointer to the queue head pointer.
* Returns: the dequeued phoneNode*, or NULL if the queue was empty.
* Assumptions: *head is a valid pointer; queue entries have been allocated.
* Side effects: frees the removed queue node.
*/
struct phoneNode* dequeue(struct queue **head) {
    if (*head == NULL) {
        return NULL;  // Return NULL if the queue is empty.
    }
    struct phoneNode *node = (*head)->pNode;
    struct queue *temp = *head;
    *head = (*head)->next;  // Move the head pointer to the next element
    free(temp);
    return node;
}



/*
* enqueue(head, pNode) -- creates a new queue entry holding pNode and appends it.
* head: pointer to the queue head pointer.
* pNode: pointer to the phoneNode to enqueue.
* Returns: void.
* Assumptions: head points to a valid queue pointer; memory allocation succeeds.
* Side effects: allocates a new queue node and links it at the tail.
*/
void enqueue(struct queue **head, struct phoneNode *pNode) {
    struct queue *node = malloc(sizeof(struct queue));
    node->pNode = pNode;
    node->next = NULL;

    if (*head == NULL) {
        *head = node;  // If the queue is empty, the new node is the head.
        return;
    }

    struct queue *cur = *head;
    while (cur->next != NULL) {
        cur = cur->next;
    }
    cur->next = node;  // Add the new node to the end of the queue
}



struct phoneNode *headLL = NULL;




/*
* addEdges(p1, p2) -- ensures there is an undirected edge between phone nodes p1 and p2.
* If an edge already exists in either direction, increments its totalCalls count;
* otherwise allocates a new edge with totalCalls initialized to 1. Updates both p1->calls
* and p2->calls to maintain bidirectional connectivity.
* Assumes p1 and p2 are non-NULL and distinct.
*/
void addEdges(struct phoneNode *p1, struct phoneNode *p2) {

	
	struct edges *curEdge = p1->calls;

	int alreadyFound = 0;

	// No Edges in Node.
	if (curEdge == NULL) {
		struct edges *p2E = malloc(sizeof(struct edges));
                
		if (p2E == NULL) {
			fprintf(stderr, "Not Enough Memory.\n");
                        exit(1);
		}

		p2E->to = p2;
                p2E->next = NULL;
		p1->calls = p2E;
		p2E->totalCalls = 1;
                curEdge = p2E;
		alreadyFound = 1;
	}

	while (curEdge != NULL) {
		if (curEdge->to == p2 && !alreadyFound) {
			curEdge->totalCalls++;
			alreadyFound = 1;
			break;
		}
		if (curEdge->next == NULL) {
			break;
		}
		curEdge = curEdge->next;
	}

	if (!alreadyFound) {
		struct edges *p2E = malloc(sizeof(struct edges));
		if (p2E == NULL) {
                        fprintf(stderr, "Not Enough Memory.\n");
                        exit(1);
                }
		p2E->to = p2;
		p2E->next = NULL;
		p2E->totalCalls = 1;
		curEdge->next = p2E;
	}

	curEdge = p2->calls;
	alreadyFound = 0;
	
	// No Edges in Node.
        if (curEdge == NULL) {
                struct edges *p1E = malloc(sizeof(struct edges));
                if (p1E == NULL) {
                        fprintf(stderr, "Not Enough Memory.\n");
                        exit(1);
                }
		p1E->to = p1;
                p1E->next = NULL;
		p2->calls = p1E;
		p1E->totalCalls = 1;
                curEdge = p1E;
		alreadyFound = 1;
        }

        while (curEdge != NULL) {
                if (curEdge->to == p1 && !alreadyFound) {
			curEdge->totalCalls++;
                        alreadyFound = 1;
                        break;
                }
		if (curEdge->next == NULL) {
			break;
		}
                curEdge = curEdge->next;
        }

        if (!alreadyFound) {
                struct edges *p1E = malloc(sizeof(struct edges));
                if (p1E == NULL) {
                        fprintf(stderr, "Not Enough Memory.\n");
                        exit(1);
                }
		p1E->to = p1;
                p1E->next = NULL;
		p1E->totalCalls = 1;
                curEdge->next = p1E;
        }
}



/*
* addNodesToLL(phoneNumber, phoneNumber2) -- ensures that both phone numbers
* are present as nodes in the global linked list headLL and then connects them
* by calling addEdges. If a phone number is not already in the list, allocates
* and initializes a new phoneNode for it.
*
* phoneNumber    : null-terminated string in format "ddd-ddd-dddd" for the first number
* phoneNumber2   : null-terminated string in format "ddd-ddd-dddd" for the second number
* Returns        : void
* Assumptions    : phoneNumber and phoneNumber2 are valid, non-null strings;
*                  headLL is a global pointer to the start of the phoneNode list;
*                  memory allocation succeeds (or else the program exits).
*/
void addNodesToLL(char *phoneNumber, char *phoneNumber2) {

	struct phoneNode *p1 = NULL;
	struct phoneNode *p2 = NULL;
	
	if (headLL == NULL) {
		p1 = malloc(sizeof(struct phoneNode));
		if (p1 == NULL) {
                        fprintf(stderr, "Not Enough Memory.\n");
                        exit(1);
                }
		
		strcpy(p1->pNumber, phoneNumber);
		p1->calls = NULL;
		p1->next = NULL;
		p1->level = 0;
		p1->queued = 0;
		headLL = p1;
	}
 
	int f1 = 1, f2 = 1;

	struct phoneNode *cur = headLL;
	struct phoneNode *behind = NULL;
	while (cur != NULL) {
		// First Phone Number found.
		if (strcmp(cur->pNumber, phoneNumber) == 0) {
			f1 = 0;
			p1 = cur;
		// Second Phone Number found.
		} else if (strcmp(cur->pNumber, phoneNumber2) == 0) {
			f2 = 0;
			p2 = cur;
		}
		behind = cur;
		cur = cur->next;
	}

	// Phone Number 1 wasn't found, Add it to LL
	if (f1) {
		p1 = malloc(sizeof(struct phoneNode));
		if (p1 == NULL) {
                	fprintf(stderr, "Not Enough Memory.\n");
                	exit(1);
        	}
		strcpy(p1->pNumber, phoneNumber);
		p1->calls = NULL;
                p1->next = NULL;
		p1->level = 0;
		p1->queued = 0;
		behind->next = p1;
		behind = behind->next;
	}

        // Phone Number 2 wasn't found, Add it to LL
	if (f2) {
		p2 = malloc(sizeof(struct phoneNode));
		if (p2 == NULL) {
                	fprintf(stderr, "Not Enough Memory.\n");
                	exit(1);
        	}
		strcpy(p2->pNumber, phoneNumber2);
                p2->calls = NULL;
                p2->next = NULL;
		p2->level = 0;
		p2->queued = 0;
		behind->next = p2;
	}	

	addEdges(p1, p2);
}



/*
* checkPhoneFormat(phoneNum) -- checks if the string phoneNum matches
* the format ddd-ddd-dddd exactly. Returns 0 if valid, 1 otherwise.
*
* phoneNum: pointer to a null-terminated string representing a phone number.
* Assumes: phoneNum is non-NULL.
*/
int checkPhoneFormat(char *phoneNum) {
	int index = 0;

    	while (*phoneNum != 0) {
        	if (index > 12) {
            		return 1;  // Too long
        	}

        	if (index == 3 || index == 7) {
            		if (*phoneNum != '-') {
                		return 1;  // Dash expected
            		}
        	} else {
            		if (*phoneNum < '0' || *phoneNum > '9') {
                		return 1;  // Digit expected
            		}
        	}

        	phoneNum++;
        	index++;
    	}

   	if (index != 12) {
        	return 1;  // Too short
    	}

    	return 0;  // Valid format
}



/*
* parseFile(argv) -- opens the file named by argv and reads each line containing
* two phone numbers in the format ddd-ddd-dddd. For each valid line, it adds
* the pair to the global graph via addNodesToLL. Skips lines with format errors
* (printing an error to stderr) but continues processing. Returns 1 if the file
* could not be opened or any formatting errors were encountered; returns 0 otherwise.
*/
int parseFile(char argv[]) {

	int errSeen = 0;
	
	FILE *input = fopen(argv, "r");

	if (input == NULL) {
		fprintf(stderr, "Could Not Open File.\n");
		return 1;
	}
	int retval;
	char area[5], firstThree[5], lastFour[6], err[2];
	char *line = NULL;
	size_t len = 0;
	while (getline(&line, &len, input) > 0) {

		char *phoneNum = calloc(13, sizeof(char));
		if (phoneNum == NULL) {
			fprintf(stderr, "Ran Out of Memory.\n");
			exit(1);
		}

		int i = 0, j = 0;
		int skipLine = 0;
    		while (line[i] != 0) {
        		if ((line[i] >= '0' && line[i] <= '9') || line[i] == '-') {
            			if (j < 12) {
                			phoneNum[j++] = line[i];
            			} else if (! isspace(line[i])) {
					fprintf(stderr, "Too Many Args Given");
					skipLine = 1;
					errSeen = 1;
					break;
				}
        		} else {
            			break;
        		}
        		i++;
    		}
		phoneNum[j] = 0;

		char *phoneNum2 = calloc(13, sizeof(char));
                if (phoneNum == NULL) {
                        fprintf(stderr, "Ran Out of Memory.\n");
                        exit(1);
                }

		// Skip whitespace
		while (isspace(line[i])) {
			i++;
		}

                j = 0;
                while (line[i] != 0) {
                        if ((line[i] >= '0' && line[i] <= '9') || line[i] == '-') {
                                if (j < 12) {
                                        phoneNum2[j++] = line[i];
                                } else if (! isspace(line[i])) {
                                        fprintf(stderr, "Too Many Args Given");
                                        errSeen = 1;
					skipLine = 1;
                                        break;
                                }
                        } else {
                                break;
                        }
                        i++;
                }
                phoneNum2[j] = 0;
	
		while (line[i] != 0) {
			// Extra Input was added.
			if (! isspace(line[i])) {
				errSeen = 1;
				skipLine = 1;
                        	fprintf(stderr, "Incorrect Format.\n");
                        	break;
			}
			i++;
		}

		if (skipLine) {
			free(line);
                	line = NULL;
                	free(phoneNum);
                	phoneNum = NULL;
                	free(phoneNum2);
                	phoneNum2 = NULL;
			continue;
		}

		// Too Many Characters, err had something in it.
		if ((retval = sscanf(phoneNum, "%3s-%3s-%4s%1s", area, firstThree, lastFour, err)) == 4) {
			errSeen = 1;
			fprintf(stderr, "Incorrect Format.");
			break;
		}
		// Too Many Characters, err had something in it.
                if ((retval = sscanf(phoneNum2, "%3s-%3s-%4s%1s", area, firstThree, lastFour, err)) == 4) {
                        errSeen = 1;
                        fprintf(stderr, "Incorrect Format.");
                        break;
                }
		int checkFormat = 0;
		checkFormat += checkPhoneFormat(phoneNum);
                checkFormat += checkPhoneFormat(phoneNum2);

		// Incorrect Format
		if (checkFormat) {
			free(line);
                        line = NULL;
                        free(phoneNum);
                        phoneNum = NULL;
                        free(phoneNum2);
                        phoneNum2 = NULL;
			fprintf(stderr, "Incorrect Formating.\n");
                        continue;
		}

		// printf("%s\n", line);
		// printf("PHONE NUMBER: %s.\n", phoneNum);
		// printf("PHONE NUMBER2: %s.\n", phoneNum2);

		addNodesToLL(phoneNum, phoneNum2);

		free(line);
		line = NULL;
		free(phoneNum);
		phoneNum = NULL;
		free(phoneNum2);
                phoneNum2 = NULL;
	}
	return errSeen;
}




/*
* BFS(start, target) -- returns the number of intermediate nodes on the
* shortest path between start and target in the phone-call graph.
* It assumes the global list headLL contains all nodes and uses
* a breadth-first search to find the shortest connection.
* Returns -1 if no path exists.
*/
int BFS(struct phoneNode *start, struct phoneNode *target) {
    
	// Reset the "queued" flag for all nodes before running BFS
    	struct phoneNode *cur = headLL;
    	while (cur != NULL) {
        	cur->queued = 0;
        	cur = cur->next;
    	}
	start->queued = 1;
    	start->level = 0;

    	// Initialize the queue as a pointer to pointer.
    	struct queue *q = NULL;
    
    	// Pass the address of q to enqueue function
    	enqueue(&q, start);  // Enqueue start node

    	struct edges *children;
    	while (q != NULL) {
        	struct phoneNode *A = dequeue(&q);  // Pass the address of q to dequeue
        	if (A == target) {
            		return A->level - 1;  // Subtract 1 to exclude the start node
        	}
        	for (children = A->calls; children != NULL; children = children->next) {
            		if (!children->to->queued) {
                		children->to->queued = 1;
                		children->to->level = A->level + 1;
                		enqueue(&q, children->to);  // Pass the address of q to enqueue
            		}
        	}
    	}
    	return -1;  // Return -1 if no connection is found
}



/*
 * checkIfInGraph(p1, p2) -- Searches for two phone numbers, p1 and p2, in the linked list of phone nodes.
 * It checks if both phone numbers exist in the graph. If either phone number is not found, an error message is printed
 * and the function returns 1. If both numbers are found, it checks if they are directly connected by checking the 'calls'
 * list of the first phone number. If they are not directly connected, it performs a breadth-first search (BFS) to determine
 * if they are indirectly connected. The function prints the number of calls if a direct connection is found or the number of
 * numbers separating the two if they are indirectly connected. If no connection is found, it prints "Not connected".
 * The function returns 0 if both phone numbers are found and processed.
 */
int checkIfInGraph(char *p1, char* p2) {
    struct phoneNode *pn1 = NULL;
    struct phoneNode *pn2 = NULL;
    struct phoneNode *cur = headLL;  

    int p1Found = 1;  
    int p2Found = 1;

    while (cur != NULL) {
        if (strcmp(cur->pNumber, p1) == 0) {
            p1Found = 0;  // Found p1
            pn1 = cur;
        }

        if (strcmp(cur->pNumber, p2) == 0) {
            p2Found = 0;  // Found p2
            pn2 = cur;
        }
        cur = cur->next;
    }

    if (p1Found || p2Found) {  // If either is still 1, then the number is not found
        fprintf(stderr, "Phone Number Not Found.\n");
        return 1;
    }

    struct edges *curEdge = pn1->calls;
    int linkedCalls = 0;

    while (curEdge != NULL) {
        if (pn2 == curEdge->to) {
            linkedCalls += curEdge->totalCalls;
            break;
        }
        curEdge = curEdge->next;
    }

    if (linkedCalls) {
        printf("Talked %d times\n", linkedCalls);
    } else {
        int search = BFS(pn1, pn2);  // Perform BFS

        if (search == -1) {
            printf("Not connected\n");
        } else {
            printf("Connected through %d numbers\n", search);
        }
    }

    return 0;
}



/*
 * freePhoneList() -- Frees all dynamically allocated memory associated with the phone list.
 * It iterates through the linked list of phone nodes starting from headLL. For each phone node, 
 * it traverses and frees the associated edges (connections between phone nodes) in the 'calls' field. 
 * After freeing the edges, it frees the phone node itself. Once all nodes and their associated memory 
 * are freed, it sets headLL to NULL to indicate the list is empty.
 */
void freePhoneList() {
    struct phoneNode *cur = headLL;  // headLL is a global variable
    while (cur != NULL) {
        struct phoneNode *temp = cur;
        cur = cur->next;

        // If you have other dynamically allocated memory within the phoneNode structure
        // like the 'calls' field, make sure to free it too.
        struct edges *call = temp->calls;
        while (call != NULL) {
            struct edges *callTemp = call;
            call = call->next;
            free(callTemp);  // Free the edge node
        }

        free(temp);  // Free the phoneNode
    }

    // Optionally, set headLL to NULL after freeing all the nodes, to indicate the list is empty.
    headLL = NULL;
}


int main(int argc, char* argv[]) {
	
	int errSeen = 0;

	if (argc <= 1) {
		fprintf(stderr, "Not enough File arguments Given.\n");
		return 1;
	}

	int i = 1;

	while (i < argc) {

		errSeen += parseFile(argv[i]);
		i++;
	}

	// Need to parse from stdin to check BFS and then to print out the message
	
	int retval;
        char area[5], firstThree[5], lastFour[6], err[2];
        char *line = NULL;
        size_t len = 0;
        while (getline(&line, &len, stdin) > 0) {

                char *phoneNum = calloc(13, sizeof(char));
                if (phoneNum == NULL) {
                        fprintf(stderr, "Ran Out of Memory.\n");
                        exit(1);
                }

                int i = 0, j = 0;
                int skipLine = 0;
                while (line[i] != 0) {
                        if ((line[i] >= '0' && line[i] <= '9') || line[i] == '-') {
                                if (j < 12) {
                                        phoneNum[j++] = line[i];
                                } else if (! isspace(line[i])) {
                                        fprintf(stderr, "Too Many Args Given");
                                        skipLine = 1;
                                        errSeen = 1;
                                        break;
                                }
                        } else {
                                break;
                        }
                        i++;
                }
                phoneNum[j] = 0;

                char *phoneNum2 = calloc(13, sizeof(char));
                if (phoneNum == NULL) {
                        fprintf(stderr, "Ran Out of Memory.\n");
                        exit(1);
                }

                // Skip whitespace
                while (isspace(line[i])) {
                        i++;
                }

                j = 0;
                while (line[i] != 0) {
                        if ((line[i] >= '0' && line[i] <= '9') || line[i] == '-') {
                                if (j < 12) {
                                        phoneNum2[j++] = line[i];
                                } else if (! isspace(line[i])) {
                                        fprintf(stderr, "Too Many Args Given");
                                        errSeen = 1;
                                        skipLine = 1;
                                        break;
                                }
                        } else {
                                break;
                        }
                        i++;
                }
                phoneNum2[j] = 0;

                while (line[i] != 0) {
                        // Extra Input was added.
                        if (! isspace(line[i])) {
                                errSeen = 1;
                                skipLine = 1;
                                fprintf(stderr, "Incorrect Format.\n");
                                break;
                        }
                        i++;
                }

                if (skipLine) {
                        free(line);
                        line = NULL;
                        free(phoneNum);
                        phoneNum = NULL;
                        free(phoneNum2);
                        phoneNum2 = NULL;
                        continue;
                }

                // Too Many Characters, err had something in it.
                if ((retval = sscanf(phoneNum, "%3s-%3s-%4s%1s", area, firstThree, lastFour, err)) == 4) {
                        errSeen = 1;
                        fprintf(stderr, "Incorrect Format.");
                        break;
                }
                // Too Many Characters, err had something in it.
                if ((retval = sscanf(phoneNum2, "%3s-%3s-%4s%1s", area, firstThree, lastFour, err)) == 4) {
                        errSeen = 1;
                        fprintf(stderr, "Incorrect Format.");
                        break;
                }
                int checkFormat = 0;
                checkFormat += checkPhoneFormat(phoneNum);
                checkFormat += checkPhoneFormat(phoneNum2);

                // Incorrect Format
                if (checkFormat) {
                        free(line);
                        line = NULL;
                        free(phoneNum);
                        phoneNum = NULL;
                        free(phoneNum2);
                        phoneNum2 = NULL;
                        fprintf(stderr, "Incorrect Formating.\n");
                        continue;
                }

                // CALL BFS FUNCTIONs
		// First, check to see if phone nums are in graph
		// then call BFS in that function
		checkIfInGraph(phoneNum, phoneNum2);

                free(line);
                line = NULL;
                free(phoneNum);
                phoneNum = NULL;
                free(phoneNum2);
                phoneNum2 = NULL;
        }


	freePhoneList();

	// printGraph();

	return errSeen >= 1;


}
