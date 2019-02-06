
struct rect2
{
  v2 Min;
  v2 Max;
};

rect2
InvertedInfinityRectangle()
{
  rect2 Result = {};
  Result.Min = V2(FLT_MAX);
  Result.Max = V2(-FLT_MAX);

  return Result;
}

rect2
RectMinMax(v2 Min, v2 Max)
{
  rect2 Result = {Min, Max};
  return Result;
}

rect2
RectMinDim(v2 Min, v2 Dim)
{
  rect2 Result = {Min, Min+Dim};
  return Result;
}

struct aabb
{
  v3 Center;
  v3 Radius;

  aabb(v3 Center_in, v3 Radius_in)
  {
    this->Center = Center_in;
    this->Radius = Radius_in;
  }

  aabb(world_position Center_in, world_position Radius_in)
  {
    this->Center = V3(Center_in);
    this->Radius = V3(Radius_in);
  }

  aabb() { Clear(this); }
};

inline aabb
MinMaxAABB(v3 Min, v3 Max)
{
  v3 Radius = (Max - Min)/2.0f;
  v3 Center = Min + Radius;
  aabb Result(Center, Radius);
  return Result;
}

inline aabb
operator+(aabb AABB, v3 V)
{
  aabb Result = AABB;
  Result.Radius = AABB.Radius + V;
  return Result;
}

v3
HalfDim( v3 P1 )
{
  v3 Result = P1 / 2;
  return Result;
}

struct ray
{
  v3 Origin;
  v3 Dir;
};

// TODO(Jesse): This could be optimized significantly
b32
Intersect(aabb AABB, ray Ray)
{
  v3 min = AABB.Center - AABB.Radius;;
  v3 max = AABB.Center + AABB.Radius;;

  r32 tmin = (min.x - Ray.Origin.x) / Ray.Dir.x;
  r32 tmax = (max.x - Ray.Origin.x) / Ray.Dir.x;

  if (tmin > tmax)
  {
    r32 temp = tmin;
    tmin = tmax;
    tmax = temp;
  }

  r32 tymin = (min.y - Ray.Origin.y) / Ray.Dir.y;
  r32 tymax = (max.y - Ray.Origin.y) / Ray.Dir.y;

  if (tymin > tymax)
  {
    r32 temp = tymin;
    tymin = tymax;
    tymax = temp;
  }

  if ((tmin > tymax) || (tymin > tmax))
  return false;

  if (tymin > tmin)
  tmin = tymin;

  if (tymax < tmax)
  tmax = tymax;

  r32 tzmin = (min.z - Ray.Origin.z) / Ray.Dir.z;
  r32 tzmax = (max.z - Ray.Origin.z) / Ray.Dir.z;

  if (tzmin > tzmax)
  {
    r32 temp = tzmin;
    tzmin = tzmax;
    tzmax = temp;
  }

  if ((tmin > tzmax) || (tzmin > tmax))
  return false;

  if (tzmin > tmin)
  tmin = tzmin;

  if (tzmax < tmax)
  tmax = tzmax;

  return true;
}

inline b32
IsInsideRect(rect2 Rect, v2 P)
{
  b32 Result = (P > Rect.Min && P < Rect.Max);
  return Result;
}

