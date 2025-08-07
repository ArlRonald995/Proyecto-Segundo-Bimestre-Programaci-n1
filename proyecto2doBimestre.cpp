#include <iostream>
#include <vector>
#include <string>
#include <cstdio>     // Para FILE* y popen
#include <fstream>    // Para crear archivo .dat
#include <algorithm>
#include <cmath>

using namespace std;

// === Variables globales ===
double precio_mesa = 0.0;
double precio_silla = 0.0;
vector<vector<double>> restricciones;

// NUEVAS: Para graficar punto óptimo
double optimo_x = -1;
double optimo_y = -1;

struct Punto {
    double x, y;
};

// Muestra el menú principal
void mostrarMenu() {
    cout << "\n=== MENÚ PRINCIPAL ===" << endl;
    cout << "1. Ingresar precios de venta" << endl;
    cout << "2. Ingresar restricciones de produccion" << endl;
    cout << "3. Mostrar funcion de ganancia" << endl;
    cout << "4. Calcular solucion optima" << endl;
    cout << "5. Graficar region y lineas de nivel (Gnuplot)" << endl;
    cout << "6. Salir" << endl;
    cout << "======================" << endl;
}

// Opción 1: Ingresar precios de mesas y sillas
void opcion1() {
    cout << "\n--- INGRESO DE PRECIOS ---" << endl;
    
    // Pedir precio de mesa con validación
    while (true) {
        cout << "Precio por mesa (USD): ";
        if (cin >> precio_mesa && precio_mesa > 0) break;
        cin.clear(); cin.ignore(1000, '\n');
        cout << "Error: Ingrese un numero positivo" << endl;
    }
    
    // Pedir precio de silla con validación
    while (true) {
        cout << "Precio por silla (USD): ";
        if (cin >> precio_silla && precio_silla > 0) break;
        cin.clear(); cin.ignore(1000, '\n');
        cout << "Error: Ingrese un numero positivo" << endl;
    }
    
    // Mostrar confirmación
    cout.precision(2);
    cout << fixed;
    cout << "\nPrecios registrados:\nMesas = USD " << precio_mesa << "\nSillas = USD " << precio_silla << endl;
}

// Opción 2: Ingresar restricciones de producción
void opcion2() {
    cout << "\n--- INGRESO DE RESTRICCIONES ---" << endl;
    cout << "Formato: coeficienteX1 coeficienteX2 valorTotal\n";
    cout << "Ejemplo: 4 3 240 (para 4x1 + 3x2 <= 240)\n";
    cout << "Escriba 'fin' para terminar\n";

    cin.ignore();//Limpiar el búfer de entrada
    while (true) {
        cout << "\nIngrese restriccion: ";
        string entrada;
        getline(cin, entrada);
        if (entrada == "fin") break;
        double x1, x2, total;
        if (sscanf(entrada.c_str(), "%lf %lf %lf", &x1, &x2, &total) == 3) {
            if (x1 >= 0 && x2 >= 0 && total >= 0) {
                restricciones.push_back({x1, x2, total});
                cout << "Agregada: " << x1 << "x1 + " << x2 << "x2 <= " << total << endl;
            } else {
                cout << "Error: Valores deben ser positivos" << endl;
            }
        } else {
            cout << "Error: Formato incorrecto. Use 3 numeros separados por espacios" << endl;
        }
    }
    
    // Mostrar todas las restricciones
    cout << "\nRestricciones registradas:" << endl;
    for (auto& r : restricciones) {
        cout << r[0] << "x1 + " << r[1] << "x2 <= " << r[2] << endl;
    }
}

// Opción 3: Mostrar la función de ganancia
void opcion3() {
    cout << "\n--- FUNCION DE GANANCIA ---" << endl;
    if (precio_mesa > 0 && precio_silla > 0) {
        cout << "Maximizar Z = " << precio_mesa << "x1 + " << precio_silla << "x2" << endl;
    } else {
        cout << "Error: Primero ingrese los precios (Opcion 1)" << endl;
    }
}

