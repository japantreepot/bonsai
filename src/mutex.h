#define TOTAL_MUTEX_OP_RECORDS (1024*1024*1024)

enum mutex_op
{
  MutexOp_Waiting,
  MutexOp_Aquired,
  MutexOp_Released
};

struct mutex
{
  native_mutex M;

#if BONSAI_INTERNAL
  struct mutex_op_record *DebugRecord;
#endif
};

struct mutex_op_record
{
  mutex *Mutex;
  u64 Cycle;
  mutex_op Op;
};

