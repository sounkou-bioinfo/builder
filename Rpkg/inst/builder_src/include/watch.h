#ifndef WATCH_H
#define WATCH_H

int watch_init(const char *path);
int watch_wait(int fd);
void watch_close(int fd);

#endif
