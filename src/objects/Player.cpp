#include "Player.hpp"

namespace sufy { namespace objects {

    Player::Player(float x, float y, Handler *handler): Object({x, y}){
        this->handler = handler;
        this->loadAnimations();
    }

    void Player::loadAnimations() {
        this->sprite.setTextureRect({0, 0, 32, 64});
        this->sprite.scale(2.0f, 3.0f);
        this->currentAnimation = AnimationType::IdleRight;
        this->animations = {
                sufy::graphics::Animation("IdleRight.png", 0, 0, 24, 32, 11, 0.15f, false, true, [](){}),
                sufy::graphics::Animation("IdleLeft.png", 0, 0, 24, 32, 11, 0.15f, true, true, [](){}),
                sufy::graphics::Animation("WalkRight.png", 0, 0, 22, 33, 13, 0.05f, false, true, [](){}),
                sufy::graphics::Animation("WalkLeft.png", 0, 0, 22, 33, 13, 0.05f, true, true, [](){}),
                sufy::graphics::Animation("AttackRight.png", 0, 0, 43, 37, 18, 0.05f, false, false, std::bind(&Player::attackComplete, this)),
                sufy::graphics::Animation("AttackLeft.png", 0, 0, 43, 37, 18, 0.05f, true, false, std::bind(&Player::attackComplete, this)),
                sufy::graphics::Animation("HitRight.png", 0, 0, 30, 32, 8, 0.1f, false, true, [](){}),
                sufy::graphics::Animation("HitLeft.png", 0, 0, 30, 32, 8, 0.1f, true, true, [](){}),
                sufy::graphics::Animation("ReactRight.png", 0, 0, 22, 32, 4, 0.1f, false, true, [](){}),
                sufy::graphics::Animation("ReactLeft.png", 0, 0, 22, 32, 4, 0.1f, true, true, [](){}),
                sufy::graphics::Animation("DeadRight.png", 0, 0, 33, 32, 15, 0.1f, false, true, [](){}),
                sufy::graphics::Animation("DeadLeft.png", 0, 0, 33, 32, 15, 0.1f, true, true, [](){}),
        };
    }

    void Player::update(float dt) {
        this->input();
        this->position += this->velocity;

        if (this->velocity.x == 0.0f) {
            if (this->direction < 0) this->currentAnimation = AnimationType::IdleLeft;
            else if (this->direction > 0) this->currentAnimation = AnimationType::IdleRight;
        } else if (this->velocity.x < 0.0f) {
            this->direction = -1;
            this->currentAnimation = AnimationType::WalkLeft;
        } else if (this->velocity.x > 0.0f) {
            this->direction = 1;
            this->currentAnimation = AnimationType::WalkRight;
        }

        if (this->attacking) {
            if (this->direction < 0) this->currentAnimation = AnimationType::AttackLeft;
            else if (this->direction > 0) this->currentAnimation = AnimationType::AttackRight;
        }

        if (this->falling || this->jumping) {
            this->velocity.y += this->gravity;
            if (this->velocity.y > this->terminal) this->velocity.y = this->terminal;
        }

        this->collision();
        this->animations[static_cast<int>(this->currentAnimation)].update(dt);
        this->animations[static_cast<int>(this->currentAnimation)].applyToSprite(sprite);
        if (this->currentAnimation == AnimationType::AttackRight) {
            this->sprite.setPosition(this->position.x, this->position.y - 20);
        } else if (this->currentAnimation == AnimationType::AttackLeft) {
            this->sprite.setPosition(this->position.x - 30, this->position.y - 20);
        } else {
            this->sprite.setPosition(this->position);
        }
    }

    void Player::render(sf::RenderTarget &rt) {
        rt.draw(this->sprite);

        if (DEBUGGING) {
            this->renderBody(rt);
            this->renderTop(rt);
            this->renderBottom(rt);
            this->renderRight(rt);
            this->renderLeft(rt);
        }
    }

    void Player::renderBody(sf::RenderTarget &rt) {
        sf::RectangleShape body;
        sf::FloatRect bounds = this->getBounds();
        body.setSize({bounds.width, bounds.height});
        body.setOutlineThickness(1.0f);
        body.setOutlineColor(sf::Color::Blue);
        body.setFillColor(sf::Color::Transparent);
        body.setPosition(bounds.left, bounds.top);
        rt.draw(body);
    }

    void Player::renderTop(sf::RenderTarget &rt) {
        sf::RectangleShape above;
        sf::FloatRect top = this->getBoundsTop();
        above.setSize({top.width, top.height});
        above.setOutlineThickness(1.0f);
        above.setOutlineColor(sf::Color::Red);
        above.setFillColor(sf::Color::Transparent);
        above.setPosition(top.left, top.top);
        rt.draw(above);
    }

