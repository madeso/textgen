#include "app/app.h"

struct TextGen : App
{
    void on_gui() override
    {
    }
};


std::unique_ptr<App> make_app()
{
    return std::make_unique<TextGen>();
}