// Opción 4: Calcular solución óptima
void opcion4() {
    cout << "\n--- CALCULO DE LA SOLUCION OPTIMA EN INTERSECCIONES ---" << endl;
    if (precio_mesa == 0 || precio_silla == 0 || restricciones.size() < 2) {
        cout << "Error: Faltan datos. Use opciones 1 y 2 primero." << endl;
        return;
    }
    
    vector<vector<double>> intersecciones;

    // Calcular intersecciones entre cada par de restricciones
    for (int i = 0; i < restricciones.size(); i++) {
        for (int j = i + 1; j < restricciones.size(); j++) {
            double a1 = restricciones[i][0], b1 = restricciones[i][1], c1 = restricciones[i][2];
            double a2 = restricciones[j][0], b2 = restricciones[j][1], c2 = restricciones[j][2];
            double det = a1 * b2 - a2 * b1;
            if (fabs(det) > 1e-9) {
                double x1 = (c1 * b2 - c2 * b1) / det;
                double x2 = (a1 * c2 - a2 * c1) / det;
                if (x1 >= 0 && x2 >= 0) {
                    intersecciones.push_back({x1, x2});
                }
            }
        }
    }

    if (intersecciones.empty()) {
        cout << "No se encontraron intersecciones factibles entre restricciones." << endl;
        return;
    }
    // Encontrar el mejor punto
    double mejor_z = -1;
    int mejor_punto = -1;

    cout << "\nEvaluando intersecciones:" << endl;
    for (int i = 0; i < intersecciones.size(); i++) {
        bool valido = true;
        double x = intersecciones[i][0];
        double y = intersecciones[i][1];
        for (int j = 0; j < restricciones.size(); j++) {
            double val = restricciones[j][0] * x + restricciones[j][1] * y;
            if (val > restricciones[j][2] + 0.001) {
                valido = false;
                break;
            }
        }
        if (valido) {
            double z = precio_mesa * x + precio_silla * y;
            cout << "(" << x << ", " << y << ") -> Z = " << z << endl;
            if (z > mejor_z) {
                mejor_z = z;
                mejor_punto = i;
            }
        }
    }

    if (mejor_punto >= 0) {
        optimo_x = intersecciones[mejor_punto][0];
        optimo_y = intersecciones[mejor_punto][1];
        cout << "\n=== SOLUCION OPTIMA EN INTERSECCION ===" << endl;
        cout << "Mesas (x1) = " << optimo_x << endl;
        cout << "Sillas (x2) = " << optimo_y << endl;
        cout << "Ganancia maxima = " << mejor_z << endl;
    } else {
        cout << "No se encontro solucion factible en las intersecciones." << endl;
    }
}

