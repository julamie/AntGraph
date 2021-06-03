#include <stdio.h>

int scanContents() {
    char id[40];
    char nodeIDs[40][20];

    if (scanf("%[^:]", id) != 1) {
        fprintf(stderr, "Error");
    }
    while (getchar() != '\n') {
        if (scanf("%[^,\n]", id) != 1) {
            fprintf(stderr, "Error");
        }
    }

    printf("%s ", id);
    for (int i = 0; i < 20; i++) {
        printf("%s", nodeIDs[i]);
    }

    return 0;
}

int main() {

}
