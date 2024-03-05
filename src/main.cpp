
#include "first_app.hpp"
#include "dk_client.hpp"
// std
#include <cstdlib>
#include <iostream>
#include <stdexcept>

int main()
{
  std::cout << "Connecting to server..." << std::endl;
  if (connectToServer() == 1)
  {
    std::cerr << "Failed to connect to server" << std::endl;
  }
  lve::FirstApp app{};

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