// Opción 5: Graficar la región factible y las líneas de nivel
void opcion5() {
    //Verifica que haya al menos 2 restricciones y un punto optimo
    if (restricciones.size() < 2 || optimo_x < 0 || optimo_y < 0) {
        cout << "Error: primero calcule la solución óptima con la opción 4." << endl;
        return;
    }
    //Verifica si las restricciones se cruzan
    double a1 = restricciones[0][0], b1 = restricciones[0][1], c1 = restricciones[0][2];
    double a2 = restricciones[1][0], b2 = restricciones[1][1], c2 = restricciones[1][2];

    double det = a1 * b2 - a2 * b1;
    if (det == 0) {
        cout << "Las restricciones 1 y 2 son paralelas, no se puede graficar la región correctamente." << endl;
        return;
    }
    //Calcula el punto de interseccion
    double x_inter = (c1 * b2 - c2 * b1) / det;
    double y_inter = (c1 * a2 - c2 * a1) / (b1 * a2 - b2 * a1);
    
    //interseccion entre las dos restricciones
    vector<Punto> candidatos = {
        {0, 0},
        {(a1 != 0) ? c1 / a1 : 0, 0},
        {0, (b1 != 0) ? c1 / b1 : 0},
        {(a2 != 0) ? c2 / a2 : 0, 0},
        {0, (b2 != 0) ? c2 / b2 : 0},
        {x_inter, y_inter}
    };
    //Verifica si un punto satisface las dos restricciones
    auto cumple = [&](const Punto& punto) {
        return (a1 * punto.x + b1 * punto.y <= c1 + 1e-9) &&
               (a2 * punto.x + b2 * punto.y <= c2 + 1e-9) &&
               punto.x >= -1e-9 && punto.y >= -1e-9;
    };
    //Filtra los puntos que cumplen las restricciones
    vector<Punto> region;
    for (auto& punto_actual : candidatos) {
        if (cumple(punto_actual)) region.push_back(punto_actual);
    }
    //Ordena los puntos para formar un poligono
    Punto centro = {0, 0};
    for (auto& punto_actual : region) {
        centro.x += punto_actual.x;
        centro.y += punto_actual.y;
    }
    centro.x /= region.size();
    centro.y /= region.size();
    //Se calcula el centro del poligono
    auto compararAngulos = [&](const Punto& a, const Punto& b) {
        return atan2(a.y - centro.y, a.x - centro.x) < atan2(b.y - centro.y, b.x - centro.x);
    };

    sort(region.begin(), region.end(), compararAngulos);

    region.push_back(region[0]);
    

    //Prepara las instrucciones para Gnuplot
    ofstream data("region.dat");
    for (auto& punto : region) {
        data << punto.x << " " << punto.y << "\n";
    }
    data.close();
    
    //Calcular las lineas del nivel de ganancia
    double Z_optima = precio_mesa * optimo_x + precio_silla * optimo_y;
    double Z1 = Z_optima - 5;
    double Z2 = Z_optima + 5;
    
    //Ajusta los ejes de la gráfica
    double x_max = optimo_x;
    double y_max = optimo_y;
    for (auto& punto_actual : region) {
        if (punto_actual.x > x_max) x_max = punto_actual.x;
        if (punto_actual.y > y_max) y_max = punto_actual.y;
    }
    x_max *= 1.2; if (x_max < 10) x_max = 10;
    y_max *= 1.2; if (y_max < 10) y_max = 10;
    
    //Popen se comunica con Gnuplot
    FILE* gnuplot = popen("gnuplot -persistent", "w");
    fprintf(gnuplot, "set title 'Programacion lineal con lineas de nivel'\n");
    fprintf(gnuplot, "set xlabel 'x1'\n");
    fprintf(gnuplot, "set ylabel 'x2'\n");
    fprintf(gnuplot, "set xrange [0:%f]\n", x_max);
    fprintf(gnuplot, "set yrange [0:%f]\n", y_max);
    fprintf(gnuplot, "set style fill transparent solid 0.3\n");
    fprintf(gnuplot, "set key outside\n");
   
    //Dibujar todo lo importante para la funcion
    fprintf(gnuplot,
        "plot \\\n"
        "'region.dat' with filledcurves fs solid 0.3 lc rgb 'yellow' title 'Region factible', \\\n"
        "(%f - %f*x)/%f with lines lw 2 lc rgb 'blue' title '%.0fx1 + %.0fx2 <= %.0f', \\\n"
        "(%f - %f*x)/%f with lines lw 2 lc rgb 'green' title '%.0fx1 + %.0fx2 <= %.0f', \\\n"
        "(%f - %f*x)/%f with lines dt 2 lc rgb 'red' title 'Z = %.2fx1 + %.2fx2 = %.2f', \\\n"
        "(%f - %f*x)/%f with lines dt 2 lc rgb 'red' title 'Z = %.2fx1 + %.2fx2 = %.2f', \\\n"
        "'-' with points pt 7 ps 1.5 lc rgb 'black' title 'Punto optimo (%.2f, %.2f)'\n", //Punto en las coordenadas exactas del punto óptimo
        c1, a1, b1, a1, b1, c1,
        c2, a2, b2, a2, b2, c2,
        Z1, precio_mesa, precio_silla, precio_mesa, precio_silla, Z1,
        Z2, precio_mesa, precio_silla, precio_mesa, precio_silla, Z2,
        optimo_x, optimo_y
    );

    //Finaliza la comunicación con Gnuplot
    fprintf(gnuplot, "%.6f %.6f\n", optimo_x, optimo_y);
    fprintf(gnuplot, "e\n");
    pclose(gnuplot);
}

int main() {
    int opcion;
    do {
        mostrarMenu();
        cout << "Seleccione opcion (1-6): ";
        while (!(cin >> opcion)) {
            cin.clear();
            cin.ignore(1000, '\n');
            cout << "Error: ingrese un numero del 1 al 6\n";
            cout << "Seleccione opcion (1-6): ";
        }
        switch (opcion) {
            case 1: opcion1(); break;
            case 2: opcion2(); break;
            case 3: opcion3(); break;
            case 4: opcion4(); break;
            case 5: opcion5(); break;
            case 6: cout << "\nSaliendo del programa...\n"; break;
            default: cout << "Opción no valida\n";
        }
    } while (opcion != 6);
    return 0;
}