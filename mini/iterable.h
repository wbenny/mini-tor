#pragma once

namespace mini {

template <typename T>
class iterable
{
  public:
    virtual ~iterable() {}

    virtual T* begin() = 0;
    virtual const T* begin() const = 0;

    virtual T* end() = 0;
    virtual const T* end() const = 0;
};

template <typename T>
class randomly_accessible
{
  public:
    virtual ~randomly_accessible() {}

    virtual T& operator[](size_t index) = 0;
    virtual const T& operator[](size_t index) const = 0;

    virtual T& at(size_t index) = 0;
    virtual const T& at(size_t index) const = 0;
};

template <typename T>
class appendable
{
  public:
    virtual ~appendable() {}

    virtual void add(const T& item) = 0;
};

template <typename T>
class container
  : public iterable<T>
  , public randomly_accessible<T>
  , public appendable<T>
{

};

}

