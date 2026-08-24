#pragma once

#include "IInspectable.h"
#include <vector>

struct GLFWwindow;

class UserInterface {
public:
    explicit UserInterface(GLFWwindow *window);
    ~UserInterface();

    UserInterface(const UserInterface&) = delete;
    UserInterface& operator=(const UserInterface &) = delete;

    void addPanel(IInspectable *panel);

    void beginFrame(); 
    void render();     

private:
    std::vector<IInspectable *> panels_;
};
