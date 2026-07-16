#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum Type
{
	VECTOR,
	BOOK
} Type;

typedef struct ElementRecord {
	int id;
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
	int nextId;
} Manager;

typedef struct Handle {
    int id;
} Handle;

Manager* InitManager();
Handle CreateVector(Manager* manager);
Handle CreateBook(Manager* manager);
void ReleaseManager(Manager* manager);
void* GetElements(Manager* manager, Handle handle);
void RemoveElements(Manager* manager, Handle handle);

int main()
{
    Manager* manager = InitManager();
	
	Handle _1 = CreateVector(manager);
	Handle _2 = CreateVector(manager);
	Handle _3 = CreateBook(manager);
	
    Vector v = *(Vector*)GetElements(manager, _1);
	Vector v1 = *(Vector*)GetElements(manager, _2);
	Book book = *(Book*)GetElements(manager, _3);
	printf("Before removal:\n");
	printf("v : %f, %f, %f, %f\n", v.x, v.y, v.z, v.w);
	printf("v1: %f, %f, %f, %f\n", v1.x, v1.y, v1.z, v1.w);
	printf("book: number: %d, name: %s \n\n", book.number, book.name);
	
	RemoveElements(manager, _2);

	Book* book_after = (Book*)GetElements(manager, _3);
	if (book_after != NULL) {
		printf("After removal:\n");
		printf("book: number: %d, name: %s \n\n", book_after->number, book_after->name);
	} else {
		printf("Book not found after removal!\n\n");
	}
	
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
	manager->nextId = 0;
	return manager;
}

Handle CreateVector(Manager* manager)
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
	int myId = manager->nextId++;
    manager->records[manager->size].id = myId;
    manager->records[manager->size].type = VECTOR;
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
	
	Handle h = { myId };
	
	return h;
}

Handle CreateBook(Manager* manager)
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
	int myId = manager->nextId++;
    manager->records[manager->size].id = myId;
    manager->records[manager->size].type = BOOK;      
    manager->records[manager->size].offset = currentTotal; 
	
	manager->size += 1;
	manager->total += sizeof(Book);
	
	Book temp;
    temp.number = 1;
	temp.name = "the math course of pure";
	
	void* targetAddr = (char*)manager->data + currentTotal;
	memcpy(targetAddr, &temp, sizeof(temp));
	
	Handle h = { myId };
	
	return h;
}

void* GetElements(Manager* manager, Handle handle)
{
	for (int i = 0; i < manager->size; i++) {
        if (manager->records[i].id == handle.id) {
            return (char*)manager->data + manager->records[i].offset;
        }
    }
    return NULL;
}

void RemoveElements(Manager* manager, Handle handle)
{
	int targetIndex = -1;
	
	for (int i = 0; i < manager->size; i++) {
        if (manager->records[i].id == handle.id) {
			targetIndex = i;
			break;
        }
    }
	
	if (targetIndex == -1) {
		printf("Handle not found!\n");
		return;
	}

	Type targetType = manager->records[targetIndex].type;
	int targetOffset = manager->records[targetIndex].offset;
	int targetSize = 0;
	
	switch (targetType)
	{
		case VECTOR:
		    targetSize = sizeof(Vector);
			printf("[Removal] Releasing Vector at offset %d\n", targetOffset);
		    break;
		
		case BOOK:
		    targetSize = sizeof(Book);
			printf("[Removal] Releasing Book at offset %d\n", targetOffset);
		    break;
			
		default:
            break;		
	}

	int moveBytes = manager->total - (targetOffset + targetSize);
	if (moveBytes > 0) {
		void* dest = (char*)manager->data + targetOffset;
		void* src = (char*)manager->data + targetOffset + targetSize;
		memmove(dest, src, moveBytes);
	}
	
	for (int i = targetIndex; i < manager->size - 1; i++) {
		manager->records[i] = manager->records[i + 1];
		manager->records[i].offset -= targetSize;
	}
	
	manager->size -= 1;
	manager->total -= targetSize;

	if (manager->total > 0) {
		manager->data = realloc(manager->data, manager->total);
	} else {
		free(manager->data);
		manager->data = NULL;
	}
	
	if (manager->size > 0) {
		manager->records = realloc(manager->records, manager->size * sizeof(ElementRecord));
	} else {
		free(manager->records);
		manager->records = NULL;
	}
}

void ReleaseManager(Manager* manager)
{
	if (manager == NULL) return;

    for (int i = 0; i < manager->size; i++) {

        void* itemAddr = (char*)manager->data + manager->records[i].offset;
        
        switch (manager->records[i].type) {
            case BOOK:
                printf("Releasing Book at offset %d\n", manager->records[i].offset);
                break;
                
            case VECTOR:
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