#include<stdio.h>
#include<string.h>
struct node
{
    int id;
    char name[50];
    int passengers;
};
int main()
{
    struct node temp;
    temp.id=1;
    strcpy(temp.name, "Ballupur");
    temp.passengers=120;
    printf("ID: %d\nName: %s\nPassengers: %d\n", temp.id, temp.name, temp.passengers);
    return 0;
}