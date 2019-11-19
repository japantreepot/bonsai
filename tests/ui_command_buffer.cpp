#define BONSAI_NO_PUSH_METADATA
#define BONSAI_NO_DEBUG_MEMORY_ALLOCATOR
#define PLATFORM_LIBRARY_AND_WINDOW_IMPLEMENTATIONS 1


#include <debug.cpp>
#include <test_utils.cpp>


function void
TestTable(debug_ui_render_group* Group, window_layout* Window)
{
  PushWindowStart(Group, Window);

  PushTableStart(Group);
  for (u32 Index = 0;
      Index < 32;
      ++Index)
  {
    interactable_handle Interaction = PushButtonStart(Group, (umm)"TestButtonInteraction"^(umm)Index);
      PushColumn(Group, CS("String1"));
      PushColumn(Group, CS("String2"));
      PushColumn(Group, CS("String3"));
      PushColumn(Group, CS("String4"));
      PushColumn(Group, CS("String5"));
      PushColumn(Group, CS("String6"));
      PushColumn(Group, CS("String7"));
      PushColumn(Group, CS("String8"));
      PushNewRow(Group);
    PushButtonEnd(Group);

    TestThat(!Hover(Group, &Interaction));
    TestThat(!Clicked(Group, &Interaction));
    TestThat(!Pressed(Group, &Interaction));
  }
  PushTableEnd(Group);

  PushWindowEnd(Group, Window);

  //
  //

  FlushCommandBuffer(Group, Group->CommandBuffer);
}

s32 main()
{
  TestSuiteBegin("ui_command_buffer");

  memory_arena Arena           = {};
  debug_ui_render_group* Group = Allocate(debug_ui_render_group, &Arena, 1);
  Group->CommandBuffer         = Allocate(ui_render_command_buffer, &Arena, 1);
  Group->Input                 = Allocate(input, &Arena, 1);;
  Group->MouseP                = Allocate(v2, &Arena, 1);;

  local_persist window_layout Window = WindowLayout("TestWindow", V2(0));

  TestTable(Group, &Window);
  TestTable(Group, 0);

  TestSuiteEnd();
}
