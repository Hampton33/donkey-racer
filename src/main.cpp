
#include "first_app.hpp"
// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main()
{
  std::cout << "Connecting to server..." << std::endl;

  lve::FirstApp app{};
  try
  {
    if (!app.isThreadRunning())
    {
      std::cout << "Failed to connect to server" << std::endl;
    }
    app.run();
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}