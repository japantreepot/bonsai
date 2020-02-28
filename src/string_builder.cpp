counted_string
CountedString(umm Count, memory_arena* Memory)
{
  TIMED_FUNCTION();
  counted_string Result = {
    .Start = AllocateProtection(const char, Memory, Count, False),
    .Count = Count
  };
  return Result;
}

counted_string
CountedString(const char* Start, memory_arena* Memory)
{
  TIMED_FUNCTION();
  umm StringLength = Length(Start);

  counted_string Result = CountedString(StringLength, Memory);
  MemCopy((u8*)Start, (u8*)Result.Start, StringLength);

  return Result;
}

counted_string
CountedString(const char* Start, umm Count, memory_arena* Memory)
{
  TIMED_FUNCTION();
  counted_string Result = {
    .Start = AllocateProtection(const char, Memory, Count, False),
    .Count = Count
  };

  MemCopy((u8*)Start, (u8*)Result.Start, Count);

  return Result;
}

function void
Append(string_builder* Builder, counted_string String)
{
  TIMED_FUNCTION();
  Push(&Builder->Chunks, String, Builder->Memory);
}

function counted_string
Finalize(string_builder* Builder, memory_arena* PermMemory)
{
  TIMED_FUNCTION();
  u32 TotalMemRequired = 0;

  ITERATE_OVER(counted_string, &Builder->Chunks)
  {
    counted_string* At = GET_ELEMENT(Iter);
    TotalMemRequired += At->Count;
  }

  counted_string Result = CountedString(TotalMemRequired, PermMemory);

  u32 AtIndex = 0;

  ITERATE_OVER(counted_string, &Builder->Chunks)
  {
    counted_string* At = GET_ELEMENT(Iter);
    MemCopy((u8*)At->Start, (u8*)(Result.Start+AtIndex), At->Count);
    AtIndex += At->Count;
    Assert(AtIndex <= Result.Count);
  }
  Assert(AtIndex == Result.Count);

  VaporizeArena(Builder->Memory);
  return Result;
}

function counted_string
Concat(counted_string S1, counted_string S2, memory_arena* Memory)
{
  umm TotalLength = S1.Count + S2.Count;
  counted_string Result = {
    .Start = AllocateProtection(char, Memory, TotalLength, False),
    .Count = TotalLength
  };

  MemCopy((u8*)S1.Start, (u8*)Result.Start, S1.Count);
  MemCopy((u8*)S2.Start, (u8*)Result.Start+S1.Count, S2.Count);

  return Result;
}

function const char*
GetNullTerminated(counted_string Str, memory_arena* Memory = TranArena)
{
  const char* Result = AllocateProtection(const char, Memory, Str.Count+1, False);
  MemCopy((u8*)Str.Start, (u8*)Result, Str.Count);
  return Result;
}

function char
ToUpper(char C)
{
  u32 MapToUpper = 'a' - 'A';
  if (C >= 'a' && C <= 'z')
  {
    C -= MapToUpper;
  }
  return C;
}

function counted_string
ToCapitalCase(counted_string Source, memory_arena* Memory)
{
  u32 ResultLength = 0;
  for (u32 CharIndex = 0;
      CharIndex < Source.Count;
      ++CharIndex)
  {
    if (Source.Start[CharIndex] != '_')
    {
      ++ResultLength;
    }
  }

  counted_string Result = CountedString(ResultLength, Memory);

  b32 NextCharToUpper = True;
  u32 SourceIndex = 0;
  for (u32 ResultIndex = 0;
      ResultIndex < Result.Count;
      )
  {
    char At = Source.Start[SourceIndex];

    if (At == '_')
    {
      NextCharToUpper = True;
      ++SourceIndex;
      continue;
    }

    if (NextCharToUpper)
    {
      At = ToUpper(At);
      NextCharToUpper = False;
    }

    // TODO(Jesse): Gross..
    *(char*)(Result.Start + ResultIndex) = At;
    ++SourceIndex;
    ++ResultIndex;
  }

  return Result;
}

