#include <iostream>
#include <vector>
#include <string>
#include <cstdio>     // Para FILE* y popen
#include <fstream>    // Para crear archivo .dat
#include <algorithm>
#include <cmath>
#include "declaraciones.h" // Incluye las declaraciones de funciones

using namespace std;

// === Variables globales ===
double precioMesa = 0.0;
double precioSilla = 0.0;
vector<vector<double>> restricciones; // Almacena restricciones como {coeficienteX1, coeficienteX2, valorTotal}

// NUEVAS: Para graficar punto óptimo
double optimoX = -1;
double optimoY = -1;

// Estructura para representar un punto en el plano
struct Punto {
    double x, y;
};

// main: Punto de entrada del programa
int main() {
    try {
        int opcion;
        do {
            mostrarMenu();
            cout << "Seleccione opcion (1-6): ";
            while (!(cin >> opcion)) {
                cin.clear();
                cin.ignore(1000, '\n');
                cout << "Error: ingrese un numero del 1 al 6\n";
            }
            switch (opcion) {
                case 1: ingresoPreciodeVenta(); break;
                case 2: ingresoRestriccionesDeProduccion(); break;
                case 3: mostrarFuncionGanancia(); break;
                case 4: calcularSolucionOptima(); break;
                case 5: graficarSolucion(); break;
                case 6: cout << "\nSaliendo del programa...\n"; break;
                default: cout << "Opcion no valida\n";
            }
        } while (opcion != 6);
    } catch (const exception& e) {
        cerr << "Error inesperado: " << e.what() << endl;
    }
    return 0;
}

// Muestra el menú principal
void mostrarMenu() {
    cout << "\n=== MENU PRINCIPAL ===" << endl;
    cout << "1. Ingresar precios de venta" << endl;
    cout << "2. Ingresar restricciones de produccion" << endl;
    cout << "3. Mostrar funcion de ganancia" << endl;
    cout << "4. Calcular solucion optima" << endl;
    cout << "5. Graficar region y lineas de nivel (Gnuplot)" << endl;
    cout << "6. Salir" << endl;
    cout << "======================" << endl;
}

// Opción 1: Ingresar precios de mesas y sillas
void ingresoPreciodeVenta() {
    try {
        cout << "\n--- INGRESO DE PRECIOS ---" << endl;

        // Pedir precio de mesa con validación
        cout << "Precio por mesa (USD): ";
        if (!(cin >> precioMesa) || precioMesa <= 0) { 
            throw invalid_argument("El precio de la mesa debe ser un numero positivo.");
        }

        // Pedir precio de silla con validación
        cout << "Precio por silla (USD): ";
        if (!(cin >> precioSilla) || precioSilla <= 0) {
            throw invalid_argument("El precio de la silla debe ser un numero positivo.");
        }

        // Mostrar confirmación
        cout.precision(2);
        cout << fixed;
        cout << "\nPrecios registrados:\nMesas = USD " << precioMesa << "\nSillas = USD " << precioSilla << endl;

    } catch (const invalid_argument& e) {
        cin.clear(); 
        cin.ignore(1000, '\n');
        cerr << "Error: " << e.what() << "\nReintentando ingreso...\n";
        ingresoPreciodeVenta(); // Llama de nuevo a la función para reintentar
    }
}


// Opción 2: Ingresar restricciones de producción
void ingresoRestriccionesDeProduccion() {
    cout << "\n--- INGRESO DE RESTRICCIONES ---" << endl;
    cout << "Formato: coeficienteX1 coeficienteX2 valorTotal\n";
    cout << "Ejemplo: 4 3 240 (para 4x1 + 3x2 <= 240)\n";
    cout << "Escriba 'fin' para terminar\n";

    cin.ignore(); // Limpiar el búfer de entrada

    while (true) {
        string entrada;
        double x1, x2, total;

        cout << "\nIngrese restriccion: ";
        getline(cin, entrada);

        if (entrada == "fin") break; // Termina el ingreso de restricciones

        // Intenta parsear y validar la entrada
        try {
            if (sscanf(entrada.c_str(), "%lf %lf %lf", &x1, &x2, &total) == 3) { // Verifica que se ingresen 3 números
                if (x1 >= 0 && x2 >= 0 && total >= 0) { // Validación de valores no negativos
                    restricciones.push_back({x1, x2, total}); // Almacena en el vector dinamicamente
                    cout << "Agregada: " << x1 << "x1 + " << x2 << "x2 <= " << total << endl;
                } else {
                    throw invalid_argument("Todos los coeficientes deben ser valores positivos.");
                }
            } else {
                throw invalid_argument("Formato incorrecto. Use 3 numeros separados por espacios.");
            }
        // Captura errores de formato o validación
        } catch (const invalid_argument& e) {
            cerr << "Error: " << e.what() << " Intente de nuevo.\n";
        }
    }

    // Mostrar todas las restricciones válidas
    cout << "\nRestricciones registradas:" << endl;
    for (auto& r : restricciones) {
        cout << r[0] << "x1 + " << r[1] << "x2 <= " << r[2] << endl;
    }
}