    void Player::renderBottom(sf::RenderTarget &rt) {
        sf::RectangleShape below;
        sf::FloatRect bottom = this->getBoundsBottom();
        below.setSize({bottom.width, bottom.height});
        below.setOutlineThickness(1.0f);
        below.setOutlineColor(sf::Color::Red);
        below.setFillColor(sf::Color::Transparent);
        below.setPosition(bottom.left, bottom.top);
        rt.draw(below);
    }

    void Player::renderRight(sf::RenderTarget &rt) {
        sf::RectangleShape rightwards;
        sf::FloatRect right = this->getBoundsRight();
        rightwards.setSize({right.width, right.height});
        rightwards.setOutlineThickness(1.0f);
        rightwards.setOutlineColor(sf::Color::Green);
        rightwards.setFillColor(sf::Color::Transparent);
        rightwards.setPosition(right.left, right.top);
        rt.draw(rightwards);
    }

    void Player::renderLeft(sf::RenderTarget &rt) {
        sf::RectangleShape leftwards;
        sf::FloatRect left = this->getBoundsLeft();
        leftwards.setSize({left.width, left.height});
        leftwards.setOutlineThickness(1.0f);
        leftwards.setOutlineColor(sf::Color::Green);
        leftwards.setFillColor(sf::Color::Transparent);
        leftwards.setPosition(left.left, left.top);
        rt.draw(leftwards);
    }

    void Player::collision() {
        for (auto object : this->handler->objects) {
            auto block = std::dynamic_pointer_cast<sufy::objects::Block>(object);
            if (block and block->isCollidable() and block->isVisible()) {
                if (this->getBoundsTop().intersects(block->getBounds())) {
                    this->position.y = block->getPos().y + block->getBounds().height;
                    this->velocity.y = 0.0f;
                }

                if (this->getBoundsBottom().intersects(block->getBounds())) {
                    this->position.y = block->getPos().y - this->height;
                    this->velocity.y = 0.0f;
                    this->falling = false;
                    this->jumping = false;
                } else {
                    this->falling = true;
                }

                if (this->getBoundsRight().intersects(block->getBounds())) {
                    this->position.x = block->getPos().x - this->width;
                    if (this->attacking and block->isBreakable()) block->destroy();
                }

                if (this->getBoundsLeft().intersects(block->getBounds())) {
                    this->position.x = block->getPos().x + block->getBounds().width;
                    if (this->attacking and block->isBreakable()) block->destroy();
                }
            }
        }
    }

    bool Player::attack() {
        return (
                sf::Keyboard::isKeyPressed(sf::Keyboard::Space) or
                (sf::Joystick::isConnected(0) and sf::Joystick::isButtonPressed(0, sf::Joystick::Y))
        );
    }

    bool Player::up() {
        return (
                sf::Keyboard::isKeyPressed(sf::Keyboard::W) or
                sf::Keyboard::isKeyPressed(sf::Keyboard::Up) or
                (sf::Joystick::isConnected(0) and sf::Joystick::isButtonPressed(0, sf::Joystick::X))
        );
    }

    bool Player::right() {
        return (
                sf::Keyboard::isKeyPressed(sf::Keyboard::D) or
                sf::Keyboard::isKeyPressed(sf::Keyboard::Right) or
                (sf::Joystick::isConnected(0) and sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X) > 50)
        );
    }

    bool Player::left() {
        return (
                sf::Keyboard::isKeyPressed(sf::Keyboard::A) or
                sf::Keyboard::isKeyPressed(sf::Keyboard::Left) or
                (sf::Joystick::isConnected(0) and sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X) < -50)
        );
    }

    void Player::input() {
        if (this->right()) this->velocity.x = 5.0f;
        else if (this->left()) this->velocity.x = -5.0f;
        else this->velocity.x = 0.0f;

        if (this->up() and not this->jumping) {
            this->jumping = true;
            this->velocity.y = -10.0f;
        }

        if (this->attack()) this->attacking = true;
    }

    sf::FloatRect Player::getBounds() {
        return {
                this->position.x,
                this->position.y,
                this->width,
                this->height
        };
    }

    sf::FloatRect Player::getBoundsTop() {
        return {
                this->position.x + (this->width/2.0f) - ((this->width/2.0f)/2.0f),
                this->position.y,
                this->width/2.0f,
                this->height/2.0f
        };
    }

    sf::FloatRect Player::getBoundsBottom() {
        return {
                this->position.x + (this->width/2.0f) - ((this->width/2.0f)/2.0f),
                this->position.y + (this->height/2.0f),
                this->width/2.0f,
                this->height/2.0f
        };
    }

    sf::FloatRect Player::getBoundsRight() {
        return {
                this->position.x + this->width - 5.0f,
                this->position.y + 5.0f,
                5.0f,
                this->height -10.0f
        };
    }

    sf::FloatRect Player::getBoundsLeft() {
        return {
                this->position.x,
                this->position.y + 5.0f,
                5.0f,
                this->height - 10.0f
        };
    }

    void Player::attackComplete() {
        this->attacking = false;
        this->animations[static_cast<int>(this->currentAnimation)].reset();
    }

}}