function char*
MemorySize(u64 Number)
{
  r64 KB = (r64)Kilobytes(1);
  r64 MB = (r64)Megabytes(1);
  r64 GB = (r64)Gigabytes(1);

  r64 Display = (r64)Number;
  char Units = ' ';

  if (Number >= KB && Number < MB)
  {
    Display = Number / KB;
    Units = 'K';
  }
  else if (Number >= MB && Number < GB)
  {
    Display = Number / MB;
    Units = 'M';
  }
  else if (Number >= GB)
  {
    Display = Number / GB;
    Units = 'G';
  }


  char *Buffer = AllocateProtection(char, TranArena, 32, False);
  snprintf(Buffer, 32, "%.1f%c", (r32)Display, Units);
  return Buffer;
}

template<typename number_type>counted_string
NumericValueToString(number_type Number, const char* Format)
{
  u32 BufferLength = 32;
  char *Buffer = AllocateProtection(char, TranArena, BufferLength, False);
  snprintf(Buffer, BufferLength, Format, Number);

  counted_string Result = CS(Buffer);
  return Result;
}

function counted_string
CS(s64 Number)
{
  counted_string Result = NumericValueToString(Number, "%ld");
  return Result;
}

function counted_string
CS(u64 Number)
{
  counted_string Result = NumericValueToString(Number, "%lu");
  return Result;
}

function counted_string
CS(s32 Number)
{
  counted_string Result = NumericValueToString(Number, "%i");
  return Result;
}

function counted_string
CS(u32 Number)
{
  counted_string Result = NumericValueToString(Number, "%u");
  return Result;
}

function counted_string
CS(r64 Number)
{
  counted_string Result = NumericValueToString(Number, "%.2f");
  return Result;
}

function counted_string
CS(r32 Number)
{
  counted_string Result = NumericValueToString(Number, "%.2f");
  return Result;
}

function counted_string
CS(v2 V)
{
  char *Buffer = AllocateProtection(char, TranArena, 32, False);
  snprintf(Buffer, 32, "(%.2f,%.2f)", V.x, V.y);
  counted_string Result = CS(Buffer);
  return Result;
}

function char*
ToString(u64 Number)
{
  char *Buffer = AllocateProtection(char, TranArena, 32, False);
  snprintf(Buffer, 32, "%lu", Number);
  return Buffer;
}

function char*
ToString(s32 Number)
{
  char *Buffer = AllocateProtection(char, TranArena, 32, False);
  snprintf(Buffer, 32, "%i", Number);
  return Buffer;
}

function char*
ToString(u32 Number)
{
  char *Buffer = AllocateProtection(char, TranArena, 32, False);
  snprintf(Buffer, 32, "%u", Number);
  return Buffer;
}

function char*
ToString(r32 Number)
{
  char *Buffer = AllocateProtection(char, TranArena, 32, False);
  snprintf(Buffer, 32, "%.2f", Number);
  return Buffer;
}

function char*
FormatMemorySize(u64 Number)
{
  r64 KB = (r64)Kilobytes(1);
  r64 MB = (r64)Megabytes(1);
  r64 GB = (r64)Gigabytes(1);

  r64 Display = (r64)Number;
  char Units = ' ';

  if (Number >= KB && Number < MB)
  {
    Display = Number / KB;
    Units = 'K';
  }
  else if (Number >= MB && Number < GB)
  {
    Display = Number / MB;
    Units = 'M';
  }
  else if (Number >= GB)
  {
    Display = Number / GB;
    Units = 'G';
  }

  char *Buffer = AllocateProtection(char, TranArena, 32, False);
  snprintf(Buffer, 32, "%.1f%c", (r32)Display, Units);

  return Buffer;
}

function char*
FormatThousands(u64 Number)
{
  u64 OneThousand = 1000;
  r32 Display = (r32)Number;
  char Units = ' ';

  if (Number >= OneThousand)
  {
    Display = Number / (r32)OneThousand;
    Units = 'K';
  }

  char *Buffer = AllocateProtection(char, TranArena, 32, False);
  snprintf(Buffer, 32, "%.1f%c", Display, Units);

  return Buffer;
}

// Note(Jesse): Shamelessly copied, then modified, from the Handmade Hero codebase
global_variable char DecChars[] = "0123456789";
global_variable char LowerHexChars[] = "0123456789abcdef";
global_variable char UpperHexChars[] = "0123456789ABCDEF";

// TODO(Jesse): Figure out how to not require a statically allocated buffer size
global_variable u32 MaxResultLength = 32;
function void
CopyToDest(char* Dest, u32* Count, char C)
{
  Dest[*Count] = C;
  *Count = *Count+1;
  Assert(*Count < MaxResultLength);
}

