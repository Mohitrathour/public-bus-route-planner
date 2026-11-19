#include <stdio.h>
#include "stops.h"
void displayStops(struct node stops[], int count)
{
    if (count == 0)
    {
        printf("\nNo bus stops available.\n");
        return;
    }

    printf("\n----- BUS STOP DETAILS -----\n");

    for (int i = 0; i < count; i++)
    {
        printf("\nStop ID: %d", stops[i].id);
        printf("\nName: %s", stops[i].name);
        printf("\nPassengers/hour: %d\n",
               stops[i].passengers);
    }
}
int loadStops(struct node stops[], int maxStops)
{
    FILE *fp;
    char line[200];
    int count = 0;

    fp = fopen("data/stops.csv", "r");

    if (fp == NULL)
    {
        printf("Unable to open stops.csv\n");
        return 0;
    }

    fgets(line, sizeof(line), fp);

    while (count < maxStops && fgets(line, sizeof(line), fp) != NULL)
    {
        int result = sscanf(line, "%d,%49[^,],%d", &stops[count].id,stops[count].name, &stops[count].passengers);

        if (result == 3 &&stops[count].id > 0 &&stops[count].passengers >= 0 &&stops[count].name[0] != '\0')
        {
            count++;
        }
    }

    fclose(fp);

    return count;

}
int linearSearch(struct node stops[], int count, int id)
{
    for (int i = 0; i < count; i++)
    {
        if (stops[i].id == id)
        {
            return i;
        }
    }

    return -1;
}
int binarySearchIterative(struct node stops[], int count, int id)
{
    int low = 0;
    int high = count - 1;

    while (low <= high)
    {
        int mid = low + (high - low) / 2;

        if (stops[mid].id == id)
        {
            return mid;
        }
        else if (stops[mid].id < id)
        {
            low = mid + 1;
        }
        else
        {
            high = mid - 1;
        }
    }

    return -1;
}
int binarySearchRecursive(struct node stops[], int low, int high, int id)
{
    if (low > high)
    {
        return -1;
    }

    int mid = low + (high - low) / 2;

    if (stops[mid].id == id)
    {
        return mid;
    }
    else if (stops[mid].id < id)
    {
        return binarySearchRecursive(stops, mid + 1, high, id);
    }
    else
    {
        return binarySearchRecursive(stops, low, mid - 1, id);
    }
}