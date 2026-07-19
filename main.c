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
    void* ptr; 
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
	int size;
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
	
    Vector* v = (Vector*)GetElements(manager, _1);
	Vector* v1 = (Vector*)GetElements(manager, _2);
	Book* book = (Book*)GetElements(manager, _3);
	printf("Before removal:\n");
	printf("v : %f, %f, %f, %f\n", v->x, v->y, v->z, v->w);
	printf("v1: %f, %f, %f, %f\n", v1->x, v1->y, v1->z, v1->w);
	printf("book: number: %d, name: %s \n\n", book->number, book->name);
	
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
	Manager* manager = malloc(sizeof(Manager));
	
	if (manager != NULL)
	{
        manager->records = NULL;
	    manager->size = 0;
	    manager->nextId = 0;		
	}
	
	return manager;
}

Handle CreateVector(Manager* manager)
{

    Vector* temp = malloc(sizeof(Vector));
	temp->x = 1.0f;
	temp->y = 0.0f;
	temp->z = 1.0f;
	temp->w = 0.0f;

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
	manager->records[manager->size].ptr = temp;
	
	manager->size += 1;
	
	Handle h = { myId };
	
	return h;
}

Handle CreateBook(Manager* manager)
{
	
	Book* temp = malloc(sizeof(Book));
	temp->number = 10;
	temp->name = "A course of pure math";

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
	manager->records[manager->size].ptr = temp;
	
	manager->size += 1;
	
	Handle h = { myId };
	
	return h;
}

void* GetElements(Manager* manager, Handle handle)
{
	for (int i = 0; i < manager->size; i++) {
        if (manager->records[i].id == handle.id) {
            return manager->records[i].ptr;
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
	void* itemAddr = manager->records[targetIndex].ptr;
	
	if (itemAddr != NULL)
	{
		switch (targetType)
	    {
		case VECTOR:
		    free(itemAddr);
			printf("[Removal] Releasing Vector, Handle: %d\n", manager->records[targetIndex].id);
		    break;
		
		case BOOK:
		    free(itemAddr);
			printf("[Removal] Releasing Book, Handle: %d\n", manager->records[targetIndex].id);
		    break;
			
		default:
            break;		
	    }
	}
	
	for (int i = targetIndex; i < manager->size - 1; i++) {
		manager->records[i] = manager->records[i + 1];
	}
	
	manager->size -= 1;
	
	if (manager->size > 0) {
		void* temp = realloc(manager->records, manager->size * sizeof(ElementRecord));
		if (temp != NULL)
		{
			manager->records = temp;
		}
	} else {
		free(manager->records);
		manager->records = NULL;
	}
}

void ReleaseManager(Manager* manager)
{
	if (manager == NULL) return;

    for (int i = 0; i < manager->size; i++) {

        void* itemAddr = manager->records[i].ptr;
        
        switch (manager->records[i].type) {
            case BOOK:
                free(itemAddr);
				printf("Released Book, Handle: %d\n", manager->records[i].id);
                break;
                
            case VECTOR:
			    free(itemAddr);
				printf("Released VECTOR, Handle: %d\n", manager->records[i].id);
                break;
            
            default:
                break;
        }
    }

    free(manager->records); 
    free(manager);       
}