#ifndef __MAIN_H_
#define __MAIN_H_

#include <cstddef> // For std::ptrdiff_t
#include <functional>
#include <iterator> // For std::forward_iterator_tag
#include <string_view>
#include <type_traits> // For std:remove_pointer

/*
template <typename T> class CstrWrapper {
public:
  using CharType = typename std::remove_pointer<T>::type;
  using PointerType = CharType *;
  using ReferenceType = CharType &;

  struct ConstantIterator {
    using iterator_category = std::forward_iterator_tag;
    using difference_type = std::ptrdiff_t;
    using value_type = CharType;
    using pointer = PointerType;     // or also value_type*
    using reference = ReferenceType; // or also value_type&

    // constructor
    ConstantIterator(pointer ptr) : m_ptr(ptr) {}

    const pointer operator->() const { return m_ptr; }
    const reference operator*() const { return *m_ptr; }

    // Prefix increment
    ConstantIterator &operator++() {
      m_ptr++;
      return *this;
    }

    // Postfix increment
    ConstantIterator operator++(int) {
      ConstantIterator tmp = *this;
      ++(*this);
      return tmp;
    }

    friend bool operator==(const ConstantIterator &a,
                           const ConstantIterator &b) {
      return a.m_ptr == b.m_ptr;
    };
    friend bool operator!=(const ConstantIterator &a,
                           const ConstantIterator &b) {
      return a.m_ptr != b.m_ptr;
    };
    pointer m_ptr;
  };

public:
  CstrWrapper(PointerType _p, int _len) : m_p(_p), m_len(_len > 0 ? _len : 0) {}
  size_t length() const { return m_len; }
  PointerType data() const { return m_p; }

  ConstantIterator begin() const { return ConstantIterator(&m_p[0]); }
  ConstantIterator end() const { return ConstantIterator(&m_p[m_len]); }
  ConstantIterator cbegin() const { return ConstantIterator(&m_p[0]); }
  ConstantIterator cend() const { return ConstantIterator(&m_p[m_len]); }

private:
  PointerType m_p;
  size_t m_len;
};
*/
#endif // !__MAIN_H_