#include <iostream>
#include <string>
#include "any_of.h"
using sz::any_of;
using sz::any_of_cast;

// Your base class. In this example, it's just an abstract method.
// But you can have more members too.
class Greeter {
 public:
  virtual std::string show_msg() const = 0;
  virtual ~Greeter() {}
};

// Use the base class, copy it, destroy it, without knowing its runtime type.
void show_greeting(const any_of<Greeter>& g) {
  any_of<Greeter> g2 = g;
  std::cout << g2->show_msg() << std::endl;
}

// Later, derive a concrete runtime type.
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

