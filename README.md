# 📦 Sistema de Gestión de Pedidos – Proyecto de Estructuras de Datos

## 🧩 Descripción general
Este proyecto implementa un **sistema de gestión de pedidos** desarrollado en **C++**, diseñado para simular el registro, almacenamiento y procesamiento de pedidos a lo largo del año.  
El sistema fue construido siguiendo principios de **estructuras de datos** vistas en clase: listas, colas, colas con prioridad y arreglos multidimensionales.

---

## ⚙️ Características principales
- Estructura modular con librerías separadas:
  - `lib/calendar/` → Maneja los días, meses y horas del calendario.  
  - `lib/factory/` → Controla la recepción y procesamiento de pedidos.  
  - `lib/products/` → Gestiona los productos disponibles y su información.
- Registro de pedidos con:
  - Cliente  
  - ID único  
  - Fecha (año, mes, día, hora)  
  - Estado (`depositado` o `entregado`)
- Uso de **listas** para almacenar pedidos diarios y **arreglos** para organizar la información anual.
- Ejecución completamente por terminal.

---

## 🧱 Compilación
Asegúrate de tener instalado **g++** y **make**.  
Para compilar el proyecto:

```bash
make
```
## 📊[Presentación](https://www.canva.com/design/DAG3ASmarns/QmnYjO-q_h9pp_3mQSYIAA/edit?utm_content=DAG3ASmarns&utm_campaign=designshare&utm_medium=link2&utm_source=sharebutton)
