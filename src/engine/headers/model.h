
enum model_index
{
  ModelIndex_None,

  ModelIndex_Enemy_Skeleton_Axe,
  ModelIndex_Enemy_Skeleton_Sword,
  ModelIndex_Enemy_Skeleton_Lasher,
  ModelIndex_Enemy_Skeleton_Archer,
  ModelIndex_Enemy_Skeleton_Spear,
  ModelIndex_Enemy_Skeleton_AxeArmor,
  ModelIndex_Enemy_Skeleton_Hounds,
  ModelIndex_Enemy_Skeleton_Horserider,
  ModelIndex_Enemy_Skeleton_Horsebanner,
  ModelIndex_Enemy_Skeleton_Shaman,
  ModelIndex_Enemy_Skeleton_Champion,
  ModelIndex_Enemy_Skeleton_ChampionChampion,
  ModelIndex_Enemy_Skeleton_Concubiner,
  ModelIndex_Enemy_Skeleton_King,

  ModelIndex_FirstEnemyModel = ModelIndex_Enemy_Skeleton_Axe,
  ModelIndex_LastEnemyModel = ModelIndex_Enemy_Skeleton_King,

  ModelIndex_Player_jp,
  ModelIndex_Player_bow,
  ModelIndex_Player_cat,
  ModelIndex_Player_fox,
  ModelIndex_Player_gumi,
  ModelIndex_Player_knight,
  ModelIndex_Player_man,
  ModelIndex_Player_mom,
  ModelIndex_Player_old,
  ModelIndex_Player_poem,
  ModelIndex_Player_rain,
  ModelIndex_Player_sasami,
  ModelIndex_Player_sol,
  ModelIndex_Player_sword,
  ModelIndex_Player_tale,
  ModelIndex_Player_tama,
  ModelIndex_Player_tsurugi,

  ModelIndex_FirstPlayerModel = ModelIndex_Player_jp,
  ModelIndex_LastPlayerModel = ModelIndex_Player_tsurugi,

  ModelIndex_Loot,
  ModelIndex_Projectile,
  ModelIndex_Proton,
  ModelIndex_Bitty0,
  ModelIndex_Bitty1,

  ModelIndex_Level,

  ModelIndex_Count,
};
poof(string_and_value_tables(model_index))
#include <generated/string_and_value_tables_model_index.h>


struct model
{
  untextured_3d_geometry_buffer Mesh;
  chunk_dimension Dim;
  animation Animation;

  /* v4 *Palette; // Optional */
};


