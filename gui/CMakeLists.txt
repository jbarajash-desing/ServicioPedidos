cmake_minimum_required(VERSION 3.10)
project(FactoryManager)

# --- Configuración del Estándar C++ ---
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Encontrar wxWidgets ---
find_package(wxWidgets REQUIRED COMPONENTS core base)
include(${wxWidgets_USE_FILE})

# --- Definir tu Ejecutable ---
# AÑADE TODOS TUS ARCHIVOS .cpp AQUÍ
add_executable(FactoryApp 
    App.cpp
    ../lib/factory/src/factory.cpp
    ../lib/products/src/products.cpp
    ../lib/calendar/src/calendar.cpp
)

# --- Incluir Directorios ---
# (Esto le dice al compilador dónde encontrar los .hpp)
target_include_directories(FactoryApp PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/factory/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/products/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../lib/calendar/include
)

# --- Enlazar Librerías ---
# (Esto ahora solo enlaza bibliotecas reales, como wxWidgets)
target_link_libraries(FactoryApp
    PRIVATE
    ${wxWidgets_LIBRARIES}
)

# --- Configuración Específica de Plataforma (para macOS) ---
if(APPLE)
    target_compile_definitions(FactoryApp PRIVATE "__WXMAC__")
endif()