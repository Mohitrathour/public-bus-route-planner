#include <stdio.h>
#include "stops.h"
int main()
{
    struct node stops[MAX_STOPS];
    int count;
    int choice;
    count = loadStops(stops, MAX_STOPS);
    choice = 0;

    while (choice != 6)
    {
        printf("\n===== PUBLIC BUS PLANNER =====\n");
        printf("1. Display Bus Stops\n");
        printf("2. Search Bus Stop\n");
        printf("3. Sort Bus Stops\n");
        printf("4. Find Shortest Route\n");
        printf("5. Plan Bus Allocation\n");
        printf("6. Exit\n");

        printf("\nEnter your choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input!\n");
            return 1;
        }

        switch (choice)
        {
            case 1:
                displayStops(stops, count);
                break;

            case 2:
            {
                int method;
                printf("1. Linear Search\n");
                printf("2. Binary Search\n");
                printf("3. Recursive Binary Search\n");
                printf("Choose search method: ");
                scanf("%d", &method);
                int id, index;
                printf("Enter the ID of the bus stop to search: ");
                if (scanf("%d", &id) != 1)
                {
                    printf("Invalid input!\n");
                    break;
                }
                if (method == 1)
                {
                    index = linearSearch(stops, count, id);
                }
                else if (method == 2)
                {
                    index = binarySearchIterative(stops, count, id);
                }
                else if (method == 3)
                {
                    index = binarySearchRecursive(stops, 0, count - 1, id);
                }
                else
                {
                    printf("Invalid search method!\n");
                    break;
                }
                if (index != -1)
                {
                    printf("Bus stop found:\n");
                    printf("Stop ID: %d\n", stops[index].id);
                    printf("Name: %s\n", stops[index].name);
                    printf("Passengers/hour: %d\n", stops[index].passengers);
                }
                else
                {
                    printf("Bus stop with ID %d not found.\n", id);
                }
                break;
            }
            case 3:
                printf("Sort function coming soon.\n");
                break;

            case 4:
                printf("Dijkstra function coming soon.\n");
                break;

            case 5:
                printf("Bus allocation coming soon.\n");
                break;

            case 6:
                printf("Exiting program...\n");
                break;

            default:
                printf("Invalid choice! Try again.\n");
        }

    }

    return 0;
}