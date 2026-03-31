#include <cstddef>
#include <algorithm>
#include <stdexcept>

template <class T>
class dynamic_array {
    private:
        T* items;
        size_t size;
    public:
        dynamic_array();
        dynamic_array(size_t size);
        dynamic_array(T* items_ptr, size_t size);
        dynamic_array(const dynamic_array<T>& other);

        ~dynamic_array();

        dynamic_array<T>& operator=(const dynamic_array<T>& other);


        T get(size_t index);
        size_t get_size();

        void set(size_t index, const T& value);

        
        void resize(size_t new_size);
};

template <class T>
dynamic_array<T>::dynamic_array() : items(nullptr), size(0) {}

template <class T>
dynamic_array<T>::dynamic_array(size_t size_) : items(new T[size_]), size(size_) {}

template <class T>
dynamic_array<T>::dynamic_array(T* items_, size_t size_) : items(new T[size_]), size(size_) {
    std::copy(items_, items_ + size, items);
}

template <class T>
dynamic_array<T>::dynamic_array(const dynamic_array<T>& other) : items(new T[other.size]), size(other.size) {
    std::copy(other.items, other.items + size, items);
}


template <class T>
dynamic_array<T>::~dynamic_array() {
    delete [] items;
}


template <class T>
dynamic_array<T>& dynamic_array<T>::operator=(const dynamic_array<T>& other){
    if (this == &other) return *this;
    
    auto new_items = new T[other.size];
    std::copy(other.items, other.items + other.size, new_items);
    
    delete[] items;
    items = new_items;
    size = other.size;
    return *this;
}


template <class T>
T dynamic_array<T>::get(size_t index) {
    if(index >= size) throw std::out_of_range("index out of range");
    return items[index];
}

template <class T>
size_t dynamic_array<T>::get_size() {
    return size;    
}

template<class T>
void dynamic_array<T>::set(size_t index, const T& value){
    if(index >= size) throw std::out_of_range("index out of range");
    items[index] = value;
}

template<class T>
void dynamic_array<T>::resize(size_t new_size){
    if(new_size == size) return;

    auto new_items = new T[new_size];
    std::copy(items, items + std::min({size, new_size}), new_items);

    delete[] items;
    items = new_items;
    size = new_size;
}