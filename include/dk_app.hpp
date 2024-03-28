#pragma once

#include "lve_device.hpp"
#include "lve_game_object.hpp"
#include "lve_renderer.hpp"
#include "lve_window.hpp"
#include "dk_input.hpp"
#include "dk_client.hpp"
// std
#include <memory>
#include <vector>

namespace lve
{
  class DkApp
  {
  public:
    static constexpr int WIDTH = 800;
    static constexpr int HEIGHT = 600;

    DkApp();
    ~DkApp();

    DkApp(const DkApp &) = delete;
    DkApp &operator=(const DkApp &) = delete;

    dk::DkClient client;
    bool isThreadRunning() { return client.isThreadRunning; };
    void run();
    std::shared_ptr<LveModel> carcarModel = nullptr;
    void makeModelForPlayers(std::vector<dk::Player> &players);

  private:
    void loadGameObjects();
    LveWindow lveWindow{WIDTH, HEIGHT, "DONKEY RACER"};
    LveDevice lveDevice{lveWindow};
    LveRenderer lveRenderer{lveWindow, lveDevice};

    std::vector<LveGameObject> gameObjects;
    LveGameObject viewerObject = LveGameObject::createGameObject();
  };
} // namespace lve
