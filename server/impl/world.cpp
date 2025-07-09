
internal bool8
spawn_food(v2f pos)
{
  for (u32 i = 0; i < FOOD_MAX; i++)
  {
    if (!food_alive[i])
    {
      temp[i] = {
        (s64)(pos._x - FOOD_SIZE*0.5f), 
        (s64)(pos._y + FOOD_SIZE*0.5f),
        (s64)(pos._x + FOOD_SIZE*0.5f),
        (s64)(pos._y - FOOD_SIZE*0.5f)
      };
      fprintf(stdout, "FF %d %d %d %d\n", temp[i].left, temp[i].right,
          temp[i].top, temp[i].bottom);
      food_alive[i] = TRUE;
      food[i] = pos;
      food_count++;
      return (TRUE);
    }
  }
  return (FALSE);
}
