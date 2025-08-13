# Usar imagen base de Ubuntu con compiladores C++
FROM ubuntu:22.04

# Evitar prompts interactivos durante la instalación
ENV DEBIAN_FRONTEND=noninteractive

# Actualizar el sistema e instalar dependencias
RUN apt-get update && apt-get install -y \
    g++ \
    gnuplot-nox \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Crear directorio de trabajo
WORKDIR /app

# Copiar archivos del proyecto
COPY proyecto2doBimestre.cpp .
COPY declaraciones.h .

# Compilar la aplicación
RUN g++ -o proyecto2doBimestre proyecto2doBimestre.cpp

# Crear directorio para archivos de salida
RUN mkdir -p /app/output

# Comando por defecto para ejecutar la aplicación
CMD ["./proyecto2doBimestre"]