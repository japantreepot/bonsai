
link_internal void
UpdateCameraP(world *World, canonical_position NewTarget, camera *Camera)
{
  TIMED_FUNCTION();
  if (Camera->DistanceFromTarget < 0.1f)
  {
    Camera->DistanceFromTarget = 0.1f;
  }

  r32 Px = Sin(Camera->Yaw);
  r32 Py = Cos(Camera->Yaw);
  r32 Pz = Cos(Camera->Pitch);

  Camera->Front = Normalize(V3(Px, Py, Pz));

  Camera->Right = Normalize(Cross(V3(0,0,1), Camera->Front));
  Camera->Up = Normalize(Cross(Camera->Front, Camera->Right));

  Camera->ViewingTarget = NewTarget;

  Camera->TargetP = Canonicalize(World->ChunkDim, NewTarget - (Camera->Front*Camera->DistanceFromTarget));
  Camera->CurrentP = Lerp(0.25f, Camera->CurrentP, Camera->TargetP, World->ChunkDim);
  /* Camera->CurrentP = Canonicalize(World->ChunkDim, Camera->CurrentP); */

  Camera->RenderSpacePosition = GetRenderP(World->ChunkDim, Camera->CurrentP, Camera);

#if 1

  //
  // Frustum computation
  //
  v3 FrustLength = V3(0.0f, 0.0f, Camera->Frust.farClip);
  v3 FarHeight = ( V3( 0.0f, ((Camera->Frust.farClip - Camera->Frust.nearClip)/Cos(Camera->Frust.FOV/2.0f)) * Sin(Camera->Frust.FOV/2.0f), 0.0f));
  v3 FarWidth = V3( FarHeight.y, 0.0f, 0.0f);

  v3 MaxMax = Normalize(FrustLength + FarHeight + FarWidth);
  v3 MaxMin = Normalize(FrustLength + FarHeight - FarWidth);
  v3 MinMax = Normalize(FrustLength - FarHeight + FarWidth);
  v3 MinMin = Normalize(FrustLength - FarHeight - FarWidth);

  v3 Front = V3(0,0,1);
  v3 Target = Camera->Front;

  Quaternion GrossRotation = RotatePoint(Front, Target);

  // We've got to correct the rotation so it ends pointing the frustum in the cameras 'up' direction
  v3 UpVec = V3(0, 1, 0);
  /* v3 UpVec = Camera->Up; */
  v3 RotatedUp = Rotate(UpVec, GrossRotation);
  Quaternion DesiredUp = RotatePoint(RotatedUp, Camera->Up);

  Quaternion FinalRotation = DesiredUp * GrossRotation;

  /* MaxMin = Normalize(Rotate(MaxMin, GrossRotation)); */
  /* MaxMax = Normalize(Rotate(MaxMax, GrossRotation)); */
  /* MinMin = Normalize(Rotate(MinMin, GrossRotation)); */
  /* MinMax = Normalize(Rotate(MinMax, GrossRotation)); */

  MaxMin = Normalize(Rotate(MaxMin, FinalRotation));
  MaxMax = Normalize(Rotate(MaxMax, FinalRotation));
  MinMin = Normalize(Rotate(MinMin, FinalRotation));
  MinMax = Normalize(Rotate(MinMax, FinalRotation));

#if DEBUG_DRAW_FRUSTUM_AT_ORIGIN
  {
    auto Resources = GetEngineResources();
    auto *GpuBuffer = &GetCurrentGpuMap(Resources->Graphics)->Buffer;
    auto Dest = ReserveBufferSpace(GpuBuffer, VERTS_PER_LINE*4);
    DEBUG_DrawLine(&Dest, line(V3(0), MaxMax*40), RED, 0.2f );
    DEBUG_DrawLine(&Dest, line(V3(0), MaxMin*40), BLUE, 0.2f );
    DEBUG_DrawLine(&Dest, line(V3(0), MinMax*40), GREEN, 0.2f );
    DEBUG_DrawLine(&Dest, line(V3(0), MinMin*40), YELLOW, 0.2f );
  }
#endif

  v3 CameraSimP = GetSimSpaceP(World, Camera->CurrentP);

  plane Top(CameraSimP,   Normalize(Cross(MaxMax, MaxMin)));
  plane Bot(CameraSimP,   Normalize(Cross(MinMin, MinMax)));
  plane Left(CameraSimP,  Normalize(Cross(MinMax, MaxMax)));
  plane Right(CameraSimP, Normalize(Cross(MaxMin, MinMin)));

  Camera->Frust.Top = Top;
  Camera->Frust.Bot = Bot;
  Camera->Frust.Left = Left;
  Camera->Frust.Right = Right;

#endif

  // TODO(Jesse, id: 128, tags: correctness, robustness, speed): Do culling on these as well?
  /* plane Near; */
  /* plane Far; */


  return;
}

inline v2
GetMouseDelta(platform *Plat)
{
  r32 DPToWorldModifier = -0.001f;
  v2 Result = Plat->MouseDP * DPToWorldModifier;
  return Result;
}

link_internal void
UpdateGameCamera(world *World, v2 MouseDelta, input *Input, canonical_position NewTarget, camera* Camera)
{

  if (Input)
  {
    if (Input->LMB.Pressed)
    {
      Camera->Yaw += MouseDelta.x;
      Camera->Pitch += MouseDelta.y;
      Camera->Pitch = ClampBetween(0.0, Camera->Pitch, PI32);
    }

    if (Input->RMB.Pressed)
    {
      Camera->DistanceFromTarget += MouseDelta.y*Camera->DistanceFromTarget;
    }
  }

  UpdateCameraP(World, NewTarget, Camera);
  return;
}

inline bool
IsInFrustum(world *World, camera *Camera, canonical_position P)
{
  bool Result = true;

  v3 TestP = GetSimSpaceP(World, P);

  // This says if we're on the back-side of the plane by more than the dim of a
  // world chunk, we're outside the frustum
  /* Result &= (DistanceToPlane(&Camera->Frust.Top, TestP)   < -1*World->ChunkDim.y); */
  /* Result &= (DistanceToPlane(&Camera->Frust.Bot, TestP)   < -1*World->ChunkDim.y); */
  /* Result &= (DistanceToPlane(&Camera->Frust.Left, TestP)  < -1*World->ChunkDim.x); */
  /* Result &= (DistanceToPlane(&Camera->Frust.Right, TestP) < -1*World->ChunkDim.x); */

  Result &= (DistanceToPlane(&Camera->Frust.Top, TestP)   > 0);
  Result &= (DistanceToPlane(&Camera->Frust.Bot, TestP)   > 0);
  Result &= (DistanceToPlane(&Camera->Frust.Left, TestP)  > 0);
  Result &= (DistanceToPlane(&Camera->Frust.Right, TestP) > 0);

  return Result;
}

inline bool
IsInFrustum( world *World, camera *Camera, world_chunk *Chunk )
{
  v3 ChunkMid = World->ChunkDim/2.0f;
  canonical_position P1 = Canonical_Position(ChunkMid, Chunk->WorldP );
  bool Result = IsInFrustum(World, Camera, P1);
  return Result;
}


