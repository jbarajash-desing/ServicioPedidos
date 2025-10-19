#ifndef CALENDARIO_PAQUETES_H
#define CALENDARIO_PAQUETES_H

#include <iostream>
#include <list>
using namespace std;

struct Paquete {
    int id;
    string cliente;
    string estado; // "depositado" o "entregado"
};

using hora = list<Paquete>; // usamos lista

class Calendario {
private:
    int DiasPorMes[12] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
    hora calendario[12][31][24]; // [mes][día][hora] → lista de paquetes

public:
    Calendario() = default;

    void agregarPaquete(int mes, int dia, int hora, const Paquete& p) {
        calendario[mes][dia][hora].push_back(p);
    }

    void mostrarPaquetesDelDia(int mes, int dia) const {
        list<Paquete> depositados, entregados;

        for (int h = 0; h < 24; ++h) {
            for (const auto& p : calendario[mes][dia][h]) {
                if (p.estado == "depositado")
                    depositados.push_back(p);
                else if (p.estado == "entregado")
                    entregados.push_back(p);
            }
        }

        cout << "\nDepositados:\n";
        for (const auto& p : depositados)
            cout << "ID: " << p.id << " Cliente: " << p.cliente << endl;

        cout << "\nEntregados:\n";
        for (const auto& p : entregados)
            cout << "ID: " << p.id << " Cliente: " << p.cliente << endl;
    }
};

#endif
/*
Esta libreria local usa arreglos estáticos fijos en tamaño: 12 meses, 31 días, 24 horas.
Usa listas dinámicas (std::list) para manejar múltiples paquetes en cada hora.
*/
