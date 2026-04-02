#include <cstddef>
#include <cstdlib>
#include <initializer_list>
#include <memory>
#include <stdexcept>
#include <utility>

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
  for (const auto &el : other) {
    append(el);
  }
};
template <class T>
linked_list<T>::linked_list(std::initializer_list<T> init_list) : size(0) {
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
