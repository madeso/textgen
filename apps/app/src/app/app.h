#pragma once

#include <memory>

struct App
{
    virtual ~App() = default;
    virtual void on_gui() = 0;
};

std::unique_ptr<App> make_app();