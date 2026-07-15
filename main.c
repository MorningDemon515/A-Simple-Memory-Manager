#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Vector
{
	float x, y, z, w;
} Vector;

typedef struct Book
{
	int number;
	const char* name;
} Book;

typedef struct Manager
{
	void* data;
	int size;
	int total;
} Manager;

Manager* InitManager();
Vector CreateVector(Manager* manager);
Book CreateBook(Manager* manager);
void ReleaseManager(Manager* manager);

int main()
{

    Manager* manager = InitManager();
	
	Vector v = CreateVector(manager);
	Book book = CreateBook(manager);
	
	printf("%f, %f, %f, %f\n", v.x, v.y, v.z,v.w);
	printf("number: %d, name: %s \n", book.number, book.name);
	
    ReleaseManager(manager);
	system("pause");
	return 0;
}

Manager* InitManager()
{
	Manager* manager =malloc(sizeof(Manager));
	manager->data = NULL;
	manager->size = 0;
	manager->total = 0;
	return manager;
}

Vector CreateVector(Manager* manager)
{
	int currentSize = manager->size;
	int currentTotal = manager->total;
	
	void* data = realloc(manager->data ,manager->total + sizeof(Vector));
	
	if (data == NULL) {
    printf("Failed! \n");
    ReleaseManager(manager);
    exit(1); 
    }
	
	manager->data = data;
	manager->size += 1;
	manager->total += sizeof(Vector);
	
	Vector temp;
	temp.x = 1.0f;
	temp.y = 2.0f;
	temp.z = 3.0f;
	temp.w = 4.0f;
	
	void* targetAddr = (char*)manager->data + currentTotal;
	memcpy(targetAddr, &temp, sizeof(temp));
	
	return *(Vector*)targetAddr;
}

Book CreateBook(Manager* manager)
{
	int currentSize = manager->size;
	int currentTotal = manager->total;
	
	void* data = realloc(manager->data ,manager->total + sizeof(Book));
	
	if (data == NULL) {
    printf("Failed! \n");
    ReleaseManager(manager);
    exit(1); 
    }
	
	manager->data = data;
	manager->size += 1;
	manager->total += sizeof(Book);
	
	Book temp;
    temp.number = 1;
	temp.name = "the math course of pure";
	
	void* targetAddr = (char*)manager->data + currentTotal;
	memcpy(targetAddr, &temp, sizeof(temp));
	
	return *(Book*)targetAddr;
}

void ReleaseManager(Manager* manager)
{
	free(manager->data);
	free(manager);
}