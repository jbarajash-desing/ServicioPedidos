//App.h
#pragma once

// --- Includes de wxWidgets ---
#include <wx/wx.h>
#include <wx/notebook.h>
#include <wx/spinctrl.h>
#include <wx/listbox.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/sizer.h>

// --- Includes de tu Lógica ---
#include <string>
#include <vector>
#include <array>
#include <memory>
#include <algorithm>
#include <sstream>
#include <set>

#include "../lib/factory/include/factory.hpp"
#include "../lib/products/include/products.hpp"
#include "../lib/calendar/include/calendar.hpp"

// Declaración externa (igual que en tu main)
extern void initProducts();

// ----------------------------------------------------------------------------
// Clase App: El punto de entrada de la aplicación wxWidgets
// ----------------------------------------------------------------------------
class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};

// ----------------------------------------------------------------------------
// Clase MainFrame: La ventana principal de la aplicación
// ----------------------------------------------------------------------------
class MainFrame : public wxFrame
{
public:
    MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
    // --- Lógica de Negocio (antes en mainTerminal) ---
    Calendar m_calendar;
    Factory m_factory;
    std::vector<std::string> m_productList;
    std::vector<int> m_productTimes;
    std::vector<std::array<int, 2>> m_fechas;
    std::vector<std::array<int, 2>> m_registereddates;
    std::set<int> m_processPackageIds;
    int m_nextId;

    // Constantes (adaptadas para wx)
    const std::vector<int> m_daysPerMonth = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    wxString m_meses[12];
    wxString m_Hours[24];

    // --- Controles de la Pestaña "Hacer Pedido" ---
    wxComboBox* m_comboProduct;
    wxComboBox* m_comboMonth;
    wxSpinCtrl* m_spinDay;
    wxComboBox* m_comboHour;
    wxComboBox* m_comboPriority;
    wxTextCtrl* m_textClient;
    wxButton* m_btnSubmitOrder;
    wxStaticText* m_statusTextOrder;

    // --- Controles de la Pestaña "Ver Pedidos" ---
    wxButton* m_btnProcessOrders;
    wxStaticText* m_statusTextProcess;
    wxListBox* m_listDates;
    wxButton* m_btnViewDate;
    wxButton* m_btnViewAll;
    wxTextCtrl* m_textOrderDisplay;

    // --- Manejadores de Eventos (Event Handlers) ---
    void OnExit(wxCommandEvent& event);
    void OnMonthChange(wxCommandEvent& event);
    void OnSubmitOrder(wxCommandEvent& event);
    void OnProcessOrders(wxCommandEvent& event);
    void OnViewDate(wxCommandEvent& event);
    void OnViewAll(wxCommandEvent& event);

    // --- Funciones de Ayuda ---
    bool fechasContains(const std::vector<std::array<int, 2>>& fechas, int m, int d);
    void UpdateDatesList();

    // Enum para IDs de controles
    enum
    {
        ID_ComboMonth = 1,
        ID_SubmitOrder,
        ID_ProcessOrders,
        ID_ViewDate,
        ID_ViewAll
    };
};
