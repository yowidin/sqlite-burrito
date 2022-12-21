/**
 * @file   iterator.h
 * @author Dennis Sitelew
 * @date   Jul. 23, 2020
 */
#ifndef INCLUDE_SQLITE_BURRITO_DETAIL_ITERATOR_H
#define INCLUDE_SQLITE_BURRITO_DETAIL_ITERATOR_H

#include <type_traits>
#include <iterator>

namespace sqlite_burrito {

template <typename ValueType,
          typename ValueGetter,
          typename DependencyType,
          typename Owner>
class index_based_iterator
   : public std::iterator<std::random_access_iterator_tag,
                          ValueType,
                          int,
                          const ValueType *,
                          const ValueType &> {
private:
   friend Owner;

public:
   using base_t = std::iterator<std::random_access_iterator_tag,
                                ValueType,
                                int,
                                const ValueType *,
                                const ValueType &>;

   using self_t =
       index_based_iterator<ValueType, ValueGetter, DependencyType, Owner>;

   using reference = typename base_t::reference;

protected:
   index_based_iterator(int idx, const DependencyType &dep)
      : index_{idx}
      , dep_{&dep}
      , value_{get_value(dep, idx)} {
      // Nothing to do here
   }

public:
   index_based_iterator(const index_based_iterator &other) = default;
   index_based_iterator(index_based_iterator &&other) = default;

public:
   index_based_iterator &operator=(const index_based_iterator &other) = default;
   index_based_iterator &operator=(index_based_iterator &&other) = default;

private:
   static ValueType get_value(const DependencyType &dep, int index) {
      return ValueGetter()(dep, index);
   }

public:
   reference operator*() const { return value_; }

   // --i
   self_t &operator--() {
      *this -= 1;
      return *this;
   }

   // i--
   self_t operator--(int) {
      self_t it{*this};
      --(*this);
      return it;
   }

   self_t &operator++() {
      *this += 1;
      return *this;
   }

   self_t operator++(int) {
      self_t it{*this};
      ++(*this);
      return it;
   }

   self_t &operator+=(int n) {
      index_ += n;
      value_ = get_value(*dep_, index_);
      return *this;
   }

   self_t &operator-=(int n) { return *this += -n; }

   friend self_t operator+(const self_t &other, int n) {
      return self_t{other.index_ + n, *other.dep_};
   }

   friend self_t operator+(int n, const self_t &other) {
      return other + n;
   }

   friend self_t operator-(const self_t &other, int n) {
      return self_t{other.index_ - n, *other.dep_};
   }

   friend self_t operator-(int n, const self_t &other) {
      return other - n;
   }

   ValueType operator[](int n) const {
      return *(*this + n);
   }

   friend bool operator<(const self_t &lhs, const self_t &rhs) {
      return lhs.index_ < rhs.index_;
   }

   friend bool operator>(const self_t &lhs, const self_t &rhs) {
      return lhs.index_ > rhs.index_;
   }

   friend bool operator>=(const self_t &lhs, const self_t &rhs) {
      return lhs.index_ >= rhs.index_;
   }

   friend bool operator<=(const self_t &lhs, const self_t &rhs) {
      return lhs.index_ <= rhs.index_;
   }

   bool operator==(const self_t &other) const {
      return index_ == other.index_;
   }

   bool operator!=(const self_t &other) const { return index_ != other.index_; }

private:
   int index_;
   const DependencyType *dep_;
   ValueType value_;
};

} // namespace sqlite_burrito

#endif /* INCLUDE_SQLITE_BURRITO_DETAIL_ITERATOR_H */
