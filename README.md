# Class `any_of`

This is a _constrained_ version of the C++ class
[`std::any`](https://en.cppreference.com/w/cpp/utility/any). It provides a
single template class, `any_of<YourBaseClass>`, that can store any (copyable)
object of a class derived from `YourBaseClass`. Constraining it by a base class
means you can access base class members without needing to downcast to some
specific subclass, or without even knowing what the specific subclass is. This
is in contrast to [`std::any`](https://en.cppreference.com/w/cpp/utility/any)
where you can't do anything useful to the object without knowing the underlying
type.

## Example code

```cpp
#include "any_of.h"
using sz::any_of;

// Your own base class.
class Greeter {
 public:
  virtual std::string show_msg() const = 0;
  virtual ~Greeter() {}
};

// Use the base class, copy it, destroy it, without knowing its runtime type.
void show_greeting(const any_of<Greeter>& g) {
  any_of<Greeter> g2 = g;  // copied here.
  std::cout << g2->show_msg() << std::endl;  // base class method show_msg().
  // g2 destroyed here.
}

// Later, derive a concrete runtime type, and use it.
class HelloGreeter : public Greeter {
 public:
  std::string show_msg() const override { return "hello"; }
};

int main() {
  any_of<Greeter> g = HelloGreeter{};
  show_greeting(g);

  // Get back the full type if needed.
  HelloGreeter* hg = any_of_cast<HelloGreeter>(&g);
  if(hg != nullptr) std::cout << "Downcast successful!" << std::endl;
}
```

## When to use

I myself found it useful in breaking dependency chains. For example, when I
didn't want to include too many header files when defining a copyable type. Or
when I wanted to keep an incomplete type as a member, which is declared but not
yet defined. Recursive data structures is another common use case.


## How to use

It's a single-file standalone library, so just drop the header file `any_of.h`
into your project's include path and it should work.
