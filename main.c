#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum Type
{
	T_Vector,
	T_Book
} Type;

typedef struct ElementRecord {
    Type type;
    int offset;    
} ElementRecord;

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
	
	ElementRecord* records;
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
	manager->records = NULL;
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
	
	ElementRecord* newRecords = realloc(manager->records, (manager->size + 1) * sizeof(ElementRecord));
    if (newRecords == NULL) {
        printf("Failed to allocate records! \n");
        ReleaseManager(manager);
        exit(1);
    }
    manager->records = newRecords;
    manager->records[manager->size].type = T_Vector;
    manager->records[manager->size].offset = currentTotal;
	
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
	
	ElementRecord* newRecords = realloc(manager->records, (manager->size + 1) * sizeof(ElementRecord));
    if (newRecords == NULL) {
        printf("Failed to allocate records! \n");
        ReleaseManager(manager);
        exit(1);
    }
    manager->records = newRecords;
    manager->records[manager->size].type = T_Book;      
    manager->records[manager->size].offset = currentTotal; 
	
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
	if (manager == NULL) return;

    for (int i = 0; i < manager->size; i++) {

        void* itemAddr = (char*)manager->data + manager->records[i].offset;
        
        switch (manager->records[i].type) {
            case T_Book:
                printf("Releasing Book at offset %d\n", manager->records[i].offset);
                break;
                
            case T_Vector:
			    printf("Releasing Vector at offset %d\n", manager->records[i].offset);
                break;
            
            default:
                break;
        }
    }

    free(manager->records); 
    free(manager->data);  
    free(manager);       
}