function counted_string
U64ToCountedString(memory_arena* Memory, u64 Value, u32 Base = 10, char *Digits = DecChars)
{
  TIMED_FUNCTION();
  Assert(Base != 0);

  // TODO(Jesse): This is over-allocating, and we should consider not doing this
  // in the future!
  char* StartAlloc = AllocateProtection(char, Memory, MaxResultLength, False);
  u32 Count = 0;

  do
  {
      u64 DigitIndex = (Value % Base);
      char Digit = Digits[DigitIndex];

      CopyToDest(StartAlloc, &Count, Digit);

      Value /= Base;

  } while(Value != 0);

  char *Start = (char*)StartAlloc;
  char *End = (char*)StartAlloc+Count;

  while(Start < End)
  {
      --End;
      char Temp = *End;
      *End = *Start;
      *Start = Temp;
      ++Start;
  }

  counted_string Result = { .Start = StartAlloc, .Count = Count };
  return Result;
}

function counted_string
S64ToCountedString(memory_arena* Memory, s64 Value, u32 Base = 10, char *Digits = DecChars)
{
  TIMED_FUNCTION();
  counted_string Result = {};
  if (Value < 0)
  {
    Result = CS("-");
    // TODO(Jesse): This concat should not be here!
    Result = Concat(Result, U64ToCountedString(Memory, (u64)-Value, Base, Digits), Memory);

  }
  else
  {
    Result = U64ToCountedString(Memory, (u64)Value, Base, Digits);
  }

  return Result;
}

// Note(Jesse): Shamelessly copied, then modified, from the Handmade Hero codebase
function counted_string
F64ToCountedString(memory_arena* Memory, r64 Value, u32 Precision = 16)
{
  TIMED_FUNCTION();
  // TODO(Jesse): This is over-allocating, and we should consider not doing this
  // in the future!
  char* StartAlloc = AllocateProtection(char, Memory, MaxResultLength, False);
  u32 Count = 0;

  if(Value < 0)
  {
      CopyToDest(StartAlloc, &Count, '-');
      Value = -Value;
  }

  u64 IntegerPart = (u64)Value;
  Value -= (r64)IntegerPart;
  counted_string IntString = U64ToCountedString(Memory, IntegerPart);

  MemCopy((u8*)IntString.Start+Count, (u8*)StartAlloc, IntString.Count);
  Count += IntString.Count;

  CopyToDest(StartAlloc, &Count, '.');

  // TODO(casey): Note that this is NOT an accurate way to do this!
  for(u32 PrecisionIndex = 0;
      PrecisionIndex < Precision;
      ++PrecisionIndex)
  {
      Value *= 10.0f;
      u32 Int = (u32)Value;
      Value -= (r32)Int;
      CopyToDest(StartAlloc, &Count, DecChars[Int]);
  }

  counted_string Result = { .Start = StartAlloc, .Count = Count };
  return Result;
}

#define STRING_BUFFER_LENGTH 2048

char *
FormatString(memory_arena *Memory, const char* FormatString, ...)
{
  char *Buffer = AllocateProtection(char, Memory, STRING_BUFFER_LENGTH, False);

  va_list Arguments;
  va_start(Arguments, FormatString);
  vsnprintf(Buffer, STRING_BUFFER_LENGTH-1, FormatString, Arguments);
  va_end(Arguments);

  return Buffer;
}

#if 1

counted_string
FormatCountedString(memory_arena *Memory, const char* FormatString, ...)
{
  TIMED_FUNCTION();

  char *Buffer = AllocateProtection(char, Memory, STRING_BUFFER_LENGTH, False);

  va_list Arguments;
  va_start(Arguments, FormatString);
  vsnprintf(Buffer, STRING_BUFFER_LENGTH-1, FormatString, Arguments);
  va_end(Arguments);

  counted_string Result = CountedString(Buffer);
  return Result;
}

#else

// This is to silence the warnings when passing counted_strings to this function
#define FormatCountedString(Memory, Fmt, ...)             \
  _Pragma("clang diagnostic push")                        \
  _Pragma("clang diagnostic ignored \"-Wclass-varargs\"") \
  FormatCountedString_(Memory, Fmt, __VA_ARGS__)          \
  _Pragma("clang diagnostic pop")


