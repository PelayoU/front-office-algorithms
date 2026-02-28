boost_test_project(NAME test_practica1 DEPS practica1)#DEPS significa dependencias, es decir, que la prueba depende de la libreria practica1.
# A diferencia de create_library,boost_test_project CREA UN EJECTUBLE, es una funcion que rastrea tu subcarpeta tests/, coge los archivos .cpp (como test.cpp), y los compila en un programa independiente llamado test_foo
#(DEPS foo): No crea una librería que contenga a foo. Lo que hace es indicar al Linker (el enlazador) que tu ejecutable de prueba necesita conectarse a la librería dinámica libfoo.dylib que creaste previamente. De esta forma, cuando escribes #include "foo.h" en tu archivo de test, el binario sabe perfectamente dónde encontrar el código máquina de esas funciones para probarlas.

