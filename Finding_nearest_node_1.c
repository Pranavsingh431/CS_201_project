#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define MAX_CITIES 100

typedef struct Point {
    int x, y;
} Point;

typedef struct Node {
    Point pos;
    char city[50];
} Node;

typedef struct Quad {
    Point NorthWest, SouthEast;
    Node* n;
    struct Quad *NorthWestTree, *NorthEastTree, *SouthWestTree, *SouthEastTree;
} Quad;

typedef struct {
    Node* nodes[MAX_CITIES];
    int count;
} NodeList;

// Function to allocate memory for a new Quad
Quad* newQuad(Point NorthWest, Point SouthEast) {
    Quad* quad = (Quad*)malloc(sizeof(Quad));
    quad->NorthWest = NorthWest;
    quad->SouthEast = SouthEast;
    quad->n = NULL;
    quad->NorthWestTree = NULL;
    quad->NorthEastTree = NULL;
    quad->SouthWestTree = NULL;
    quad->SouthEastTree = NULL;
    return quad;
}

// Check if a point is within the boundary
int inBoundary(Point NorthWest, Point SouthEast, Point p) {
    return (p.x >= NorthWest.x && p.x <= SouthEast.x && p.y >= NorthWest.y && p.y <= SouthEast.y);
}

// Calculate Euclidean distance between two points
double calculateDistance(Point p1, Point p2) {
    return sqrt(pow(p1.x - p2.x, 2) + pow(p1.y - p2.y, 2));
}

// Insert a city node into the Quadtree
void insert(Quad* root, Node* node) {
    if (node == NULL || !inBoundary(root->NorthWest, root->SouthEast, node->pos)) {
        return;
    }

    if (root->n == NULL && root->NorthWestTree == NULL && root->SouthEastTree == NULL &&
        root->SouthWestTree == NULL && root->NorthEastTree == NULL) {
        root->n = node;
        return;
    }

    if (root->n != NULL) {
        Node* temp = root->n;
        root->n = NULL;
        insert(root, temp);
    }

    int xMid = (root->NorthWest.x + root->SouthEast.x) / 2;
    int yMid = (root->NorthWest.y + root->SouthEast.y) / 2;

    if (node->pos.x <= xMid) {
        if (node->pos.y <= yMid) {
            if (root->NorthWestTree == NULL) {
                root->NorthWestTree = newQuad(root->NorthWest, (Point){xMid, yMid});
            }
            insert(root->NorthWestTree, node);
        } else {
            if (root->SouthWestTree == NULL) {
                root->SouthWestTree = newQuad((Point){root->NorthWest.x, yMid}, (Point){xMid, root->SouthEast.y});
            }
            insert(root->SouthWestTree, node);
        }
    } else {
        if (node->pos.y <= yMid) {
            if (root->NorthEastTree == NULL) {
                root->NorthEastTree = newQuad((Point){xMid, root->NorthWest.y}, (Point){root->SouthEast.x, yMid});
            }
            insert(root->NorthEastTree, node);
        } else {
            if (root->SouthEastTree == NULL) {
                root->SouthEastTree = newQuad((Point){xMid, yMid}, root->SouthEast);
            }
            insert(root->SouthEastTree, node);
        }
    }
}

// Search for the nearest cities
void searchNearest(Quad* root, Point p, NodeList* bestMatches, double* bestDist) {
    if (root == NULL) return;

    if (root->n != NULL) {
        double dist = calculateDistance(p, root->n->pos);
        if (dist < *bestDist) {
            *bestDist = dist;
            bestMatches->count = 1;
            bestMatches->nodes[0] = root->n;
        } else if (dist == *bestDist) {
            if (bestMatches->count < MAX_CITIES) {
                bestMatches->nodes[bestMatches->count++] = root->n;
            }
        }
    }

    if (root->NorthWestTree != NULL) searchNearest(root->NorthWestTree, p, bestMatches, bestDist);
    if (root->NorthEastTree != NULL) searchNearest(root->NorthEastTree, p, bestMatches, bestDist);
    if (root->SouthWestTree != NULL) searchNearest(root->SouthWestTree, p, bestMatches, bestDist);
    if (root->SouthEastTree != NULL) searchNearest(root->SouthEastTree, p, bestMatches, bestDist);
}

// Search for cities within a specified radius
void searchWithinRadius(Quad* root, Point p, double radius, NodeList* results) {
    if (root == NULL) return;

    if (root->n != NULL) {
        double dist = calculateDistance(p, root->n->pos);
        if (dist <= radius) {
            results->nodes[results->count++] = root->n;
        }
    }

    if (root->NorthWestTree != NULL) searchWithinRadius(root->NorthWestTree, p, radius, results);
    if (root->NorthEastTree != NULL) searchWithinRadius(root->NorthEastTree, p, radius, results);
    if (root->SouthWestTree != NULL) searchWithinRadius(root->SouthWestTree, p, radius, results);
    if (root->SouthEastTree != NULL) searchWithinRadius(root->SouthEastTree, p, radius, results);
}

