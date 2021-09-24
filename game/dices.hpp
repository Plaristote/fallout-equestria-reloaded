#ifndef  DICES_HPP
# define DICES_HPP

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct Dices
{
  static void Initialize(void)
  {
    srand(time(0));
  }
  
  static int Throw(unsigned int max)
  {
    if (max != 0)
      return ((rand() % max) + 1);
    return max;
  }

  static bool Test(int successPercentage)
  {
    return (Throw(100) <= successPercentage);
  }
};

#endif