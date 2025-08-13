# Ejecutar Proyecto con Docker

## Prerrequisitos
- Docker
- Docker Compose

## Instrucciones de uso

### 1. Construir la imagen
```bash
docker-compose build
```

### 2. Ejecutar en modo interactivo
```bash
# Ejecutar la aplicación (podrás escribir en el menú)
docker-compose run --rm proyecto-cpp

# Alternativamente, si prefieres usar docker directamente:
docker run -it --rm proyecto-test
```

**Nota**: Una vez ejecutado, verás el menú y podrás escribir números (1-6) para navegar por las opciones.

### 3. Para visualizar gráficas (Linux con X11)
```bash
# Permitir conexiones X11
xhost +local:docker

# Ejecutar con soporte gráfico
DISPLAY=$DISPLAY docker-compose run --rm proyecto-cpp
```

### 4. Copiar archivos generados (opcional)
```bash
# Los archivos se guardan en un volumen, para acceder:
docker run --rm -v proyecto-segundo-bimestre-programaci-n1_output_data:/data alpine ls -la /data
```

### 5. Limpiar recursos
```bash
docker-compose down -v
```

## Notas importantes
- La aplicación se ejecuta de forma interactiva
- Los archivos generados (region.dat) se guardan en un volumen Docker
- Para gráficas en Windows/Mac, se requiere configuración adicional de X11
- El ejecutable se compila dentro del contenedor, no se sobrescribe