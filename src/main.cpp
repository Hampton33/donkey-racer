
#include "dk_app.hpp"
// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main()
{
  std::cout << "Connecting to server..." << std::endl;

  lve::DkApp app{};
  try
  {

    app.run();
  }
  catch (const std::exception &e)
  {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}