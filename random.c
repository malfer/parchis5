/* In Mingw there are not random and srandom functions, so */
/* we use rand and srand instead                           */
/*                                                         */

#include <stdlib.h>

int srandom(int seed)
{
  srand(seed);
  return 0;
}

long random(void)
{
  return rand();
}
