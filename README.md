# Tarea 2: Sincronización de _"threads"_

Los integrantes del equipo son:

- Abreu Cárdenas, Miguel Guillermo
- Guevara Sánchez, David Alberto
- Ortíz Ruiz, Victor Adolfo
- Yip Chen, Michael

## ¿Cómo ejecutar el proyecto?

```sh
# Primero se necesita o clonar el repositorio o extraer el tarball. e.g.
tar -xvf Abreu-Guevara-Ortiz-Yip.tgz
# Luego es necesario entrar al directorio extraído. e.g.
cd Abreu-Guevara-Ortiz-Yip
# Luego de eso se necesita compilar el proyecto.
# Si clang no está instalado o se desea usar otro compilador ejecutar como: make CC=gcc
make
# Y finalmente se puede ejecutar el proyecto. e.g.
./build/main 10 15
# Esto resulta con el programa siendo ejecutado con 10 carros viniendo del este
# y 15 carros viniendo del oeste
```

## ¿Cuáles son los argumentos con los que puedo ejecutar el proyecto?

El programa acepta como primer argumento la cantidad de carros viniendo del
este y como segundo argumento la cantidad de carros viniendo del oeste.

La cantidad de carros viniendo en cada dirección debe ser un número entero
positivo. Cualquier número menor a 0 resultará en ningún carro viniendo de la
dirección. Como la entrada es procesada con la función `atoi()` si el argumento
no empieza con un número entero entonces se asumirá 0.

## ¿Cuál es el λ utilizado para cada dirección?

Para la dirección de **Este a Oeste** se utiliza un lambda de 0.005.

Para la dirección de **Oeste a Este** se utiliza un lambda de 0.005.

## ¿Qué no funciona del proyecto?

Todo el proyecto está funcionando correctamente según las especificaciones del profesor.

## Capturas de pantalla

<img width="682" alt="image" src="https://user-images.githubusercontent.com/37723586/232627763-a10ff6cf-c2f1-4da4-89d2-f9e6fd7b5a7f.png">

<img width="682" alt="image" src="https://user-images.githubusercontent.com/37723586/232627883-29329543-63f1-4ed8-b85e-95314d308323.png">

<img width="682" alt="image" src="https://user-images.githubusercontent.com/37723586/232627976-d98edff5-9fbc-4c18-a483-170244578844.png">
