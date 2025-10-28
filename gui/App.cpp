//App.cpp
#include "App.h"

// Macro para iniciar la aplicación
wxIMPLEMENT_APP(MyApp);

// ----------------------------------------------------------------------------
// Implementación de MyApp
// ----------------------------------------------------------------------------
bool MyApp::OnInit()
{
    // Cargar productos al inicio
    initProducts();

    // Crear la ventana principal
    MainFrame* frame = new MainFrame("Gestor de Pedidos de Fábrica", wxPoint(50, 50), wxSize(700, 500));
    frame->Show(true);
    return true;
}

// ----------------------------------------------------------------------------
// Implementación de MainFrame
// ----------------------------------------------------------------------------

MainFrame::MainFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
    : wxFrame(NULL, wxID_ANY, title, pos, size),
      m_factory(5), // Quantum de 5 horas
      m_nextId(1)
{
    // --- 1. Inicializar datos de negocio ---
    m_productList = storage.getProductNames();
    m_productTimes = storage.getProductTimes();

    const std::string mesesStr[] = {
        "Enero", "Febrero", "Marzo", "Abril", "Mayo", "Junio",
        "Julio", "Agosto", "Septiembre", "Octubre", "Noviembre", "Diciembre"
    };
    const std::string hoursStr[] = {
        "00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
        "10", "11", "12", "13", "14", "15", "16", "17", "18", "19", "20", "21", "22", "23"
    };

    for(int i=0; i<12; ++i) m_meses[i] = mesesStr[i];
    for(int i=0; i<24; ++i) m_Hours[i] = hoursStr[i];


    // --- 2. Crear Menú ---
    wxMenu* menuFile = new wxMenu;
    menuFile->Append(wxID_EXIT, "Salir");
    wxMenuBar* menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&Archivo");
    SetMenuBar(menuBar);

    // --- 3. Crear Notebook (Pestañas) ---
    wxNotebook* notebook = new wxNotebook(this, wxID_ANY);

    // --- 4. Pestaña 1: Hacer Pedido ---
    wxPanel* panelOrder = new wxPanel(notebook, wxID_ANY);
    wxFlexGridSizer* formSizer = new wxFlexGridSizer(2, 10, 10); // 2 columnas, con gaps

    // Producto
    formSizer->Add(new wxStaticText(panelOrder, wxID_ANY, "Producto:"), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    wxArrayString productChoices;
    for(const auto& prod : m_productList) productChoices.Add(prod);
    m_comboProduct = new wxComboBox(panelOrder, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, productChoices, wxCB_READONLY);
    formSizer->Add(m_comboProduct, 1, wxEXPAND);

    // Fecha (Mes, Día, Hora)
    formSizer->Add(new wxStaticText(panelOrder, wxID_ANY, "Fecha:"), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    wxBoxSizer* dateSizer = new wxBoxSizer(wxHORIZONTAL);
    m_comboMonth = new wxComboBox(panelOrder, ID_ComboMonth, "", wxDefaultPosition, wxDefaultSize, 12, m_meses, wxCB_READONLY);
    m_spinDay = new wxSpinCtrl(panelOrder, wxID_ANY, "", wxDefaultPosition, wxSize(60, -1), wxSP_ARROW_KEYS, 1, 31, 1);
    m_comboHour = new wxComboBox(panelOrder, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 24, m_Hours, wxCB_READONLY);
    dateSizer->Add(m_comboMonth, 1, wxEXPAND | wxRIGHT, 5);
    dateSizer->Add(m_spinDay, 0, wxEXPAND | wxRIGHT, 5);
    dateSizer->Add(m_comboHour, 0, wxEXPAND);
    formSizer->Add(dateSizer, 1, wxEXPAND);
    
    // Prioridad
    wxString priorityChoices[] = {"1=Alta", "2=Media", "3=Baja"};
    formSizer->Add(new wxStaticText(panelOrder, wxID_ANY, "Prioridad:"), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    m_comboPriority = new wxComboBox(panelOrder, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, 3, priorityChoices, wxCB_READONLY);
    formSizer->Add(m_comboPriority, 1, wxEXPAND);

    // Cliente
    formSizer->Add(new wxStaticText(panelOrder, wxID_ANY, "Cliente:"), 0, wxALIGN_CENTER_VERTICAL | wxALIGN_RIGHT);
    m_textClient = new wxTextCtrl(panelOrder, wxID_ANY, "");
    formSizer->Add(m_textClient, 1, wxEXPAND);

    // Botón de Enviar
    m_btnSubmitOrder = new wxButton(panelOrder, ID_SubmitOrder, "Agregar Pedido");
    m_statusTextOrder = new wxStaticText(panelOrder, wxID_ANY, "Complete el formulario para agregar un pedido.");

    wxBoxSizer* orderMainSizer = new wxBoxSizer(wxVERTICAL);
    orderMainSizer->Add(formSizer, 0, wxEXPAND | wxALL, 10);
    orderMainSizer->Add(m_btnSubmitOrder, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    orderMainSizer->Add(m_statusTextOrder, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    panelOrder->SetSizer(orderMainSizer);

    // --- 5. Pestaña 2: Ver Pedidos y Procesar ---
    wxPanel* panelView = new wxPanel(notebook, wxID_ANY);
    wxBoxSizer* viewMainSizer = new wxBoxSizer(wxVERTICAL);

    m_btnProcessOrders = new wxButton(panelView, ID_ProcessOrders, "Procesar Pedidos Pendientes");
    m_statusTextProcess = new wxStaticText(panelView, wxID_ANY, "No hay pedidos nuevos para procesar.");
    viewMainSizer->Add(m_btnProcessOrders, 0, wxEXPAND | wxALL, 10);
    viewMainSizer->Add(m_statusTextProcess, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);
    
    viewMainSizer->Add(new wxStaticText(panelView, wxID_ANY, "Fechas con pedidos:"), 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
    m_listDates = new wxListBox(panelView, wxID_ANY);
    viewMainSizer->Add(m_listDates, 1, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    wxBoxSizer* viewBtnSizer = new wxBoxSizer(wxHORIZONTAL);
    m_btnViewDate = new wxButton(panelView, ID_ViewDate, "Ver Fecha Seleccionada");
    m_btnViewAll = new wxButton(panelView, ID_ViewAll, "Ver Todas las Fechas");
    viewBtnSizer->Add(m_btnViewDate, 1, wxEXPAND | wxRIGHT, 5);
    viewBtnSizer->Add(m_btnViewAll, 1, wxEXPAND | wxLEFT, 5);
    viewMainSizer->Add(viewBtnSizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    m_textOrderDisplay = new wxTextCtrl(panelView, wxID_ANY, "", wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_DONTWRAP | wxTE_RICH);
    m_textOrderDisplay->SetFont(wxFont(10, wxFONTFAMILY_TELETYPE, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    viewMainSizer->Add(m_textOrderDisplay, 3, wxEXPAND | wxALL, 10);

    panelView->SetSizer(viewMainSizer);

    // --- 6. Añadir pestañas al Notebook ---
    notebook->AddPage(panelOrder, "Hacer Pedido");
    notebook->AddPage(panelView, "Ver Pedidos");

    // --- 7. Bindear Eventos ---
    Bind(wxEVT_MENU, &MainFrame::OnExit, this, wxID_EXIT);
    Bind(wxEVT_COMBOBOX, &MainFrame::OnMonthChange, this, ID_ComboMonth);
    Bind(wxEVT_BUTTON, &MainFrame::OnSubmitOrder, this, ID_SubmitOrder);
    Bind(wxEVT_BUTTON, &MainFrame::OnProcessOrders, this, ID_ProcessOrders);
    Bind(wxEVT_BUTTON, &MainFrame::OnViewDate, this, ID_ViewDate);
    Bind(wxEVT_BUTTON, &MainFrame::OnViewAll, this, ID_ViewAll);
}

// ----------------------------------------------------------------------------
// Implementación de Ayudantes y Eventos
// ----------------------------------------------------------------------------

// Copiada de tu main
bool MainFrame::fechasContains(const std::vector<std::array<int, 2>>& fechas, int m, int d)
{
    for (auto& f : fechas) if (f[0] == m && f[1] == d) return true;
    return false;
}

// Ayudante para refrescar la lista de fechas
void MainFrame::UpdateDatesList()
{
    m_listDates->Clear();
    for (const auto& f : m_fechas)
    {
        m_listDates->Append(wxString::Format("%s/%d", m_meses[f[0]], f[1] + 1));
    }
}

// --- Manejadores de Eventos ---

void MainFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MainFrame::OnMonthChange(wxCommandEvent& event)
{
    int monthIdx = m_comboMonth->GetSelection();
    if (monthIdx != wxNOT_FOUND)
    {
        // Ajustar rango del día. Asumimos no-bisiesto como en tu main.
        m_spinDay->SetRange(1, m_daysPerMonth[monthIdx]);
    }
}

void MainFrame::OnSubmitOrder(wxCommandEvent& event)
{
    // --- 1. Validar Entradas ---
    int pidx = m_comboProduct->GetSelection();
    int month = m_comboMonth->GetSelection();
    int day = m_spinDay->GetValue() - 1; // 0-based
    int hour = m_comboHour->GetSelection();
    int priorityIdx = m_comboPriority->GetSelection();
    wxString client = m_textClient->GetValue();

    if (pidx == wxNOT_FOUND || month == wxNOT_FOUND || hour == wxNOT_FOUND ||
        priorityIdx == wxNOT_FOUND || client.IsEmpty())
    {
        wxMessageBox("Por favor, complete todos los campos.", "Error de Entrada", wxOK | wxICON_ERROR);
        return;
    }
    int priority = priorityIdx + 1; // 0,1,2 -> 1,2,3

    // --- 2. Lógica de Negocio (Copiada de tu main) ---
    Package pkg;
    pkg.id = m_nextId++;
    pkg.urgency = priority;
    pkg.fabricationTime = m_productTimes[pidx];
    pkg.client = client.ToStdString(); // Convertir wxString a std::string
    pkg.status = "deposited";

    m_processPackageIds.insert(pkg.id);
    m_calendar.addPackage(month, day, hour, pkg);

    if (!fechasContains(m_fechas, month, day)) m_fechas.push_back({month, day});
    if (!fechasContains(m_registereddates, month, day)) m_registereddates.push_back({month, day});

    // --- 3. Feedback al Usuario ---
    m_statusTextOrder->SetLabel(wxString::Format("Pedido agregado: id=%d | Cliente: %s | Fecha=%s/%d - %s:00",
        pkg.id, client, m_meses[month], day + 1, m_Hours[hour]));
    
    m_statusTextProcess->SetLabel(wxString::Format("%d pedidos pendientes de procesar.", (int)m_processPackageIds.size()));

    // Limpiar formulario
    m_comboProduct->SetSelection(wxNOT_FOUND);
    m_comboMonth->SetSelection(wxNOT_FOUND);
    m_spinDay->SetValue(1);
    m_comboHour->SetSelection(wxNOT_FOUND);
    m_comboPriority->SetSelection(wxNOT_FOUND);
    m_textClient->SetValue("");
}

void MainFrame::OnProcessOrders(wxCommandEvent& event)
{
    // --- 1. Validar (Copiado de tu main) ---
    if (m_registereddates.empty())
    {
        wxMessageBox("No hay pedidos nuevos para procesar.", "Información", wxOK | wxICON_INFORMATION);
        return;
    }
    m_statusTextProcess->SetLabel(wxString::Format("Procesando %d pedidos...", (int)m_processPackageIds.size()));

    // --- 2. Lógica de Procesamiento (Copiada de tu main) ---
    for (auto& fd : m_registereddates) {
        int m = fd[0], d = fd[1];
        auto orders = m_calendar.getDay(m, d);

        std::array<Hour, 24> toProcess;
        for (int h = 0; h < 24; ++h) {
            for (const auto& itemPtr : orders[h]) {
                if (!itemPtr) continue;
                if (auto pkg = std::dynamic_pointer_cast<Package>(itemPtr)) {
                    if (pkg->status != "delivered" && m_processPackageIds.find(pkg->id) != m_processPackageIds.end()) {
                        toProcess[h].push_back(itemPtr);
                    }
                }
            }
        }

        auto processing = m_factory.processDayThroughFactory(toProcess);
        m_factory.receiveOrders(processing);
        m_factory.process();
        auto completed = m_factory.getAllBatchesAsVector();
        m_factory.clear();

        for (size_t b = 0; b < completed.size(); ++b) {
            for (auto& o : completed[b]) {
                Package pkg = Package::convertOrderToPackage(o);
                m_calendar.addPackage(m, d, 23, pkg); // Añadir al final del día
                m_processPackageIds.erase(pkg.id); // marcar como procesado
                if (!fechasContains(m_fechas, m, d)) m_fechas.push_back({m, d});
            }

            d++;
            if (d >= m_daysPerMonth[m]) {
                d = 0;
                m++;
                if (m >= 12) break; 
            }
        }
    }
    m_registereddates.clear(); // ya se procesaron

    // --- 3. Feedback y Actualización de GUI ---
    m_statusTextProcess->SetLabel(wxString::Format("Procesamiento completo. %d pedidos restantes.", (int)m_processPackageIds.size()));
    UpdateDatesList();
    m_textOrderDisplay->SetValue("Se han procesado los pedidos.\nSeleccione una fecha para ver los detalles actualizados.");
}

void MainFrame::OnViewDate(wxCommandEvent& event)
{
    int choice = m_listDates->GetSelection();
    if (choice == wxNOT_FOUND)
    {
        wxMessageBox("Por favor, seleccione una fecha de la lista.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    std::array<int, 2> fd = m_fechas[choice];
    std::ostringstream oss = m_calendar.showDay(fd[0], fd[1]);
    m_textOrderDisplay->SetValue(oss.str()); // wxTextCtrl acepta std::string
}

void MainFrame::OnViewAll(wxCommandEvent& event)
{
    if (m_fechas.empty())
    {
        m_textOrderDisplay->SetValue("No hay pedidos para mostrar.");
        return;
    }

    std::ostringstream oss;
    for (auto& fd : m_fechas)
    {
        oss << m_calendar.showDay(fd[0], fd[1]).str();
        oss << "\n==========================================\n\n";
    }
    m_textOrderDisplay->SetValue(oss.str());
}
