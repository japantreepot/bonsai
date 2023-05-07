struct u64_cursor
{
  u64 *Start;
  // TODO(Jesse)(immediate): For the love of fucksakes change these to indices
  u64 *At;
  u64 *End;
};

link_internal u64_cursor
U64Cursor(umm ElementCount, memory_arena* Memory)
{
  u64 *Start = (u64*)PushStruct(Memory, sizeof(u64)*ElementCount, 1, 0);
  u64_cursor Result = {
    .Start = Start,
    .End = Start+ElementCount,
    .At = Start,
  };
  return Result;
}

link_internal u64
Get(u64_cursor *Cursor, umm ElementIndex)
{
  Assert(ElementIndex < CurrentCount(Cursor));
  u64 Result = Cursor->Start[ElementIndex];
  return Result;
}

struct u64_stream_chunk
{
  u64 Element;
  u64_stream_chunk* Next;
};

struct u64_stream
{
  memory_arena *Memory;
  u64_stream_chunk* FirstChunk;
  u64_stream_chunk* LastChunk;
  umm ChunkCount;
};

link_internal void
Deallocate(u64_stream *Stream)
{
  Stream->LastChunk = 0;
  Stream->FirstChunk = 0;
  VaporizeArena(Stream->Memory);
}

struct u64_iterator
{
  u64_stream* Stream;
  u64_stream_chunk* At;
};

link_internal u64_iterator
Iterator(u64_stream* Stream)
{
  u64_iterator Iterator = {
    .Stream = Stream,
    .At = Stream->FirstChunk
  };
  return Iterator;
}

link_internal b32
IsValid(u64_iterator* Iter)
{
  b32 Result = Iter->At != 0;
  return Result;
}

link_internal void
Advance(u64_iterator* Iter)
{
  Iter->At = Iter->At->Next;
}

link_internal b32
IsLastElement(u64_iterator* Iter)
{
  b32 Result = Iter->At->Next == 0;
  return Result;
}


link_internal u64 *
Push(u64_stream* Stream, u64 Element)
{
  if (Stream->Memory == 0)
  {
    Stream->Memory = AllocateArena();
  }

  /* (Type.name)_stream_chunk* NextChunk = AllocateProtection((Type.name)_stream_chunk*), Stream->Memory, 1, False) */
  u64_stream_chunk* NextChunk = (u64_stream_chunk*)PushStruct(Stream->Memory, sizeof(u64_stream_chunk), 1, 0);
  NextChunk->Element = Element;

  if (!Stream->FirstChunk)
  {
    Assert(!Stream->LastChunk);
    Stream->FirstChunk = NextChunk;
    Stream->LastChunk = NextChunk;
  }
  else
  {
    Stream->LastChunk->Next = NextChunk;
    Stream->LastChunk = NextChunk;
  }

  Assert(NextChunk->Next == 0);
  Assert(Stream->LastChunk->Next == 0);

  Stream->ChunkCount += 1;

  u64 *Result = &NextChunk->Element;
  return Result;
}



