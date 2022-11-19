#include "queue.h"
#include <arduino.h>



Queue::Queue() {
  init_sending_queue();
}

void Queue::init_sending_queue() {
  queue_struct *pool;
  pool = (queue_struct*)heap_caps_malloc(sizeof(queue_struct) * 64, MALLOC_CAP_8BIT);

  for (int i = 0; i < 64; i++) {
    pool[i].next = free_pool;
    free_pool = &pool[i];
  }
}



void Queue::need_free_items() {
  if (!enough_free_memory(20000)) {
    remove_first();
    Serial.println("NOT ENOUGH MEMORY, REMOVING FIRST ITEM");
  } else {
    init_sending_queue();
  }
}


queue_struct * Queue::get_one_free_item_and_add_to_queue() {
  queue_struct *new_item = free_pool;
  free_pool = (queue_struct*)new_item->next;
  return new_item;
}


void Queue::put_item_into_queue(queue_struct * new_item) {
  if (!first_item) {
    first_item = new_item;
    last_item = new_item;
  } else {
    last_item->next = new_item;
    last_item = new_item;
  }

  last_item->next = (queue_struct*)0;
}

bool Queue::enough_free_memory(long needed) {
  if (heap_caps_get_free_size(MALLOC_CAP_8BIT) < needed) {
    return false;
  }
  return true;
}

bool Queue::make_free_memory(long needed) {
  int deleted = 0;
  while (!enough_free_memory(needed) && deleted < 100) {
    remove_first();
    deleted += 1;
    Serial.println(heap_caps_get_free_size(MALLOC_CAP_8BIT));
    Serial.println("NOT ENOUGH MEMORY, DELETING FIRST ITEM");
  }

  if (!enough_free_memory(needed)) {
    return false;
  }
  return true;
}





void Queue::add_to_queue(void* data, int size) {
  make_free_memory(size + 20000);
  if (!free_pool) {
    need_free_items();
  }

  queue_struct *new_item = get_one_free_item_and_add_to_queue();
  void* data_dto = (void*)heap_caps_malloc(size, MALLOC_CAP_8BIT);
  memcpy(data_dto, data, size);
  new_item->data = data_dto;
  new_item->data_size = size;

  new_item->next = (void*)0;
  put_item_into_queue(new_item);
  in_list++;
}



void * Queue::get_first_data() {
  return (void*)first_item->data;
}

int Queue::get_first_size() {
  return (int)first_item->data_size;
}



void Queue::remove_first() {
  if (first_item) {
    in_list --;
    free(first_item->data);
    queue_struct* second = (queue_struct*)first_item->next;
    first_item->next = free_pool;
    free_pool = first_item;
    first_item = second;
  }
}
