#ifndef __LIST__H__
#define __LIST__H__

#include <stddef.h>

#ifndef container_of
#define container_of(ptr, type, member)                                        \
  ({                                                                           \
    const typeof(((type *)0)->member) *__mptr = (ptr);                         \
    (type *)((char *)__mptr - offsetof(type, member));                         \
  })
#endif

#define LIST_INIT(list) { &(list), &(list) }

struct list_head {
  struct list_head *prev;
  struct list_head *next;
};

static inline void list_init(struct list_head *list) {
  list->prev = list->next = list;
}

static inline void list_add(struct list_head *list, struct list_head *item) {
  struct list_head *prev = list->prev;

  item->next = list;
  item->prev = prev;

  prev->next = list->prev = item;
}

static inline void list_del(struct list_head *item) {
  item->prev->next = item->next;
  item->next->prev = item->prev;
}

#define list_entry_first(list, type, member)                                   \
  container_of((list)->next, type, member)

#define list_entry_next(item, member)                                          \
  container_of((item)->member.next, typeof(*(item)), member)

#define list_for_each_entry_safe(item, next, list, member)                     \
  for (item = list_entry_first(list, typeof(*(item)), member),                 \
      next = list_entry_next(item, member);                                    \
       &item->member != list;                                                  \
       item = next, next = list_entry_next(item, member))

#endif
