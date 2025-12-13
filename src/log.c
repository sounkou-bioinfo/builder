#include <stdio.h>

void log_info(char *message)
{
  printf("[INFO] %s\n", message);
}

void log_error(char *message)
{
  printf("[ERROR] %s\n", message);
}

void log_warning(char *message)
{
  printf("[WARNING] %s\n", message);
}
