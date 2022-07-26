#pragma once

#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <functional>


namespace textgen
{


enum class PinType
{
    image
    // add SuperImage, ArrayImage, ArraySuperImage, perhaps float or int?
};

enum class PinDirection { intput, output };


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

struct Node;

struct Pin
{
    unsigned int id;
    PinType type;
    PinDirection direction;
    std::string name;

    Node* node;

    Pin(PinType type, const std::string& name);
};

struct Link
{
    unsigned int id;

    unsigned int start_pin;
    unsigned int end_pin;
};

struct Node
{
    unsigned int id = 0;
    std::vector<Pin> inputs;
    std::vector<Pin> outputs;

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

    NoiseNode();
    std::string get_name() override;
    void do_work() override;
};

struct DummyNode : Node
{
    DummyNode();
    std::string get_name() override;
    void do_work() override;
};

struct TextGen
{
    TextGen();

    unsigned int next_id = 1;

    std::vector<std::unique_ptr<Node>> nodes;
    std::vector<std::unique_ptr<Link>> links;

    MakeNativeImageFun make_native_image_fun;

    unsigned int create_new_id();
    void work();

    Node* find_node(unsigned int id);
    Link* find_link(unsigned int id);
    Pin* find_pin(unsigned int id);

    bool is_pin_linked(unsigned int id) const;
    bool can_create_link(Pin* a, Pin* b) const;
};


}
