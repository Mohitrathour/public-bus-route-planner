#ifndef STOPS_H
#define STOPS_H

#define MAX_STOPS 100

struct node
{
    int id;
    char name[50];
    int passengers;
};

void displayStops(struct node stops[], int count);
int loadStops(struct node stops[], int maxStops);
int linearSearch(struct node stops[], int count, int id);
int binarySearchIterative(struct node stops[], int count, int id);
int binarySearchRecursive(struct node stops[], int low, int high, int id);
#endif