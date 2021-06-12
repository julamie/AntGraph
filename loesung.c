#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

struct NodeList {
    uint32_t size;
    uint32_t len;
    struct Node *nodes;
};

struct Node {
    char *id;
    uint32_t value;
    struct Node *neighbours;
    uint32_t numNeighbours;
};

struct NodeList nodelist;

int init() {
    nodelist.size = 10;
    nodelist.len = 0;
    nodelist.nodes = malloc(sizeof(struct Node) * nodelist.size);

    return 0;
}

char* parseLeftSide() {
    char *id;
    char currChar;
    uint32_t size = 10;
    uint32_t len = 0;

    // make space for id
    id = realloc(NULL, sizeof(*id) * size);
    if (id == NULL) {
        // TODO: ERROR
    }

    // add every allowed char to id
    currChar = getchar();
    while ((currChar >= 'a' && currChar <= 'z') ||
           (currChar >= '0' && currChar <= '9')) {

        id[len++] = currChar;

        // increase size of id if necessary
        if (len == size) {
            size = size * 2;

            id = realloc(id, sizeof(*id) * size);
            if (id == NULL) {
                // TODO: ERROR
            }
        }
        currChar = getchar();
    }

    // end the string and clip it behind the '\0'
    id[len++] = '\0';
    id = realloc(id, sizeof(*id) * len);

    // at the end of id only a colon is allowed
    if (currChar == 'A') {
        if (getchar() != ':') {
            // TODO: ERROR
        }

        free(id);
        id = "A";
    } else if (currChar != ':') {
        // TODO: ERROR
    }

    return id;
}

void parseRightSide() {
    char* id;
    char currChar;
    uint32_t size = 10;
    uint32_t len = 0;

    // make space for id
    id = realloc(NULL, sizeof(*id) * size);
    if (id == NULL) {
        // TODO: ERROR
    }

    // add every allowed char to id
    currChar = getchar();
    while ((currChar >= 'a' && currChar <= 'z') ||
           (currChar >= '0' && currChar <= '9')) {

        id[len++] = currChar;

        // increase size of id if necessary
        if (len == size) {
            size = size * 2;

            id = realloc(id, sizeof(*id) * size);
            if (id == NULL) {
                // TODO: ERROR
            }
        }
        currChar = getchar();
    }

    // end the string and clip it behind the '\0'
    id[len++] = '\0';
    id = realloc(id, sizeof(*id) * len);
    printf("Rest: %s\n", id);

    if (currChar == ',') {
        parseRightSide();
    } else if (currChar == '-') {
        // TODO: Make correct
        int value;
        if(scanf("%d", &value) != 1) {
            // TODO: ERROR
        }
        printf("Value: %d\n", value);

        if (getchar() != '\n') {
            // TODO: ERROR
        }
    } else if (currChar != '\n') {
        // TODO: ERROR
    }
}

int scanContents() {
    bool finished = false;
    while (!finished) {
        char* id = parseLeftSide();
        printf("ID: %s\n", id);
        parseRightSide();

        if (strcmp(id, "A") == 0) {
            finished = true;
        }
    }

    /*struct Node node;
    node.id = malloc(sizeof(*node.id) * 100);
    char* rest = malloc(sizeof(*rest) * 100);

    while (scanf("%[0-9a-z]", node.id) == 1) {
        printf("NodeID: %s\n", node.id);

        if (getchar() != ':') {
            // TODO: ERROR
        }

        int finished = 0;
        while(!finished) {
            if(scanf("%[0-9a-z]", rest) != 1) {
                if(getchar() != '-') {
                    // TODO: ERROR
                } else {

                }
            }
            printf("Rest: %s\n", rest);

            char x = getchar();
            if (x == '\n') {
                finished = 1;
            } else if (x == '-') {
                int value;

                if(scanf("%d", &value) != 1) {
                    // TODO: ERROR
                }

                node.value = value;
                printf("NodeValue: %d\n", node.value);
                if (getchar() != '\n') {
                    // TODO: ERROR
                }

                finished = 1;
            } else if (x != ',') {
                // TODO: ERROR
            }

        }
        printf("\n");
    }*/

    return 0;
}

int main() {
    scanContents();
}
