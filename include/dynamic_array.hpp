#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <stdexcept>

template <class T> class dynamic_array {
public:
  class iterator;

  dynamic_array();
  dynamic_array(size_t size);
  dynamic_array(T *items_ptr, size_t size);
  dynamic_array(const dynamic_array<T> &other);
  dynamic_array(std::initializer_list<T>);

  ~dynamic_array();

  dynamic_array<T> &operator=(const dynamic_array<T> &other);
  const T &operator[](size_t index) const;
  T &operator[](size_t index);

  const T &get(size_t index) const;
  size_t get_size();

  void set(size_t index, const T &value);

  iterator begin() const;
  iterator end() const;

  void resize(size_t new_size);

private:
  T *items;
  size_t size;
};

template <class T>
dynamic_array<T>::dynamic_array() : items(nullptr), size(0) {}

template <class T>
dynamic_array<T>::dynamic_array(size_t size_)
    : items(new T[size_]), size(size_) {}

template <class T>
dynamic_array<T>::dynamic_array(T *items_, size_t size_)
    : items(new T[size_]), size(size_) {
  std::copy(items_, items_ + size, items);
}

template <class T>
dynamic_array<T>::dynamic_array(const dynamic_array<T> &other)
    : items(new T[other.size]), size(other.size) {
  std::copy(other.items, other.items + size, items);
}
template <class T>
dynamic_array<T>::dynamic_array(std::initializer_list<T> init_list)
    : items(new T[init_list.size()]), size(init_list.size()) {
  std::copy(std::begin(init_list), std::end(init_list), items);
};

template <class T> dynamic_array<T>::~dynamic_array() { delete[] items; }

template <class T>
dynamic_array<T> &dynamic_array<T>::operator=(const dynamic_array<T> &other) {
  if (this == &other)
    return *this;

  auto new_items = new T[other.size];
  std::copy(other.items, other.items + other.size, new_items);

  delete[] items;
  items = new_items;
  size = other.size;
  return *this;
}

template <class T> const T &dynamic_array<T>::operator[](size_t index) const {
  if (index >= size)
    throw std::out_of_range("index out of range");
  return items[index];
}
template <class T> T &dynamic_array<T>::operator[](size_t index) {
  if (index >= size)
    throw std::out_of_range("index out of range");
  return items[index];
}

template <class T> const T &dynamic_array<T>::get(size_t index) const {
  if (index >= size)
    throw std::out_of_range("index out of range");
  return items[index];
}

template <class T> size_t dynamic_array<T>::get_size() { return size; }

template <class T> void dynamic_array<T>::set(size_t index, const T &value) {
  if (index >= size)
    throw std::out_of_range("index out of range");
  items[index] = value;
}

template <class T> void dynamic_array<T>::resize(size_t new_size) {
  if (new_size == size)
    return;

  auto new_items = new T[new_size];
  std::copy(items, items + std::min({size, new_size}), new_items);

  delete[] items;
  items = new_items;
  size = new_size;
}

template <class T> class dynamic_array<T>::iterator {
public:
  iterator(T *ptr_);
  iterator &operator++();
  T &operator*();
  bool operator!=(const iterator &other);

private:
  T *ptr;
};

template <class T> dynamic_array<T>::iterator::iterator(T *ptr_) : ptr(ptr_) {};
template <class T>
typename dynamic_array<T>::iterator &dynamic_array<T>::iterator::operator++() {
  ++ptr;
  return *this;
};
template <class T> T &dynamic_array<T>::iterator::operator*() { return *ptr; };
template <class T>
bool dynamic_array<T>::iterator::operator!=(
    const dynamic_array<T>::iterator &other) {
  return ptr != other.ptr;
};

template <class T>
typename dynamic_array<T>::iterator dynamic_array<T>::begin() const {
  return iterator(items);
}
template <class T>
typename dynamic_array<T>::iterator dynamic_array<T>::end() const {
  return iterator(items + size);
}
