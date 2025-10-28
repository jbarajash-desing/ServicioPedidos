// Versión final de gui.cpp con corrección de salto automático y repetición de letras
// Incluye temporizador en entrada de texto y espera de clic liberado

#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>
#include <chrono>
#include "../../lib/factory/include/factory.hpp"
#include "gui.hpp"
#include "../../lib/products/include/products.hpp"
#include <list> 
#include "../../lib/calendar/include/calendar.hpp"

 
extern void initProducts();
namespace fs = std::filesystem;
Calendar calendar;
Factory factory(2); // quantum de 2 unidades de tiempo
namespace {
using Clock = std::chrono::steady_clock;
Clock::time_point lastKeyPress = Clock::now();
bool clickReleased = true;

// --- Componentes y helpers ---
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
    button.text.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
    button.text.setPosition(position.x + size.x / 2.f, position.y + size.y / 2.f);
    return button;
}

void updateButtonHover(Button& button, const sf::Vector2f& mousePosition) {
    button.shape.setFillColor(button.shape.getGlobalBounds().contains(mousePosition)
        ? sf::Color(66, 105, 232) : sf::Color(52, 84, 209));
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

sf::FloatRect computeGridBounds(int index, int cols, const sf::Vector2f& itemSize, float startX, float startY, float spacingX, float spacingY) {
    int row = index / cols;
    int col = index % cols;
    float x = startX + col * (itemSize.x + spacingX);
    float y = startY + row * (itemSize.y + spacingY);
    return {x, y, itemSize.x, itemSize.y};
}

void drawOptionBox(sf::RenderWindow& window, const sf::Font& font, const std::string& text,
                   const sf::FloatRect& bounds, const sf::Vector2f& mousePosition, bool isSelected) {
    sf::RectangleShape box;
    box.setPosition(bounds.left, bounds.top);
    box.setSize({bounds.width, bounds.height});
    box.setFillColor(isSelected ? sf::Color(74, 189, 131)
                                : (bounds.contains(mousePosition) ? sf::Color(90, 90, 90)
                                                                  : sf::Color(70, 70, 70)));
    box.setOutlineThickness(2.f);
    box.setOutlineColor(sf::Color(40, 40, 40));
    window.draw(box);

    sf::Text label(text, font, 20);
    label.setFillColor(sf::Color::White);
    sf::FloatRect textBounds = label.getLocalBounds();
    label.setOrigin(textBounds.left + textBounds.width / 2.f, textBounds.top + textBounds.height / 2.f);
    label.setPosition(bounds.left + bounds.width / 2.f, bounds.top + bounds.height / 2.f);
    window.draw(label);
}

// --- Variables globales ---
const std::vector<std::string> monthOptions = {"Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio", "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"};
const std::vector<int> daysPerMonth = {31,28,31,30,31,30,31,31,30,31,30,31};
const std::vector<std::string> hourOptions = {"00","01","02","03","04","05","06","07","08","09","10","11","12","13","14","15","16","17","18","19","20","21","22","23"};
const std::vector<std::string> priorityOptions = {"Alta", "Media", "Baja"};
std::vector<std::string> productList;
std::vector<int> productTimes;


int selectedMonth = -1;
int selectedDay = -1;
int selectedHour = -1;
int selectedPriority = -1;
int selectedProduct = -1;
int optionstage=-1;
std::vector <std::array<int, 2>> fechas;
std::vector <std::string> options={"Hacer pedido","Ver pedidos"};
std::string clientName = "";
bool typingName = false;
int nextId = 1;

void handleDateTimeSelection(sf::RenderWindow& window, const sf::Font& font, const sf::Vector2f& mousePosition, sf::Event& event) {
    const sf::Vector2f boxSize(100.f, 50.f);
    const float spacingX = 12.f, spacingY = 12.f;

    // coordenadas del click basadas en el evento (si lo hay)
    sf::Vector2f eventMousePos = mousePosition;
    if (event.type == sf::Event::MouseButtonPressed || event.type == sf::Event::MouseButtonReleased) {
        eventMousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
    }

    if (event.type == sf::Event::MouseButtonReleased){
        clickReleased = true;
    }

    if(optionstage==-1){
        sf::Text title = makeTitle(font, "Selecciona una opcion", 28, 900.f, 60.f);
        window.draw(title);
        for(size_t i=0;i<options.size();++i){
            sf::FloatRect bounds = computeGridBounds(i, 4, boxSize, 90.f, 120.f, spacingX, spacingY);
            drawOptionBox(window, font, options[i], bounds, mousePosition, static_cast<int>(i) == optionstage);
            if (event.type == sf::Event::MouseButtonPressed && bounds.contains(eventMousePos)) {
                optionstage = static_cast<int>(i);

                // Si selecciona "Ver pedidos" (su opción con índice 1 según options)
                if (optionstage == 1) {
                    // recorre todas las fechas donde hubo pedidos
                    for (size_t p = 0; p < fechas.size(); ++p) {
                        int origMonth = fechas[p][0];
                        int origDay = fechas[p][1];

                        // obtener el día como array de 24 horas
                        auto dayHours = calendar.getDay(origMonth, origDay);

                        // enviar al "factory" como vector de 24 posiciones (cada una contiene Orders)
                        auto shipmentsByHour = factory.processDayThroughFactory(dayHours);

                        // convertir los pedidos devueltos a Package con status "delivered"
                        // y colocarlos en el calendario: día original + offset = posición del vector (aquí usamos el índice de la hora)
                        for (int h = 0; h < 24; ++h) {
                            for (const auto& order : shipmentsByHour[h]) {
                                Package pkg = Package::convertOrderToPackage(order);
                                pkg.status = "delivered";

                                int targetMonth = origMonth;
                                int targetDay = origDay + h; // offset por posición en el vector (hora)
                                // normalizar días/meses
                                while (targetMonth < 12 && targetDay >= daysPerMonth[targetMonth]) {
                                    targetDay -= daysPerMonth[targetMonth];
                                    targetMonth++;
                                }
                                // si se pasa de diciembre (mes >= 12) ignorar ese pedido
                                if (targetMonth >= 12) continue;

                                // añadir al calendario en la fecha calculada, en la misma hora h
                                calendar.addPackage(targetMonth, targetDay, h, pkg);
                            }
                        }
                    }

                    // limpiar estado de selección y volver al menú o mostrar resultados
                    fechas.clear();
                    clientName.clear();
                    typingName = false;
                    clickReleased = false;
                } else {
                    // comportamiento normal para "Hacer pedido" u otras opciones
                }

                return;
            }
        }
        return;
    }

    // 1) Seleccionar mes
    if (selectedMonth == -1) {
        const std::string titleText = optionstage == 0 ? "Selecciona un mes" : "Selecciona un mes a consultar";
        sf::Text title = makeTitle(font, titleText, 28, 900.f, 60.f);
        window.draw(title);
        for (size_t i = 0; i < monthOptions.size(); ++i) {
            sf::FloatRect bounds = computeGridBounds(i, 4, boxSize, 90.f, 120.f, spacingX, spacingY);
            drawOptionBox(window, font, monthOptions[i], bounds, mousePosition, static_cast<int>(i) == selectedMonth);
            if (event.type == sf::Event::MouseButtonPressed && bounds.contains(eventMousePos)) {
                selectedMonth = static_cast<int>(i);
                clickReleased = false;
            }
        }
        return;
    }

    // 2) Seleccionar día
    if (selectedDay == -1) {
        const std::string titleText = optionstage == 0 ? "Selecciona un dia" : "Selecciona un dia a consultar";
        sf::Text title = makeTitle(font, titleText, 28, 900.f, 60.f);
        window.draw(title);
        int days = daysPerMonth[selectedMonth];
        for (int i = 0; i < days; ++i) {
            sf::FloatRect bounds = computeGridBounds(i, 7, boxSize, 60.f, 120.f, spacingX, spacingY);
            drawOptionBox(window, font, std::to_string(i + 1), bounds, mousePosition, i == selectedDay);
            if (event.type == sf::Event::MouseButtonPressed && bounds.contains(eventMousePos)) {
                selectedDay = i;
                clickReleased = false;
            }
        }
        return;
    }

    if(optionstage==1){
        std::ostringstream list= calendar.showDay(selectedMonth,selectedDay);
        sf::Text title = makeTitle(font, "Pedidos del dia seleccionado", 26, 900.f, 60.f);
        window.draw(title);
        sf::Text listText(list.str(), font, 20);
        listText.setFillColor(sf::Color::White);
        listText.setPosition(80.f, 120.f);
        window.draw(listText);

        sf::Vector2f btnSize(200.f, 50.f);
        Button chooseAnother = makeButton(font, "Elegir otra fecha", btnSize, {340.f, 400.f});
        Button backToMenu = makeButton(font, "Menu principal", btnSize, {580.f, 400.f});
        updateButtonHover(chooseAnother, mousePosition);
        updateButtonHover(backToMenu, mousePosition);
        window.draw(chooseAnother.shape);
        window.draw(chooseAnother.text);
        window.draw(backToMenu.shape);
        window.draw(backToMenu.text);

        if (event.type == sf::Event::MouseButtonPressed) {
            if (chooseAnother.shape.getGlobalBounds().contains(eventMousePos)) {
                selectedMonth = selectedDay = -1;
                clickReleased = false;
            } else if (backToMenu.shape.getGlobalBounds().contains(eventMousePos)) {
                optionstage = -1;
                selectedMonth = selectedDay = selectedHour = selectedPriority = selectedProduct = -1;
                clientName.clear();
                typingName = false;
                clickReleased = false;
            }
        }
        return;
    }

    // 3) Seleccionar hora
    if (selectedHour == -1) {
        sf::Text title = makeTitle(font, "Selecciona una hora", 28, 900.f, 60.f);
        window.draw(title);
        for (size_t i = 0; i < hourOptions.size(); ++i) {
            sf::FloatRect bounds = computeGridBounds(i, 6, boxSize, 60.f, 120.f, spacingX, spacingY);
            drawOptionBox(window, font, hourOptions[i], bounds, mousePosition, static_cast<int>(i) == selectedHour);
            if (event.type == sf::Event::MouseButtonPressed && bounds.contains(eventMousePos)) {
                selectedHour = static_cast<int>(i);
                clickReleased = false;
            }
        }
        return;
    }

    // 4) Seleccionar prioridad
    if (selectedPriority == -1) {
        sf::Text title = makeTitle(font, "Selecciona prioridad", 28, 900.f, 60.f);
        window.draw(title);
        for (size_t i = 0; i < priorityOptions.size(); ++i) {
            sf::FloatRect bounds = computeGridBounds(i, 3, boxSize, 150.f, 150.f, spacingX, spacingY);
            drawOptionBox(window, font, priorityOptions[i], bounds, mousePosition, static_cast<int>(i) == selectedPriority);
            if (event.type == sf::Event::MouseButtonPressed && bounds.contains(eventMousePos)) {
                selectedPriority = static_cast<int>(i);
                clickReleased = false;
            }
        }
        return;
    }

    // 5) Ingresar nombre
    if (clientName.empty() || typingName) {
        sf::Text title = makeTitle(font, "Nombre del cliente:", 26, 900.f, 60.f);
        window.draw(title);

        sf::RectangleShape inputBox;
        inputBox.setSize({600.f, 50.f});
        inputBox.setPosition(150.f, 120.f);
        inputBox.setFillColor(typingName ? sf::Color(90, 90, 90) : sf::Color(70, 70, 70));
        inputBox.setOutlineThickness(2.f);
        inputBox.setOutlineColor(sf::Color::White);
        window.draw(inputBox);

        sf::Text nameText((clientName.empty() && !typingName) ? "Escribe tu nombre..." : clientName + (typingName ? "|" : ""), font, 22);
        nameText.setPosition(160.f, 130.f);
        nameText.setFillColor(clientName.empty() ? sf::Color(150, 150, 150) : sf::Color::White);
        window.draw(nameText);

        if (event.type == sf::Event::MouseButtonPressed && inputBox.getGlobalBounds().contains(eventMousePos)) {
            typingName = true;
            clickReleased = false;
        }

        if (typingName && event.type == sf::Event::TextEntered) {
            auto now = Clock::now();
            auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastKeyPress).count();
            if (ms > 100) {
                if (event.text.unicode == 8 && !clientName.empty()) clientName.pop_back();
                else if (event.text.unicode == 13 && clientName.length() >= 3) typingName = false;
                else if (event.text.unicode >= 32 && event.text.unicode < 128 && clientName.size() < 30)
                    clientName += static_cast<char>(event.text.unicode);
                lastKeyPress = now;
            }
        }
        return;
    }

    // 6) Seleccionar producto
    if (selectedProduct == -1) {
        sf::Text title = makeTitle(font, "Selecciona producto", 28, 900.f, 60.f);
        window.draw(title);
        for (size_t i = 0; i < productList.size(); ++i) {
            sf::FloatRect bounds = computeGridBounds(i, 3, boxSize, 100.f, 130.f, spacingX, spacingY);
            drawOptionBox(window, font, productList[i], bounds, mousePosition, static_cast<int>(i) == selectedProduct);
            if (event.type == sf::Event::MouseButtonPressed && bounds.contains(eventMousePos)) {
                selectedProduct = static_cast<int>(i);
                clickReleased = false;
            }
        }
        return;
    }

    // 7) Resumen y confirmación
    {
        std::ostringstream summary;
        summary << "Resumen del pedido\n\nCliente: " << clientName
                << "\nFecha: " << (selectedDay + 1) << " de " << monthOptions[selectedMonth]
                << "\nHora: " << hourOptions[selectedHour] << ":00"
                << "\nPrioridad: " << priorityOptions[selectedPriority]
                << "\nProducto: " << (selectedProduct >= 0 && selectedProduct < (int)productList.size() ? productList[selectedProduct] : "(ninguno)")
                << "\nID generado: " << nextId << "\n";

        sf::Text title = makeTitle(font, "Confirma tu pedido", 26, 900.f, 60.f);
        window.draw(title);

        sf::Text summaryText(summary.str(), font, 20);
        summaryText.setFillColor(sf::Color::White);
        summaryText.setPosition(80.f, 120.f);
        window.draw(summaryText);

        sf::Vector2f btnSize(180.f, 50.f);
        Button confirm = makeButton(font, "Confirmar", btnSize, {600.f, 400.f});
        Button cancel = makeButton(font, "Cancelar", btnSize, {380.f, 400.f});
        updateButtonHover(confirm, mousePosition);
        updateButtonHover(cancel, mousePosition);
        window.draw(confirm.shape);
        window.draw(confirm.text);
        window.draw(cancel.shape);
        window.draw(cancel.text);

        if (event.type == sf::Event::MouseButtonPressed) {
            if (confirm.shape.getGlobalBounds().contains(eventMousePos)) {
                // Confirmar pedido: guardar o procesar
                Package pkg=Package(nextId,selectedPriority,productTimes[selectedProduct],clientName,"deposited");
                calendar.addPackage(selectedMonth, selectedDay, selectedHour, pkg);
                nextId++;
                fechas.push_back({selectedMonth, selectedDay});
                // Reiniciar estado para nuevo pedido
                optionstage=selectedMonth = selectedDay = selectedHour = selectedPriority = selectedProduct = -1;
                clientName.clear();
                typingName = false;
            } else if (cancel.shape.getGlobalBounds().contains(eventMousePos)) {
                // Volver a editar: regresar a seleccionar producto
                selectedProduct = -1;
            }
            clickReleased = false;
        }
    }
}
} // namespace


int runGui() {
    sf::RenderWindow window(sf::VideoMode(900, 600), "Gestión de pedidos");
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    sf::Font font;
    if (!font.loadFromFile("assets/Roboto-Regular.ttf")) {
        std::cerr << "No se pudo cargar la fuente.\n";
        return 1;
    }

    initProducts();
    productList = storage.getProductNames();
    productTimes = storage.getProductTimes();


    sf::Event event;
    sf::Vector2f mouse;
    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            // actualizar estado de clickReleased si se soltó el botón
            if (event.type == sf::Event::MouseButtonReleased) clickReleased = true;
            mouse = window.mapPixelToCoords(sf::Mouse::getPosition(window));
            handleDateTimeSelection(window, font, mouse, event);
        }
        window.clear(sf::Color(30, 30, 30));
        // Pasar un evento vacío en la fase de dibujo para que no se re-procesen
        sf::Event noEvent;
        handleDateTimeSelection(window, font, mouse, noEvent);
        window.display();
    }
    return 0;
}
