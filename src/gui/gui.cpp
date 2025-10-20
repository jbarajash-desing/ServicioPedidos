#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "../../lib/factory/include/factory.hpp"
#include "gui.hpp"

namespace {

struct Button {
    sf::RectangleShape shape;
    sf::Text text;
};

Button makeButton(const sf::Font& font, const std::string& label, const sf::Vector2f& size,
                  const sf::Vector2f& position) {
    Button button;
    button.shape.setSize(size);
    button.shape.setPosition(position);
    button.shape.setFillColor(sf::Color(52, 84, 209));
    button.shape.setOutlineThickness(2.f);
    button.shape.setOutlineColor(sf::Color(33, 51, 140));

    button.text.setFont(font);
    button.text.setString(label);
    button.text.setCharacterSize(22);
    button.text.setFillColor(sf::Color::White);

    sf::FloatRect textBounds = button.text.getLocalBounds();
    button.text.setOrigin(textBounds.left + textBounds.width / 2.f,
                          textBounds.top + textBounds.height / 2.f);
    button.text.setPosition(position.x + size.x / 2.f, position.y + size.y / 2.f);
    return button;
}

void updateButtonHover(Button& button, const sf::Vector2f& mousePosition) {
    if (button.shape.getGlobalBounds().contains(mousePosition)) {
        button.shape.setFillColor(sf::Color(66, 105, 232));
    } else {
        button.shape.setFillColor(sf::Color(52, 84, 209));
    }
}

sf::Text makeTitle(const sf::Font& font, const std::string& text, unsigned int size,
                   float windowWidth, float y) {
    sf::Text title(text, font, size);
    sf::FloatRect bounds = title.getLocalBounds();
    title.setOrigin(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    title.setPosition(windowWidth / 2.f, y);
    title.setFillColor(sf::Color::White);
    return title;
}

sf::FloatRect computeOptionBounds(std::size_t index, float windowWidth, const sf::Vector2f& optionSize,
                                  float startY, float spacing) {
    float x = (windowWidth - optionSize.x) / 2.f;
    float y = startY + static_cast<float>(index) * (optionSize.y + spacing);
    return {x, y, optionSize.x, optionSize.y};
}

void drawOption(sf::RenderWindow& window, const sf::Font& font, const std::string& text,
                const sf::FloatRect& bounds, const sf::Vector2f& mousePosition, bool isSelected) {
    sf::RectangleShape box;
    box.setPosition(bounds.left, bounds.top);
    box.setSize({bounds.width, bounds.height});
    box.setFillColor(isSelected ? sf::Color(74, 189, 131)
                                : (bounds.contains(mousePosition) ? sf::Color(90, 90, 90)
                                                                  : sf::Color(70, 70, 70)));
    box.setOutlineThickness(2.f);
    box.setOutlineColor(sf::Color(40, 40, 40));

    sf::Text label(text, font, 22);
    label.setFillColor(sf::Color::White);
    sf::FloatRect textBounds = label.getLocalBounds();
    label.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
    label.setPosition(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);

    window.draw(box);
    window.draw(label);
}

enum class AppState { MainMenu, PersonalOrder, Summary };

} // namespace

int runGui() {
    sf::RenderWindow window(sf::VideoMode(900, 600), "Gestion de pedidos");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.loadFromFile("assets/Roboto-Regular.ttf")) {
        std::cerr << "No se pudo cargar la fuente assets/Roboto-Regular.ttf. "
                     "Coloca un archivo TTF en esa ruta.\n";
        return 1;
    }

    Factory factory(5); // quantum = 5 unidades de tiempo

    Button personalButton = makeButton(font, "Pedido personal", {320.f, 80.f}, {290.f, 230.f});
    Button multipleButton = makeButton(font, "Pedido multiple (en desarrollo)", {320.f, 80.f}, {290.f, 340.f});

    AppState state = AppState::MainMenu;
    std::string infoMessage;
    std::string summaryText;
    std::vector<Order> processedOrders;

    const std::vector<std::string> dayOptions = {
        "Lunes", "Martes", "Miercoles", "Jueves", "Viernes", "Sabado", "Domingo"};
    const std::vector<std::string> typeOptions = {"Estandar", "Personalizado", "Express"};
    const std::vector<int> typeDurations = {6, 12, 3};
    const std::vector<std::string> priorityOptions = {"Alta", "Media", "Baja"};
    const std::vector<int> priorityUrgencies = {1, 2, 3};

