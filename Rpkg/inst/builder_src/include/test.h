#ifndef TEST_H
#define TEST_H

struct Tests_t {
  char *description;
  char *expressions;
  struct Tests_t *next;
};

typedef struct Tests_t Tests;

typedef struct {
    Tests *tests;
    char *description;
    char *expressions;
    int in_test;
} TestCollector;

int enter_test(char *line);
// Returns 1 if line was consumed (part of test block), 0 otherwise
int collect_test_line(TestCollector *collector, char *line);
Tests *create_test(char *description, char *expressions);
void push_test(Tests **tests, Tests *test);
void free_test(Tests *test);
void write_tests(Tests *tests, const char *src);

#endif
