#pragma once

#include <vector>
#include <memory>
#include <SFML/Graphics/RenderTarget.hpp>
#include "Object.hpp"

namespace sufy { namespace objects {

    class Handler {
    private:
        std::vector<std::shared_ptr<Object>> objects;
    public:
        Handler() = default;

        void update(float dt);
        void render(sf::RenderTarget& rt);
    };
}}