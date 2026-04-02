#include <algorithm>
#include <cstddef>
#include <cstdlib>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <utility>

// NOTE(coverage): матрица «реализация ↔ linked_list_test.cpp»; T = int, float.
//   Покрыто: default/(T*,n)/copy ctor, operator=/+/+= (вкл. self),
//   append/prepend/ insert_at, get/get_first/get_last, get_sub_list,
//   исключения, begin/end, member operator== через .operator==({…}) в CHECK,
//   get_node косвенно.
// TODO(coverage): создание списка конструктором linked_list(initializer_list) —
//   в тестах почти везде (T*, size); member operator<<(ostream) не вызывается
//   (нет std::cout/stringstream с list).
// NOTE(coverage): clear() только из operator=; отдельного теста на clear нет.
template <class T> class linked_list {
public:
  class iterator;
  struct node;

  linked_list();
  linked_list(T *items, size_t size);
  linked_list(const linked_list<T> &other);
  linked_list(std::initializer_list<T> init_list);

  ~linked_list() = default;

  linked_list<T> &operator=(const linked_list<T> &other);
  linked_list<T> operator+(const linked_list<T> &other) const;
  linked_list<T> &operator+=(const linked_list<T> &other);
  bool operator==(std::initializer_list<T> arr);
  std::ostream &operator<<(std::ostream &os) const;

  T get_first();
  T get_last();
  T get(size_t index);
  size_t get_size();

  iterator begin() const;
  iterator end() const;

  template <class U> void append(U &&item);
  template <class U> void prepend(U &&item);
  template <class U> void insert_at(size_t index, U &&item);
  void clear();

  linked_list<T> get_sub_list(size_t start_index, size_t end_index) const;

private:
  std::shared_ptr<node> head;
  std::weak_ptr<node> tail;
  size_t size;

  std::shared_ptr<node> get_node(size_t index);
};

template <class T>
typename linked_list<T>::iterator linked_list<T>::begin() const {
  return iterator(head);
};
template <class T>
typename linked_list<T>::iterator linked_list<T>::end() const {
  return iterator();
};

template <class T> linked_list<T>::linked_list() : size(0) {};
template <class T>
linked_list<T>::linked_list(T *items, size_t size_) : size(0) {
  for (; size_ > 0; --size_) {
    prepend(items[size_ - 1]);
  }
};
template <class T>
linked_list<T>::linked_list(const linked_list<T> &other) : size(0) {
  *this = other;
};
template <class T>
linked_list<T>::linked_list(std::initializer_list<T> init_list) : size(0) {
  // TODO(coverage): тело ctor вызывается только при linked_list<T>{…}; в тестах
  //   список обычно собирают через (T*, n) — см. шапку файла.
  for (auto el : init_list) {
    append(el);
  }
};

template <class T> template <class U> void linked_list<T>::append(U &&item) {
  auto new_node = std::make_shared<node>(std::forward<U>(item));
  if (size == 0) {
    head = new_node;
    tail = new_node;
  } else {
    auto temp = tail.lock();
    new_node->prev = temp;
    temp->next = new_node;
    tail = new_node;
  }
  ++size;
}
template <class T> template <class U> void linked_list<T>::prepend(U &&item) {
  auto new_node = std::make_shared<node>(std::forward<U>(item));
  if (size == 0) {
    head = new_node;
    tail = new_node;
  } else {
    head->prev = new_node;
    new_node->next = head;
    head = new_node;
  }
  ++size;
};
template <class T>
template <class U>
void linked_list<T>::insert_at(size_t index, U &&item) {
  if (index > size)
    throw std::out_of_range("index out of range");
  if (index == 0) {
    prepend(item);
    return;
  }
  if (index == size) {
    append(item);
    return;
  }

  auto new_node = std::make_shared<node>(std::forward<U>(item));
  auto behind = get_node(index - 1);

  new_node->next = behind->next;
  new_node->prev = behind;

  behind->next->prev = new_node;
  behind->next = new_node;
  ++size;
};
template <class T> void linked_list<T>::clear() {
  // NOTE(coverage): прямых вызовов в тестах нет; путь выполняется из operator=.
  head.reset();
  tail.reset();
  size = 0;
}

