#pragma once
#include <iterator>
#include <utility>   // std::move

#pragma warning(push)
#pragma warning(disable: 4458)

//
// Basic implementation.
//

struct linked_list_item
{
  void add_first(linked_list_item* item)
  {
    item->head = this->head;
    item->tail = this;

    this->head->tail = item;
    this->head = item;
  }

  void add_last(linked_list_item* item)
  {
    item->head = this;
    item->tail = this->tail;

    this->tail->head = item;
    this->tail = item;
  }

  linked_list_item* remove_first()
  {
    linked_list_item* item = this->head;

    this->head = item->head;
    item->head->tail = this;

    return item;
  }

  linked_list_item* remove_last()
  {
    linked_list_item* item = this->tail;

    this->tail = item->tail;
    item->tail->head = this;

    return item;
  }

  bool remove()
  {
    auto head = this->head;
    auto tail = this->tail;

    tail->head = head;
    head->tail = tail;

    return head == tail;
  }

  linked_list_item* head;
  linked_list_item* tail;
};

struct linked_list_head : linked_list_item
{
  linked_list_head()
  {
    clear();
  }

  int get_size() const
  {
    int result = 0;

    linked_list_item* item = head;
    while (item->head != head)
    {
      ++result;

      item = item->head;
    }

    return result;
  }

  void clear()
  {
    this->head = this;
    this->tail = this;
  }

  bool empty() const
  {
    return head == this;
  }
};

//
// Templated implementation.
//

template <
  typename T
>
struct linked_list_item_wrapper : linked_list_item
{
  linked_list_item_wrapper() = default;
  linked_list_item_wrapper(const T& value) : value(value) { }
  linked_list_item_wrapper(T&& value) : value(std::move(value)) { }

  T value;
};

template <
  typename T
>
struct linked_list : linked_list_head
{
  struct iterator
  {
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = T*;
    using reference         = T&;
    using iterator_category = std::bidirectional_iterator_tag;

    iterator& operator++(   )                          {                   item = (linked_list_item_wrapper<T>*)item->head; return *this; }
    iterator  operator++(int)                          { auto tmp = *this; item = (linked_list_item_wrapper<T>*)item->head; return tmp;   }
    iterator& operator--(   )                          {                   item = (linked_list_item_wrapper<T>*)item->tail; return *this; }
    iterator  operator--(int)                          { auto tmp = *this; item = (linked_list_item_wrapper<T>*)item->tail; return tmp;   }

    bool operator==(const iterator& other)       const { return item == other.item; }
    bool operator!=(const iterator& other)       const { return item != other.item; }

    reference operator*()                              { return  item->value; }
    pointer   operator->()                             { return &item->value; }

    linked_list_item_wrapper<T>* item;
  };

  struct const_iterator
  {
    using value_type        = T;
    using difference_type   = std::ptrdiff_t;
    using pointer           = const T*;
    using reference         = const T&;
    using iterator_category = std::bidirectional_iterator_tag;

    const_iterator& operator++()                       { item = (linked_list_item_wrapper<T>*)item->head; return *this; }
    const_iterator  operator++(int)                    { auto tmp = *this; item = (linked_list_item_wrapper<T>*)item->head; return tmp; }
    const_iterator& operator--()                       { item = (linked_list_item_wrapper<T>*)item->tail; return *this; }
    const_iterator  operator--(int)                    { auto tmp = *this; item = (linked_list_item_wrapper<T>*)item->tail; return tmp; }

    bool operator==(const const_iterator& other) const { return item == other.item; }
    bool operator!=(const const_iterator& other) const { return item != other.item; }

    reference operator*()                              { return  item->value; }
    pointer   operator->()                             { return &item->value; }

    const linked_list_item_wrapper<T>* item;
  };

  using item_type         = linked_list_item_wrapper<T>;

  linked_list() = default;
  ~linked_list() { clear(); }

  iterator begin() { return iterator{ (item_type*)head }; }
  iterator end()   { return iterator{ (item_type*)this }; }

  void clear()
  {
    while (!empty())
    {
      remove_last();
    }
  }

  void add_first(const T& value)
  {
    linked_list_head::add_first(new item_type(value));
  }

  void add_first(T&& value)
  {
    linked_list_head::add_first(new item_type(std::move(value)));
  }

  void add_last(const T& value)
  {
    linked_list_head::add_last(new item_type(value));
  }

  void add_last(T&& value)
  {
    linked_list_head::add_last(new item_type(std::move(value)));
  }

  void remove_first()
  {
    delete linked_list_head::remove_first();
  }

  void remove_last()
  {
    delete linked_list_head::remove_last();
  }

  T* get_first()
  {
    return &((item_type*)this->head)->value;
  }

  T* get_last()
  {
    return &((item_type*)this->tail)->value;
  }

  iterator remove(iterator it)
  {
    it.item->remove();
    delete it.item;

    return ++it;
  }

  int remove(const T& value)
  {
    return remove_if([&value](const T& other) { return value == other; });
  }

  template <class UnaryPredicate>
  int remove_if(UnaryPredicate p)
  {
    int result = 0;

    auto it = begin();
    while (it != end())
    {
      if (p(*it))
      {
        it = remove(it);
        ++result;
      }
      else
      {
        ++it;
      }
    }

    return result;
  }
};

#pragma warning(pop)