// Opción 3: Mostrar la función de ganancia
void mostrarFuncionGanancia() {
    cout << "\n--- FUNCION DE GANANCIA ---" << endl;
    // Verifica que se hayan ingresado los precios
    if (precioMesa > 0 && precioSilla > 0) {
        cout << "Maximizar Z = " << precioMesa << "x1 + " << precioSilla << "x2" << endl;
    } else {
        cout << "Error: Primero ingrese los precios (Opcion 1)" << endl; // Validación de precios
    }
}

// Opción 4: Calcular solución óptima
void calcularSolucionOptima() {
    cout << "\n--- CALCULO DE LA SOLUCION OPTIMA EN INTERSECCIONES ---" << endl;
    // Verifica que se hayan ingresado los precios y al menos 2 restricciones
    if (precioMesa == 0 || precioSilla == 0 || restricciones.size() < 2) {
        cout << "Error: Faltan datos. Use opciones 1 y 2 primero." << endl;
        return;
    }
    
    vector<vector<double>> intersecciones; // Almacena puntos de intersección factibles

    // Calcular intersecciones entre cada par de restricciones
    for (int i = 0; i < restricciones.size(); i++) {
        for (int j = i + 1; j < restricciones.size(); j++) {
            // Extraer coeficientes de las restricciones
            double a1 = restricciones[i][0], b1 = restricciones[i][1], c1 = restricciones[i][2];
            double a2 = restricciones[j][0], b2 = restricciones[j][1], c2 = restricciones[j][2];
            double det = a1 * b2 - a2 * b1; // Determinante del sistema de ecuaciones por el metodo de Cramer

            if (fabs(det) > 1e-9) { // Evitar división por cero
                double x1 = (c1 * b2 - c2 * b1) / det; 
                double x2 = (a1 * c2 - a2 * c1) / det;
                if (x1 >= 0 && x2 >= 0) { // Verifica que las intersecciones sean factibles (no negativas)
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
    double mejorZ = -1;
    int mejorPunto = -1;

    cout << "\nEvaluando intersecciones:" << endl;
    // Iterar sobre cada punto de intersección
    for (int i = 0; i < intersecciones.size(); i++) {
        bool valido = true; 
        double x = intersecciones[i][0]; // Coordenada x 
        double y = intersecciones[i][1]; // Coordenada y
        // Verificar si cumple con todas las restricciones
        for (int j = 0; j < restricciones.size(); j++) {
            double val = restricciones[j][0] * x + restricciones[j][1] * y; // Evaluar la restricción
            // Verifica si la intersección cumple con la restricción
            if (val > restricciones[j][2] + 0.001) {
                valido = false;
                break;
            }
        }
        // Verifica si el punto es válido, si lo es, calcular la ganancia
        if (valido) {
            double z = precioMesa * x + precioSilla * y;
            cout << "(" << x << ", " << y << ") -> Z = " << z << endl;
            // Si es mejor que el mejor encontrado, actualizar
            if (z > mejorZ) {
                mejorZ = z;
                mejorPunto = i;
            }
        }
    }

    // Mostrar el resultado
    if (mejorPunto >= 0) {
        optimoX = intersecciones[mejorPunto][0];
        optimoY = intersecciones[mejorPunto][1];
        cout << "\n=== SOLUCION OPTIMA EN INTERSECCION ===" << endl;
        cout << "Mesas (x1) = " << optimoX << endl;
        cout << "Sillas (x2) = " << optimoY << endl;
        cout << "Ganancia maxima = " << mejorZ << endl;
    } else {
        cout << "No se encontro solucion factible en las intersecciones." << endl;
    }
}

// Opción 5: Graficar la región factible y las líneas de nivel
void graficarSolucion() {
    //Verifica que haya al menos 2 restricciones y un punto optimo
    if (restricciones.size() < 2 || optimoX < 0 || optimoY < 0) {
        cout << "Error: primero calcule la solucion optima con la opcion 4." << endl;
        return;
    }
    //Verifica si las restricciones se cruzan
    double a1 = restricciones[0][0], b1 = restricciones[0][1], c1 = restricciones[0][2];
    double a2 = restricciones[1][0], b2 = restricciones[1][1], c2 = restricciones[1][2];

    // Calcula el determinante para verificar si las restricciones son paralelas
    double det = a1 * b2 - a2 * b1;
    if (det == 0) {
        cout << "Las restricciones 1 y 2 son paralelas, no se puede graficar la region correctamente." << endl;
        return;
    }
    //Calcula el punto de interseccion
    double Xinter = (c1 * b2 - c2 * b1) / det;
    double Yinter = (c1 * a2 - c2 * a1) / (b1 * a2 - b2 * a1);
    
    //interseccion entre las dos restricciones
    vector<Punto> candidatos = {
        {0, 0},
        {(a1 != 0) ? c1 / a1 : 0, 0}, 
        {0, (b1 != 0) ? c1 / b1 : 0},
        {(a2 != 0) ? c2 / a2 : 0, 0},
        {0, (b2 != 0) ? c2 / b2 : 0},
        {Xinter, Yinter}
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
    // Calcula el centro del polígono
    centro.x /= region.size();
    centro.y /= region.size();
    // Ordena los puntos por ángulo respecto al centro
    auto compararAngulos = [&](const Punto& a, const Punto& b) {
        return atan2(a.y - centro.y, a.x - centro.x) < atan2(b.y - centro.y, b.x - centro.x);
    };

    // Ordena los puntos en sentido antihorario
    sort(region.begin(), region.end(), compararAngulos);

    region.push_back(region[0]); // Cierra el polígono agregando el primer punto al final
    

    //Prepara las instrucciones para Gnuplot
    ofstream data("region.dat");
    for (auto& punto : region) { // Guarda los puntos de la región factible en un archivo
        data << punto.x << " " << punto.y << "\n";
    }
    data.close();
    
    //Calcular las lineas del nivel de ganancia
    double Z_optima = precioMesa * optimoX + precioSilla * optimoY;
    double Z1 = Z_optima - 5;
    double Z2 = Z_optima + 5;
    
    //Ajusta los ejes de la gráfica
    double x_max = optimoX;
    double y_max = optimoY;
    // Encuentra los valores máximos de x e y en la región factible
    for (auto& punto_actual : region) {
        if (punto_actual.x > x_max) x_max = punto_actual.x;
        if (punto_actual.y > y_max) y_max = punto_actual.y;
    }
    // Aumenta un 20% los máximos para que la gráfica no esté tan ajustada
    x_max *= 1.2; if (x_max < 10) x_max = 10;
    y_max *= 1.2; if (y_max < 10) y_max = 10;
    
    //Popen se comunica con Gnuplot
    FILE* gnuplot = popen("gnuplot -persistent", "w"); // Abre una tubería para enviar comandos a Gnuplot
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
        "'region.dat' with filledcurves fs solid 0.3 lc rgb 'yellow' title 'Region factible', \\\n" // Dibuja la región factible 
        "(%f - %f*x)/%f with lines lw 2 lc rgb 'blue' title '%.0fx1 + %.0fx2 <= %.0f', \\\n"
        "(%f - %f*x)/%f with lines lw 2 lc rgb 'green' title '%.0fx1 + %.0fx2 <= %.0f', \\\n"
        "(%f - %f*x)/%f with lines dt 2 lc rgb 'red' title 'Z = %.2fx1 + %.2fx2 = %.2f', \\\n"
        "(%f - %f*x)/%f with lines dt 2 lc rgb 'red' title 'Z = %.2fx1 + %.2fx2 = %.2f', \\\n"
        "'-' with points pt 7 ps 1.5 lc rgb 'black' title 'Punto optimo (%.2f, %.2f)'\n", //Punto en las coordenadas exactas del punto óptimo
        c1, a1, b1, a1, b1, c1,
        c2, a2, b2, a2, b2, c2,
        Z1, precioMesa, precioSilla, precioMesa, precioSilla, Z1,
        Z2, precioMesa, precioSilla, precioMesa, precioSilla, Z2,
        optimoX, optimoY
    );

    //Finaliza la comunicación con Gnuplot
    fprintf(gnuplot, "%.6f %.6f\n", optimoX, optimoY);
    fprintf(gnuplot, "e\n");
    pclose(gnuplot);
}