template <class T> size_t linked_list<T>::get_size() { return size; };
template <class T> T linked_list<T>::get(size_t index) {
  if (index >= size)
    throw std::out_of_range("index out of range");
  return get_node(index)->value;
};
template <class T> T linked_list<T>::get_first() { return head->value; };
template <class T> T linked_list<T>::get_last() { return tail.lock()->value; };
template <class T>
std::shared_ptr<typename linked_list<T>::node>
linked_list<T>::get_node(size_t index) {
  if (index >= linked_list<T>::size)
    throw std::out_of_range("index out of range");
  if (index == 0)
    return linked_list<T>::head;
  typename linked_list<T>::node *cur = linked_list<T>::head.get();
  for (; index > 1; --index)
    cur = cur->next.get();
  return cur->next;
};
template <class T>
linked_list<T> linked_list<T>::get_sub_list(size_t start_index,
                                            size_t end_index) const {
  if (start_index > size || end_index > size || start_index > end_index)
    throw std::out_of_range("index out of range");

  linked_list<T> sub;
  size_t i = 0;
  for (const auto &el : *this) {
    if (i == end_index)
      return sub;
    if (i++ < start_index)
      continue;
    sub.append(el);
  }
  return sub;
};

template <class T>
linked_list<T> &linked_list<T>::operator=(const linked_list<T> &other) {
  // NOTE(coverage): ветка this == &other — SECTION("self-assignment")
  // linked_list opertor=.
  if (this == &other)
    return *this;
  clear();
  for (const auto &el : other) {
    append(el);
  }
  return *this;
}

template <class T>
linked_list<T> linked_list<T>::operator+(const linked_list<T> &other) const {
  linked_list<T> new_list;
  for (auto el1 : *this)
    new_list.append(el1);
  for (auto el2 : other)
    new_list.append(el2);
  return new_list;
}
template <class T>
linked_list<T> &linked_list<T>::operator+=(const linked_list<T> &other) {
  for (auto el : other)
    append(el);
  return *this;
}

template <class T> struct linked_list<T>::node {
  std::shared_ptr<node> next;
  std::weak_ptr<node> prev;
  T value;

  template <class U> node(U &&val) : value(std::forward<U>(val)) {};
};

template <class T> class linked_list<T>::iterator {
private:
  std::weak_ptr<node> ptr;

public:
  iterator();
  iterator(std::shared_ptr<node> ptr_);

  iterator &operator++();
  T &operator*();
  bool operator!=(const iterator &other);
};

template <class T> linked_list<T>::iterator::iterator() : ptr() {};
template <class T>
linked_list<T>::iterator::iterator(std::shared_ptr<node> ptr_) : ptr(ptr_) {};
template <class T>
typename linked_list<T>::iterator &linked_list<T>::iterator::operator++() {
  ptr = ptr.lock()->next;
  return *this;
}
template <class T> T &linked_list<T>::iterator::operator*() {
  return ptr.lock()->value;
}
template <class T>
bool linked_list<T>::iterator::operator!=(const iterator &other) {
  return ptr.lock() != other.ptr.lock();
}

template <class T>
bool linked_list<T>::operator==(std::initializer_list<T> arr) {
  // NOTE(coverage): исполняется из linked_list_test (.operator==({…}) в CHECK).
  if (arr.size() != get_size())
    return false;
  return std::equal(std::begin(arr), std::end(arr), begin());
}

// TODO(coverage): linked_list_test не пишет list в поток — ветки форматирования
//   здесь не гоняются (добавить stringstream или Catch stringification).
template <class T>
std::ostream &linked_list<T>::operator<<(std::ostream &os) const {
  os << "list: {";
  auto it = begin();
  // NOTE: end() — конец диапазона; qualified имя избегает коллизии с локальной
  //   переменной end (если переименовать — можно писать просто end()).
  const auto end = linked_list<T>::end();
  if (it != end) {
    os << *it;
    while (++it != end)
      os << ", " << *it;
  }
  return os << "}\n";
}
