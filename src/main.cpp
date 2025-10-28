#ifdef USE_GUI

#include <wx/wx.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include "../lib/factory/include/factory.hpp"
#include "../lib/products/include/products.hpp"
#include "../lib/calendar/include/calendar.hpp"

// Declaración de la función que ya existe
int mainTerminal();

// Aplicación principal
class PedidoApp : public wxApp {
public:
    virtual bool OnInit();
};

// Ventana principal
class MainFrame : public wxFrame {
public:
    MainFrame(const wxString& title);

private:
    void OnHacerPedido(wxCommandEvent& event);
    void OnVerPedidos(wxCommandEvent& event);
    void OnSalir(wxCommandEvent& event);
};

// Implementación del frame
MainFrame::MainFrame(const wxString& title)
    : wxFrame(nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(400, 250))
{
    wxPanel* panel = new wxPanel(this);

    wxBoxSizer* vbox = new wxBoxSizer(wxVERTICAL);

    wxStaticText* titleText = new wxStaticText(panel, wxID_ANY, "Servicio de Pedidos");
    wxFont font = titleText->GetFont();
    font.SetPointSize(14);
    font.SetWeight(wxFONTWEIGHT_BOLD);
    titleText->SetFont(font);

    wxButton* btnPedido = new wxButton(panel, wxID_ANY, "Hacer pedido");
    wxButton* btnVerPedidos = new wxButton(panel, wxID_ANY, "Ver pedidos");
    wxButton* btnSalir = new wxButton(panel, wxID_EXIT, "Salir");

    vbox->Add(titleText, 0, wxALIGN_CENTER | wxTOP | wxBOTTOM, 20);
    vbox->Add(btnPedido, 0, wxEXPAND | wxALL, 10);
    vbox->Add(btnVerPedidos, 0, wxEXPAND | wxALL, 10);
    vbox->Add(btnSalir, 0, wxEXPAND | wxALL, 10);

    panel->SetSizer(vbox);

    // Eventos
    btnPedido->Bind(wxEVT_BUTTON, &MainFrame::OnHacerPedido, this);
    btnVerPedidos->Bind(wxEVT_BUTTON, &MainFrame::OnVerPedidos, this);
    btnSalir->Bind(wxEVT_BUTTON, &MainFrame::OnSalir, this);
}

void MainFrame::OnHacerPedido(wxCommandEvent& event) {
    wxMessageBox("Ejecutando modo terminal para hacer pedido...", "Info", wxOK | wxICON_INFORMATION);
    mainTerminal();
}

void MainFrame::OnVerPedidos(wxCommandEvent& event) {
    wxMessageBox("Ejecutando modo terminal para ver pedidos...", "Info", wxOK | wxICON_INFORMATION);
    mainTerminal();
}

void MainFrame::OnSalir(wxCommandEvent& event) {
    Close(true);
}

// Inicialización de la aplicación
bool PedidoApp::OnInit() {
    MainFrame* frame = new MainFrame("Servicio de Pedidos");
    frame->Show(true);
    return true;
}

wxIMPLEMENT_APP(PedidoApp);

#endif

