#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <functional>


namespace textgen
{


struct Rgb
{
    float r = 0.0f;
    float g = 0.0f;
    float b = 0.0f;

    constexpr explicit Rgb(float gray)
        : r(gray)
        , g(gray)
        , b(gray)
    {}

    constexpr Rgb(float ar, float ag, float ab)
        : r(ar)
        , g(ag)
        , b(ab)
    {}
};

enum class Channels
{
    grayscale, rgb
};

struct Image
{
    unsigned int width = 0;
    unsigned int height = 0;
    Channels channels = Channels::grayscale;
    std::vector<float> pixels;

    void setup(unsigned int width, unsigned int height, Channels channels);
    Rgb get(unsigned int x, unsigned int y) const;
    void set(unsigned int x, unsigned int y, const Rgb& c);
};


struct NativeImage
{
    virtual ~NativeImage() = default;
};


using MakeNativeImageFun = std::function<std::unique_ptr<NativeImage>(const Image&)>;

struct Node
{
    unsigned int id = 0;

    virtual ~Node() = default;
    bool work();

    bool dirty = true;
    std::optional<Image> image;
    std::unique_ptr<NativeImage> native_image;

    virtual std::string get_name() = 0;
    virtual void do_work() = 0;
};


struct NoiseNode : Node
{
    float scale = 1.0f;
    float dx = 0.0f;
    float dy = 0.0f;

    std::string get_name() override;
    void do_work() override;
};

struct TextGen
{
    TextGen();

    unsigned int next_id = 1;
    std::vector<std::unique_ptr<Node>> nodes;

    MakeNativeImageFun make_native_image_fun;

    unsigned int create_new_id();
    void work();
};


}
