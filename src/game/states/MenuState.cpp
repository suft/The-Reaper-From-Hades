#include "MenuState.hpp"

namespace sufy { namespace game { namespace states {

    MenuState::MenuState(sufy::state::StateMachine<sf::RenderWindow> &machine, sf::RenderWindow &adapter, sufy::state::StateMode mode): State(machine, adapter, mode) {
        this->background = sf::Color::Black;
        int items = 4;
        std::string names[] = {"Play", "Options", "Help", "Quit"};
        sufy::constant::Callback callbacks[] = {
            std::bind(&MenuState::playIntent, this),
            std::bind(&MenuState::optionsIntent, this),
            std::bind(&MenuState::helpIntent, this),
            std::bind(&MenuState::quitIntent, this)
        };
        this->menu.add(std::shared_ptr<sufy::window::gui::TextView>(new sufy::window::gui::TextView(
            sufy::constant::HEIGHT / 4,
            sufy::utils::Text(
                "Hades",
                *sufy::utils::Codex::AcquireFont("Gelio.ttf"),
                sufy::constant::HEIGHT / 3,
                3.5,
                sf::Color(223, 151, 83),
                sf::Color(223, 151, 83)
            )
        )));
        for (int i = 0; i < items; ++i) {
            this->menu.add(std::shared_ptr<sufy::window::gui::Button>(new sufy::window::gui::Button(
                (sufy::constant::HEIGHT / 2) + (i * (sufy::constant::HEIGHT / 9)),
                sufy::utils::Text(
                    names[i],
                    *sufy::utils::Codex::AcquireFont("Gelio.ttf"),
                    sufy::constant::HEIGHT / 10,
                    1.5f,
                    sf::Color(),
                    sf::Color()
                ),
                {
                    {sf::Color::White, sf::Color::White},
                    {sufy::constant::SLATEGREY, sufy::constant::SLATEGREY},
                    {sf::Color(180, 222, 226), sf::Color(180, 222, 226)}
                },
                callbacks[i]
            )));
        }
        this->menu.finalize();
    }

    void MenuState::playIntent() {
        this->after = std::unique_ptr<PlayState>(new PlayState(this->machine, this->adapter, sufy::state::StateMode::REPLACE_TOP));
    }

    void MenuState::optionsIntent() {}

    void MenuState::helpIntent() {}

    void MenuState::quitIntent() {
        this->machine.quit();
    }

    void MenuState::pause() {}
    void MenuState::resume() {}

    void MenuState::update(float dt) {
        sf::Event event;
        while (this->adapter.pollEvent(event)) this->handleEvents(event);
        this->menu.update(dt);
    }

    void MenuState::render() {
        this->adapter.clear(this->background);
        this->menu.render(this->adapter);
        this->adapter.display();
    }

    void MenuState::handleEvents(const sf::Event &event) {
        switch (event.type) {
            case sf::Event::Closed:
                this->machine.quit();
                break;
            case sf::Event::KeyReleased:
                menu.handleInput(event);
                break;
            case sf::Event::JoystickButtonPressed:
                menu.handleInput(event);
            default:
                break;
        }
    }

}}}