// Delete a city node from the Quadtree
void deleteCity(Quad* root, Point p) {
    if (root == NULL) return;

    if (root->n != NULL && root->n->pos.x == p.x && root->n->pos.y == p.y) {
        printf("Deleting city: %s\n", root->n->city);
        free(root->n);
        root->n = NULL;
        return;
    }

    int xMid = (root->NorthWest.x + root->SouthEast.x) / 2;
    int yMid = (root->NorthWest.y + root->SouthEast.y) / 2;

    if (p.x <= xMid) {
        if (p.y <= yMid) {
            deleteCity(root->NorthWestTree, p);
        } else {
            deleteCity(root->SouthWestTree, p);
        }
    } else {
        if (p.y <= yMid) {
            deleteCity(root->NorthEastTree, p);
        } else {
            deleteCity(root->SouthEastTree, p);
        }
    }
}

// Free memory for the entire Quadtree
void freeQuad(Quad* root) {
    if (root == NULL) return;

    freeQuad(root->NorthWestTree);
    freeQuad(root->NorthEastTree);
    freeQuad(root->SouthWestTree);
    freeQuad(root->SouthEastTree);

    if (root->n != NULL) {
        free(root->n);
    }

    free(root);
}

// Function to read data from a file and insert into the Quadtree
void readFileData(Quad* quad, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Unable to open file.\n");
        return;
    }

    int x, y;
    char city[50];
    while (fscanf(file, "%d %d %s", &x, &y, city) == 3) {
        Node* node = (Node*)malloc(sizeof(Node));
        node->pos = (Point){x, y};
        strcpy(node->city, city);
        insert(quad, node);
    }

    fclose(file);
    printf("Data loaded from file successfully.\n");
}

// Main function
int main() {
    Point NorthWest = {0, 0};
    Point SouthEast = {128, 128};
    Quad* quad = newQuad(NorthWest, SouthEast);
    int choice;

    printf("Choose input method:\n");
    printf("1. Manual Entry\n");
    printf("2. Load from File\n");
    printf("Enter choice: ");
    scanf("%d", &choice);

    if (choice == 1) {
        int n;
        printf("Enter the number of cities you want to add: ");
        scanf("%d", &n);

        while (n--) {
            int l, r;
            char name[50];
            printf("Enter coordinates and city name: ");
            scanf("%d %d %s", &l, &r, name);

            Point pos = {l, r};
            Node* node = (Node*)malloc(sizeof(Node));
            node->pos = pos;
            strcpy(node->city, name);

            insert(quad, node);
        }
    } else if (choice == 2) {
        char filename[100];
        printf("Enter the filename to load data from: ");
        scanf("%s", filename);
        readFileData(quad, filename);
    } else {
        printf("Invalid choice. Exiting.\n");
        freeQuad(quad);
        return 0;
    }

    int f;
    while (1) {
        printf("Choose an option:\n");
        printf("1. Search for the nearest city\n");
        printf("2. Delete a city\n");
        printf("3. Search for cities within a radius\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &f);

        if (f == 1) {
            Point p;
            printf("Enter coordinates: ");
            scanf("%d %d", &p.x, &p.y);

            NodeList bestMatches = {.count = 0};
            double bestDist = 1e9;
            searchNearest(quad, p, &bestMatches, &bestDist);

            if (bestMatches.count > 0) {
                printf("Nearest city/cities at distance %.2lf:\n", bestDist);
                for (int i = 0; i < bestMatches.count; i++) {
                printf("City: %s\n", bestMatches.nodes[i]->city);
        }
            } else {
                printf("No cities found.\n");
            }
        } else if (f == 2) {
            Point p;
            printf("Enter coordinates of the city to delete: ");
            scanf("%d %d", &p.x, &p.y);
            deleteCity(quad, p);
        } else if (f == 3) {
            Point p;
            double radius;
            printf("Enter center point coordinates and radius: ");
            scanf("%d %d %lf", &p.x, &p.y, &radius);

            NodeList results = {.count = 0};
            searchWithinRadius(quad, p, radius, &results);

            printf("Cities within radius %.2lf:\n", radius);
            for (int i = 0; i < results.count; i++) {
                printf("City: %s\n", results.nodes[i]->city);
            }
        } else if (f == 0) {
            break;
        } else {
            printf("Invalid choice. Try again.\n");
        }
    }

    freeQuad(quad);
    return 0;
}
