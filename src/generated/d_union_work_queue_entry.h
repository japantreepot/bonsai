enum work_queue_entry_type
{
  type_work_queue_entry_noop,
  type_work_queue_entry_init_world_chunk,
  type_work_queue_entry_copy_buffer,
};

struct work_queue_entry
{
  enum work_queue_entry_type Type;

  union
  {
    struct work_queue_entry_init_world_chunk work_queue_entry_init_world_chunk;
    struct work_queue_entry_copy_buffer work_queue_entry_copy_buffer;
  };
};


