/*  Copyright 2023 Samee Zahur <samee.zahur@gmail.com>

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        https://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License. */

/* This file implements any_of<Base>. It's meant to be like std::any, but it
   only accepts values that derive from Base. Just like std::any, they need to
   be copyable. The Base class methods can be used without any cast, using the
   usual pointer operators ('*' and '->').
  
   Right now, it only implements the small API that I need for my own projects.
   It is easy enough to extend it if needed.
  
   See any_of_test.cpp for common usage patterns.
 */
#pragma once
#include <type_traits>  // for std::is_base_of
#include <typeinfo>
#include <utility>  // for std::move()

namespace sz {

namespace internal {

template <class Base>
class any_of_dispatcher_base {
 public:
  virtual ~any_of_dispatcher_base() {}

  virtual Base* get() = 0;
  virtual any_of_dispatcher_base* copy() const = 0;
};

template <class Base, class Derived>
class any_of_dispatcher : public any_of_dispatcher_base<Base> {
  Derived data;
 public:
  any_of_dispatcher(const Derived& d) : data{d} {}
  any_of_dispatcher(Derived&& d) : data{std::move(d)} {}
  any_of_dispatcher* copy() const override {
    return new any_of_dispatcher(*this);
  }
  Derived* get() override { return &data; }
  const Derived* get() const { return &data; }
};

}  // namespace internal

template <class Base>
class any_of {
 public:
  any_of() : ptr_{nullptr} {}

  // This checks is_base_of, but we can relax this if needed.
  // In the worst case, just delete the second type parameter.
  template <class Derived,
            class = typename std::enable_if<
              std::is_base_of<Base,Derived>::value
            >::type>
  any_of(Derived d)  // can't be noexcept, because 'new'
    : ptr_{new internal::any_of_dispatcher<Base,Derived>(std::move(d))} {}
  any_of(const any_of& that) : ptr_(that.ptr_->copy()) {}
  any_of(any_of&& that) noexcept { ptr_ = that.ptr_; that.ptr_ = nullptr; }
  ~any_of() { delete ptr_; }

  any_of& operator=(const any_of& that) {
    reset();
    if(that) this->ptr_ = that.ptr_->copy();
    return *this;
  }
  any_of& operator=(any_of&& that) noexcept {
    reset();
    this->ptr_ = that.ptr_;
    that.ptr_ = nullptr;
    return *this;
  }

  void reset() noexcept { delete ptr_; ptr_ = nullptr; }
  bool has_value() const noexcept { return ptr_ != nullptr; }
  operator bool() const noexcept { return ptr_ != nullptr; }
  const std::type_info& type() const {
    return ptr_ ? typeid(*ptr_->get()) : typeid(void);
  }

  Base& operator*() { return *ptr_->get(); }
  const Base& operator*() const { return *ptr_->get(); }
  Base* operator->() { return ptr_->get(); }
  const Base* operator->() const { return ptr_->get(); }
  Base* get() { return ptr_->get(); }
  const Base* get() const { return ptr_->get(); }

  template <class D, class B> friend D* any_of_cast(any_of<B>* a);
  template <class D, class B> friend const D* any_of_cast(const any_of<B>* a);
 private:
  internal::any_of_dispatcher_base<Base>* ptr_;
};

template <class D, class B>
D* any_of_cast(any_of<B>* a) {
  if (!a || !a->ptr_) return nullptr;
  auto* d = dynamic_cast<internal::any_of_dispatcher<B,D>*>(a->ptr_);
  return d ? d->get() : nullptr;
}
template <class D, class B>
const D* any_of_cast(const any_of<B>* a) {
  if (!a || !a->ptr_) return nullptr;
  auto* d = dynamic_cast<const internal::any_of_dispatcher<B,D>*>(a->ptr_);
  return d ? d->get() : nullptr;
}

// Dev-note: add reference versions of any_of_cast if we need them.
// Use the pointer versions to implement them. No need to add new friends.

}  // namespace sz