    int personalStep = 0;
    int selectedDay = -1;
    int selectedType = -1;
    int selectedPriority = -1;

    const sf::Vector2f optionSize(360.f, 50.f);
    const float optionStartY = 200.f;
    const float optionSpacing = 14.f;

    while (window.isOpen()) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed &&
                       event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2f mouse = window.mapPixelToCoords({event.mouseButton.x, event.mouseButton.y});

                if (state == AppState::MainMenu) {
                    if (personalButton.shape.getGlobalBounds().contains(mouse)) {
                        state = AppState::PersonalOrder;
                        personalStep = 0;
                        selectedDay = selectedType = selectedPriority = -1;
                        infoMessage.clear();
                    } else if (multipleButton.shape.getGlobalBounds().contains(mouse)) {
                        infoMessage = "La opcion de pedido multiple aun no esta disponible.";
                    }
                } else if (state == AppState::PersonalOrder) {
                    Button backButton = makeButton(font, "Volver al menu", {180.f, 46.f}, {20.f, 20.f});
                    if (backButton.shape.getGlobalBounds().contains(mouse)) {
                        state = AppState::MainMenu;
                        infoMessage.clear();
                        continue;
                    }

                    if (personalStep < 3) {
                        const std::vector<std::string>* currentOptions = nullptr;
                        if (personalStep == 0) {
                            currentOptions = &dayOptions;
                        } else if (personalStep == 1) {
                            currentOptions = &typeOptions;
                        } else {
                            currentOptions = &priorityOptions;
                        }

                        for (std::size_t i = 0; i < currentOptions->size(); ++i) {
                            sf::FloatRect bounds = computeOptionBounds(i, static_cast<float>(window.getSize().x),
                                                                       optionSize, optionStartY, optionSpacing);
                            if (bounds.contains(mouse)) {
                                if (personalStep == 0) {
                                    selectedDay = static_cast<int>(i);
                                } else if (personalStep == 1) {
                                    selectedType = static_cast<int>(i);
                                } else {
                                    selectedPriority = static_cast<int>(i);
                                }
                                ++personalStep;
                                break;
                            }
                        }
                    } else {
                        Button confirmButton = makeButton(font, "Confirmar pedido", {250.f, 60.f}, {325.f, 420.f});
                        Button resetButton = makeButton(font, "Reiniciar seleccion", {250.f, 46.f}, {325.f, 500.f});

                        if (confirmButton.shape.getGlobalBounds().contains(mouse)) {
                            std::vector<std::vector<Order>> ordersPerHour(24);

                            int urgency = priorityUrgencies.at(selectedPriority);
                            int fabricationTime = typeDurations.at(selectedType);
                            std::string orderName =
                                "Personal_" + typeOptions.at(selectedType) + "_" + dayOptions.at(selectedDay);

                            ordersPerHour[0].push_back(Order(orderName, urgency, fabricationTime));
                            factory.receiveOrders(ordersPerHour);
                            factory.process();
                            processedOrders = factory.getLastBatch();

                            std::ostringstream summary;
                            summary << "Pedido personal creado:\n";
                            summary << "  Dia: " << dayOptions.at(selectedDay) << "\n";
                            summary << "  Tipo: " << typeOptions.at(selectedType) << "\n";
                            summary << "  Prioridad: " << priorityOptions.at(selectedPriority) << "\n\n";
                            summary << "Resultado del procesamiento:\n";
                            for (const auto& order : processedOrders) {
                                summary << "  - " << order.name << " (urgencia " << order.urgency << ")\n";
                            }
                            summaryText = summary.str();

                            state = AppState::Summary;
                            infoMessage.clear();
                        } else if (resetButton.shape.getGlobalBounds().contains(mouse)) {
                            personalStep = 0;
                            selectedDay = selectedType = selectedPriority = -1;
                        }
                    }
                } else if (state == AppState::Summary) {
                    Button backButton = makeButton(font, "Volver al menu", {220.f, 60.f}, {340.f, 460.f});
                    if (backButton.shape.getGlobalBounds().contains(mouse)) {
                        state = AppState::MainMenu;
                        infoMessage = "Pedido personal registrado correctamente.";
                    }
                }
            }
        }

        sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));

        if (state == AppState::MainMenu) {
            updateButtonHover(personalButton, mousePosition);
            updateButtonHover(multipleButton, mousePosition);
        }

        window.clear(sf::Color(30, 30, 30));

        if (state == AppState::MainMenu) {
            sf::Text title = makeTitle(font, "Elige el tipo de pedido", 32, static_cast<float>(window.getSize().x), 120.f);
            window.draw(title);
            window.draw(personalButton.shape);
            window.draw(personalButton.text);
            window.draw(multipleButton.shape);
            window.draw(multipleButton.text);
        } else if (state == AppState::PersonalOrder) {
            Button backButton = makeButton(font, "Volver al menu", {180.f, 46.f}, {20.f, 20.f});
            updateButtonHover(backButton, mousePosition);
            window.draw(backButton.shape);
            window.draw(backButton.text);

            std::string stepText;
            if (personalStep == 0) {
                stepText = "Paso 1 de 3: selecciona el dia para el pedido";
            } else if (personalStep == 1) {
                stepText = "Paso 2 de 3: selecciona el tipo de pedido";
            } else if (personalStep == 2) {
                stepText = "Paso 3 de 3: define la prioridad";
            } else {
                stepText = "Revisa tu seleccion antes de confirmar";
            }

            sf::Text title = makeTitle(font, stepText, 28, static_cast<float>(window.getSize().x), 120.f);
            window.draw(title);

            sf::Text recap("", font, 20);
            recap.setFillColor(sf::Color(200, 200, 200));
            recap.setPosition(80.f, 160.f);
            std::ostringstream recapStream;
            recapStream << "Dia: " << (selectedDay >= 0 ? dayOptions.at(selectedDay) : "-")
                        << "   Tipo: " << (selectedType >= 0 ? typeOptions.at(selectedType) : "-")
                        << "   Prioridad: " << (selectedPriority >= 0 ? priorityOptions.at(selectedPriority) : "-");
            recap.setString(recapStream.str());
            window.draw(recap);

            if (personalStep < 3) {
                const std::vector<std::string>* currentOptions = nullptr;
                int highlightedIndex = -1;

                if (personalStep == 0) {
                    currentOptions = &dayOptions;
                    highlightedIndex = selectedDay;
                } else if (personalStep == 1) {
                    currentOptions = &typeOptions;
                    highlightedIndex = selectedType;
                } else {
                    currentOptions = &priorityOptions;
                    highlightedIndex = selectedPriority;
                }

                for (std::size_t i = 0; i < currentOptions->size(); ++i) {
                    sf::FloatRect bounds = computeOptionBounds(i, static_cast<float>(window.getSize().x),
                                                               optionSize, optionStartY, optionSpacing);
                    drawOption(window, font, currentOptions->at(i), bounds, mousePosition,
                               highlightedIndex == static_cast<int>(i));
                }
            } else {
                Button confirmButton = makeButton(font, "Confirmar pedido", {250.f, 60.f}, {325.f, 420.f});
                Button resetButton = makeButton(font, "Reiniciar seleccion", {250.f, 46.f}, {325.f, 500.f});
                updateButtonHover(confirmButton, mousePosition);
                updateButtonHover(resetButton, mousePosition);

                window.draw(confirmButton.shape);
                window.draw(confirmButton.text);
                window.draw(resetButton.shape);
                window.draw(resetButton.text);
            }
        } else if (state == AppState::Summary) {
            sf::Text title = makeTitle(font, "Resumen del pedido", 32, static_cast<float>(window.getSize().x), 100.f);
            window.draw(title);

            sf::Text summary(summaryText, font, 20);
            summary.setFillColor(sf::Color(220, 220, 220));
            summary.setPosition(140.f, 160.f);
            window.draw(summary);

            Button backButton = makeButton(font, "Volver al menu", {220.f, 60.f}, {340.f, 460.f});
            updateButtonHover(backButton, mousePosition);
            window.draw(backButton.shape);
            window.draw(backButton.text);
        }

        if (!infoMessage.empty()) {
            sf::Text info(infoMessage, font, 18);
            info.setFillColor(sf::Color(200, 200, 0));
            info.setPosition(40.f, static_cast<float>(window.getSize().y) - 60.f);
            window.draw(info);
        }

        window.display();
    }

    return 0;
}