function counted_string
FormatCountedString_(memory_arena* Memory, const char* fmt...)
{
  TIMED_FUNCTION();

  string_builder Builder_ = {};
  string_builder* Builder = &Builder_;

  va_list args;
  TIMED_BLOCK("va_start");
  va_start(args, fmt);
  END_BLOCK();

  const char* StartPreceedingChars = fmt;
  const char* OnePastLast = fmt;
  while (*fmt != '\0')
  {
    debug_timed_function OuterLoopTimer("OuterLoop");

    if (*fmt == '%')
    {
      debug_timed_function FmtSpecifierTimer("Format Specifier Block");

      umm NumPreceedingChars = (umm)(OnePastLast - StartPreceedingChars);
      counted_string PreceedingNonFormatSpecifiers = CS(StartPreceedingChars, NumPreceedingChars);
      Append(Builder, PreceedingNonFormatSpecifiers);
      ++fmt;

      switch (*fmt)
      {
        case 'd':
        {
          TIMED_NAMED_BLOCK("d");
          s32 Value = va_arg(args, s32);
          counted_string S = S64ToCountedString(Memory, (s64)Value);
          Append(Builder, S);
        } break;

        case 'l':
        {
          TIMED_NAMED_BLOCK("l");
          ++fmt;
          if (*fmt == 'u')
          {
            u64 Value = va_arg(args, u64);
            counted_string S = U64ToCountedString(Memory, Value);
            Append(Builder, S);
          }
          else if (*fmt == 'd')
          {
            s64 Value = va_arg(args, s64);
            counted_string S = S64ToCountedString(Memory, Value);
            Append(Builder, S);
          }
        } break;

        case 'x':
        {
          TIMED_NAMED_BLOCK("x");
          u64 Value = va_arg(args, u64);
          counted_string S = U64ToCountedString(Memory, Value);
          Append(Builder, S);
        } break;

        case 'u':
        {
          TIMED_NAMED_BLOCK("u");
          u32 Value = va_arg(args, u32);
          counted_string S = U64ToCountedString(Memory, (u64)Value);
          Append(Builder, S);
        } break;

        case 'c':
        {
          TIMED_NAMED_BLOCK("c");
          char Value = (char)va_arg(args, s32);
          Append(Builder, CS(&Value, 1));
        } break;

        case 's':
        {
          TIMED_NAMED_BLOCK("s");
          char* Value = va_arg(args, char*);
          counted_string CSValue = CountedString(Value, Memory);
          Append(Builder, CSValue);
        } break;

        case 'f':
        {
          TIMED_NAMED_BLOCK("f");
          r64 Value = va_arg(args, r64);
          counted_string S = F64ToCountedString(Memory, Value);
          Append(Builder, S);
        } break;

        case 'b':
        {
          TIMED_NAMED_BLOCK("b");
          b32 BoolVal = (b32)va_arg(args, u32);
          counted_string Output = {};
          Output = BoolVal ? CS("True") : CS("False");
          Append(Builder, Output);
        } break;

        case 'S':
        {
          TIMED_NAMED_BLOCK("S");
          u32 Count = va_arg(args, u32);
          char* Start = va_arg(args, char*);
          counted_string Output = { .Start = Start, .Count = Count };
          Append(Builder, Output);
        } break;

        case '.':
        {
          TIMED_NAMED_BLOCK(".");
          Assert(*(++fmt) == '*')
          Assert(*(++fmt) == 's')
          u32 Count = va_arg(args, u32);
          char* Start = va_arg(args, char*);
          counted_string Output = { .Start = Start, .Count = Count };
          Append(Builder, Output);
        } break;

        default:
        {
          TIMED_NAMED_BLOCK("default");
          va_arg(args, void*);
          Error("Invalid Format String");
        } break;

      }

      ++fmt;
      StartPreceedingChars = fmt;
    }
    else
    {
      ++fmt;
    }

    OnePastLast = fmt;
  }

  TIMED_BLOCK("Final String Stuff");
    umm NumPreceedingChars = (umm)(OnePastLast - StartPreceedingChars);
    counted_string PreceedingNonFormatSpecifiers = CS(StartPreceedingChars, NumPreceedingChars);
    Append(Builder, PreceedingNonFormatSpecifiers);
  END_BLOCK();

  counted_string Result = Finalize(Builder, Memory);

  TIMED_BLOCK("va_end");
  va_end(args);
  END_BLOCK();

  return Result;
}

#endif