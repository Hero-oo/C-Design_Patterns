#include <stdio.h>
#include <stdlib.h>

/**
 * @brief 迭代器接口
 */
typedef struct {
    void *data;
    void (*next)(void *itr);        // 获取下一个元素
    int (*has_next)(void *itr);     // 判断是否还有下一个元素
    void (*reset)(void *itr);       // 重置迭代器状态
} iterator_t;

/**
 * @brief 迭代器实现，数组迭代器，遍历数组元素
 */
typedef struct {
    iterator_t iterator;
    int *array;
    int size;
    int index;
} array_iterator_t;

void array_iterator_next(void *itr) 
{
    array_iterator_t *iterator = (array_iterator_t *)itr;
    iterator->index++;
}

int array_iterator_has_next(void *itr) 
{
    array_iterator_t *iterator = (array_iterator_t *)itr;
    return iterator->index < iterator->size;
}

void array_iterator_reset(void *itr) 
{
    array_iterator_t *iterator = (array_iterator_t *)itr;
    iterator->index = 0;
}

int *array_iterator_get_data(void* itr) 
{
    array_iterator_t *iterator = (array_iterator_t *)itr;
    return &(iterator->array[iterator->index]);
}

iterator_t *create_array_iterator(int *array, int size) 
{
    array_iterator_t *iterator = malloc(sizeof(array_iterator_t));
    iterator->iterator.data = iterator;
    iterator->iterator.next = array_iterator_next;
    iterator->iterator.has_next = array_iterator_has_next;
    iterator->iterator.reset = array_iterator_reset;
    iterator->array = array;
    iterator->size = size;
    iterator->index = 0;
    return &(iterator->iterator);
}

/**
 * @brief 集合接口
 */
typedef struct collection 
{
    iterator_t *(*get_iterator)(struct collection *collection);
    void (*add)(struct collection *collection, int element);    // 向集合中添加元素
    int (*get_size)(struct collection *collection);              // 获取集合大小
    void (*free)(struct collection *collection);                // 释放集合内存
} collection_t;

/**
 * @brief 集合实现
 */
typedef struct {
    collection_t collection;
    int *elements;
    int size;
    int capacity;
} array_list_t;

iterator_t *array_list_get_iterator(collection_t *collection) 
{
    array_list_t *array_list = (array_list_t *)collection;
    return create_array_iterator(array_list->elements, array_list->size);
}

void array_list_add(collection_t *collection, int element) 
{
    array_list_t *array_list = (array_list_t *)collection;
    
    if (array_list->size >= array_list->capacity) {
        int new_capacity = array_list->capacity * 2;
        int *new_elements = realloc(array_list->elements, new_capacity * sizeof(int));
        if (new_elements != NULL) {
            array_list->elements = new_elements;
            array_list->capacity = new_capacity;
        } else {
            printf("Failed to expand the collection capacity.\n");
            return;
        }
    }
    
    array_list->elements[array_list->size] = element;
    array_list->size++;
}

int array_list_get_size(collection_t *collection) 
{
    array_list_t *array_list = (array_list_t *)collection;
    return array_list->size;
}

void array_list_free(collection_t *collection) 
{
    array_list_t *array_list = (array_list_t *)collection;
    free(array_list->elements);
    free(array_list);
}

collection_t *create_array_list() 
{
    array_list_t *array_list = malloc(sizeof(array_list_t));
    array_list->collection.get_iterator = array_list_get_iterator;
    array_list->collection.add = array_list_add;
    array_list->collection.get_size = array_list_get_size;
    array_list->collection.free = array_list_free;
    
    array_list->elements = malloc(10 * sizeof(int));
    array_list->size = 0;
    array_list->capacity = 10;
    
    return &(array_list->collection);
}

// 客户端代码
void print_elements(collection_t *collection) 
{
    iterator_t *iterator = collection->get_iterator(collection);
    while (iterator->has_next(iterator->data)) {
        int *element = array_iterator_get_data(iterator->data);
        printf("%d ", *element);
        iterator->next(iterator->data);
    }
    printf("\n");
    iterator->reset(iterator->data);
}

int main() 
{
    collection_t *collection = create_array_list();

    collection->add(collection, 0);
    collection->add(collection, 1);
    collection->add(collection, 2);
    collection->add(collection, 3);

    printf("Size of collection: %d\n", collection->get_size(collection));

    printf("Elements in collection: ");
    print_elements(collection);

    collection->free(collection);

    return 0;
}