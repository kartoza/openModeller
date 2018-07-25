

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  printf("%s started with %i arguements", argv[0], argc);

  return EXIT_SUCCESS;
}
