#ifndef MESSAGE_QUEUE_H
#define MESSAGE_QUEUE_H

typedef struct  {
  void * next;
  int data_size;
  void * data;
} queue_struct;


class Queue {
  public:
    void add_to_queue(void* data, int size);
    void * get_first_data();
    int get_first_size();
    void remove_first();
    Queue();

  private:
    queue_struct* first_item = 0;
    queue_struct* last_item;
    queue_struct* free_pool;
    int in_list = 0;
    int free_size = 0;


    void init_sending_queue();
    void need_free_items();
    queue_struct * get_one_free_item_and_add_to_queue();
    void put_item_into_queue(queue_struct * new_item);
    bool enough_free_memory(long needed);
    bool make_free_memory(long needed);
};


#endif
