#ifndef SJTU_LIST_HPP
#define SJTU_LIST_HPP

#include <cstddef>
#include <type_traits>
#include <utility>

namespace sjtu {
/**
 * a data container like std::list
 * allocate random memory addresses for data and they are doubly-linked in a
 * list.
 */
template <typename T> class list {
protected:
  struct node {
    node *prev;
    node *next;
    typename std::aligned_storage<sizeof(T), alignof(T)>::type storage;
    bool has_value;

    node() : prev(this), next(this), has_value(false) {}
    explicit node(const T &value) : prev(nullptr), next(nullptr), has_value(true) {
      ::new (static_cast<void *>(&storage)) T(value);
    }
    T *ptr() { return reinterpret_cast<T *>(&storage); }
    const T *ptr() const { return reinterpret_cast<const T *>(&storage); }
    ~node() {
      if (has_value) {
        ptr()->~T();
        has_value = false;
      }
    }
  };

protected:
  node sentinel; // circular sentinel; end() == &sentinel
  size_t sz = 0;

  // insert node cur before node pos; return cur
  node *insert(node *pos, node *cur) {
    cur->next = pos;
    cur->prev = pos->prev;
    pos->prev->next = cur;
    pos->prev = cur;
    ++sz;
    return cur;
  }

  // remove node pos from list; return removed node
  node *erase(node *pos) {
    pos->prev->next = pos->next;
    pos->next->prev = pos->prev;
    pos->prev = pos->next = nullptr;
    --sz;
    return pos;
  }

public:
  class const_iterator;
  class iterator {
  private:
    node *cur;
    const list *owner;

    explicit iterator(node *p, const list *o) : cur(p), owner(o) {}
    friend class list;
    friend class const_iterator;

  public:
    iterator() : cur(nullptr), owner(nullptr) {}

    iterator operator++(int) {
      iterator tmp = *this;
      if (cur) cur = cur->next;
      return tmp;
    }
    iterator &operator++() {
      if (cur) cur = cur->next;
      return *this;
    }
    iterator operator--(int) {
      iterator tmp = *this;
      if (cur) cur = cur->prev;
      return tmp;
    }
    iterator &operator--() {
      if (cur) cur = cur->prev;
      return *this;
    }

    T &operator*() const { return *(cur->ptr()); }
    T *operator->() const noexcept { return cur->ptr(); }

    bool operator==(const iterator &rhs) const { return cur == rhs.cur; }
    bool operator==(const const_iterator &rhs) const;

    bool operator!=(const iterator &rhs) const { return cur != rhs.cur; }
    bool operator!=(const const_iterator &rhs) const;
  };

  class const_iterator {
  private:
    const node *cur;
    const list *owner;
    explicit const_iterator(const node *p, const list *o) : cur(p), owner(o) {}
    friend class list;
    friend class iterator;

  public:
    const_iterator() : cur(nullptr), owner(nullptr) {}
    const_iterator(const iterator &it) : cur(it.cur), owner(it.owner) {}

    const_iterator operator++(int) {
      const_iterator tmp = *this;
      if (cur) cur = cur->next;
      return tmp;
    }
    const_iterator &operator++() {
      if (cur) cur = cur->next;
      return *this;
    }
    const_iterator operator--(int) {
      const_iterator tmp = *this;
      if (cur) cur = cur->prev;
      return tmp;
    }
    const_iterator &operator--() {
      if (cur) cur = cur->prev;
      return *this;
    }

    const T &operator*() const { return *(reinterpret_cast<const node *>(cur)->ptr()); }
    const T *operator->() const noexcept { return reinterpret_cast<const node *>(cur)->ptr(); }

    bool operator==(const const_iterator &rhs) const { return cur == rhs.cur; }
    bool operator==(const iterator &rhs) const { return cur == rhs.cur; }

    bool operator!=(const const_iterator &rhs) const { return cur != rhs.cur; }
    bool operator!=(const iterator &rhs) const { return cur != rhs.cur; }
  };

  // iterator cross-type comparisons are defined inline in the classes above

  // Constructors
  list() : sentinel(), sz(0) {}

  list(const list &other) : sentinel(), sz(0) {
    for (auto it = other.cbegin(); it != other.cend(); ++it) {
      push_back(*it);
    }
  }

  virtual ~list() { clear(); }

  list &operator=(const list &other) {
    if (this == &other) return *this;
    clear();
    for (auto it = other.cbegin(); it != other.cend(); ++it) push_back(*it);
    return *this;
  }

  const T &front() const { return *cbegin(); }
  const T &back() const { auto it = cend(); --it; return *it; }

  iterator begin() { return iterator(sentinel.next, this); }
  const_iterator cbegin() const { return const_iterator(sentinel.next, this); }

  iterator end() { return iterator(&sentinel, this); }
  const_iterator cend() const { return const_iterator(&sentinel, this); }

  virtual bool empty() const { return sz == 0; }
  virtual size_t size() const { return sz; }

  virtual void clear() {
    node *p = sentinel.next;
    while (p != &sentinel) {
      node *nxt = p->next;
      erase(p);
      delete p;
      p = nxt;
    }
    // reset sentinel links
    sentinel.next = sentinel.prev = &sentinel;
    sz = 0;
  }

  virtual iterator insert(iterator pos, const T &value) {
    node *newn = new node(value);
    insert(pos.cur, newn);
    return iterator(newn, this);
  }

  virtual iterator erase(iterator pos) {
    node *p = pos.cur;
    node *nxt = p->next;
    erase(p);
    delete p;
    return iterator(nxt, this);
  }

  void push_back(const T &value) { insert(iterator(&sentinel, this), value); }
  void pop_back() {
    iterator it = end();
    --it;
    erase(it);
  }
  void push_front(const T &value) { insert(begin(), value); }
  void pop_front() { erase(begin()); }
};

} // namespace sjtu

#endif // SJTU_LIST_HPP
