
    
    struct parser_stream_chunk
    {
      parser Element;
      parser_stream_chunk* Next;
    };

    
    struct parser_stream
    {
      parser_stream_chunk* FirstChunk;
      parser_stream_chunk* LastChunk;
    };


    
    struct parser_iterator
    {
      parser_stream* Stream;
      parser_stream_chunk* At;
    };

    function parser_iterator
    Iterator(parser_stream* Stream)
    {
      parser_iterator Iterator = {
        .Stream = Stream,
        .At = Stream->FirstChunk
      };
      return Iterator;
    }

    function b32
    IsValid(parser_iterator* Iter)
    {
      b32 Result = Iter->At != 0;
      return Result;
    }

    function void
    Advance(parser_iterator* Iter)
    {
      Iter->At = Iter->At->Next;
    }


    
    function void
    Push(parser_stream* Stream, parser Element, memory_arena* Memory)
    {
      parser_stream_chunk* NextChunk = (parser_stream_chunk*)PushStruct(Memory, sizeof( parser_stream_chunk ), 1, 0);
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

      return;
    }

