#include "textgen/textgen.h"

// #include <iostream>
#include <cassert>

#include "FastNoiseLite.h"



namespace textgen { namespace {



unsigned int get_channel_size(Channels ch)
{
    switch(ch)
    {
    case Channels::grayscale: return 1;
    case Channels::rgb: return 3;
    default:
        assert(false && "unknown channel in get_channel_size");
        return 1;
    }
}


std::size_t get_index_from_xy(unsigned x, unsigned y, unsigned int height)
{
    return y * height + x;
}


}}




namespace textgen
{


// ----------------------------------------------------------------------------


void Image::setup(unsigned int new_width, unsigned int new_height, Channels new_channels)
{
    width = new_width;
    height = new_height;
    channels = new_channels;

    const auto size = width * height * get_channel_size(channels);
    pixels.resize(size);
}



Rgb Image::get(unsigned int x, unsigned int y) const
{
    assert(x < width && y < height);
    const auto i = get_index_from_xy(x, y, height) * get_channel_size(channels);
    switch(channels)
    {
    case Channels::grayscale: return Rgb{pixels[i]};
    case Channels::rgb: return {pixels[i+0], pixels[i+1], pixels[i+2]};
    default:
        assert(false && "unknown channel in Image::get(...)");
        return Rgb{0.0f};
    }
}



void Image::set(unsigned int x, unsigned int y, const Rgb& c)
{
    assert(x < width && y < height);
    const auto i = get_index_from_xy(x, y, height) * get_channel_size(channels);
    switch(channels)
    {
    case Channels::grayscale: pixels[i] = c.r; break;
    case Channels::rgb:
    {
        pixels[i+0] = c.r;
        pixels[i+1] = c.g;
        pixels[i+2] = c.b;
        break;
    }
    default:
        assert(false && "unknown channel in Image::set(...)");
        break;
    }
}


// ----------------------------------------------------------------------------


Pin::Pin(PinType t, const std::string& n)
    : id(0)
    , type(t)
    , direction(PinDirection::intput)
    , name(n)
    , node(nullptr)
{
}


// ----------------------------------------------------------------------------


bool
Node::work()
{
    if(dirty == false) { return false; }

    do_work();
    dirty = false;

    return true;
}


// ----------------------------------------------------------------------------


NoiseNode::NoiseNode()
{
    outputs.emplace_back(PinType::image, "out");
}


std::string
NoiseNode::get_name()
{
    return "open simplex x2";
}


void
NoiseNode::do_work()
{
    unsigned int size = 128;
    
    auto noise = FastNoiseLite{};
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);

    image = Image{};
    image->setup(size, size, Channels::grayscale);
    
    for (unsigned int y = 0; y < size; y++)
    {
        for (unsigned int x = 0; x < size; x++)
        {
            const auto color = noise.GetNoise
            (
                dx + static_cast<float>(x) * scale,
                dy + static_cast<float>(y) * scale
            );
            image->set(x, y, Rgb{color});
        }
    }
}


// ----------------------------------------------------------------------------


DummyNode::DummyNode()
{
    inputs.emplace_back(PinType::image, "noise");
    outputs.emplace_back(PinType::image, "diffuse");
    outputs.emplace_back(PinType::image, "ambient");
}


std::string
DummyNode::get_name()
{
    return "dummy";
}


void
DummyNode::do_work()
{
}


// ----------------------------------------------------------------------------


TextGen::TextGen()
    : make_native_image_fun([](const Image&){return nullptr;})
{
}


unsigned int
TextGen::create_new_id()
{
    const auto new_id = next_id;
    next_id += 1;
    return new_id;
}


void
TextGen::work()
{
    for(auto& n: links)
    {
        if(n->id == 0) { n->id = create_new_id(); }
    }

    for(auto& n: nodes)
    {
        // update id and node refs
        if(n->id == 0) { n->id = create_new_id(); }
        for(auto& p: n->inputs)
        {
            if(p.id == 0) { p.id = create_new_id(); }
            p.node = n.get();
            p.direction = PinDirection::intput;
        }
        for(auto& p: n->outputs)
        {
            if(p.id == 0) { p.id = create_new_id(); }
            p.node = n.get();
            p.direction = PinDirection::output;
        }

        const auto was_updated = n->work();
        if(was_updated && n->image)
        {
            n->native_image = make_native_image_fun(*n->image);
        }
    }
}




Node*
TextGen::find_node(unsigned int id)
{
    assert(id != 0);
    if (id == 0) { return nullptr; }

    for (auto& node :nodes)
    {
        if (node->id == id)
        {
            return node.get();
        }
    }

    return nullptr;
}



Link*
TextGen::find_link(unsigned int id)
{
    assert(id != 0);
    if (id == 0) { return nullptr; }

    for (auto& link : links)
    {
        if (link->id == id)
        {
            return link.get();
        }
    }

    return nullptr;
}



Pin*
TextGen::find_pin(unsigned int id)
{
    assert(id != 0);
    if (id == 0) { return nullptr; }

    for (auto& node : nodes)
    {
        for (auto& pin : node->inputs)
        {
            if (pin.id == id)
            {
                return &pin;
            }
        }

        for (auto& pin : node->outputs)
        {
            if (pin.id == id)
            {
                return &pin;
            }
        }
    }

    return nullptr;
}


bool TextGen::is_pin_linked(unsigned int id) const
{
    assert(id == 0);
    if (id == 0) { return false; }

    for (auto& link : links)
    {
        if (link->start_pin == id || link->end_pin == id)
        {
            return true;
        }
    }

    return false;
}



bool
TextGen::can_create_link(Pin* a, Pin* b) const
{
    if(a == nullptr || b == nullptr)
    {
        // std::cout << "must be valid nodes\n";
        return false;
    }
    if(a == b)
    {
        // std::cout << "can't be the same node\n";
        return false;
    }
    if(a->direction == b->direction)
    {
        // std::cout << "one must be output and one must be input\n";
        return false;
    }
    if(a->type != b->type)
    {
        // std::cout << "the types must match\n";
        return false;
    }
    if(a->node == b->node)
    {
        // std::cout << "they can't be part of the same node\n";
        return false;
    }

    // std::cout << "accepting link\n";
    return true;
}


}