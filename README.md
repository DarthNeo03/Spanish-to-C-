El compilador generará código c++ a partir de un lenguaje español, para la programación de microcontroladores.

# ---> TOKENS 
Los elementos que componen el lenguaje

# # Tokens en C++

   - Los nombres de las variables
   - Los nombres de las funciones
   - Los nombres de los tipos de datos
   - Las palabras clave
   - Las operaciones aritméticas
   - Las operaciones relacionales
   - Las operaciones lógicas
   - Las operaciones de asignación
   - Las operaciones de incremento y decremento
   - Las operaciones de comparación


# # Tokens en stcpp (Spanish to C++)
En el lenguaje a compilar, que lleva por nombre stcpp, los tokens propuestos serían:

## Palabras reservadas 

### tipos de datos
    entero, flotante, booleano, carácter, cadena 

### condicionales
        si entonces, sino, fin_si 

### bucles
        repetir, veces, mientras, hacer, fin_mientras 

### funciones
        funcion, retornar, fin_funcion

### valores booleanos
        verdadero, falso 

### operaciones de GPIO
        configurar_pin, escribir_pin, leer_pin 

### delays 
        esperar_microsegundos, esperar_millisegundos, esperar_segundos


## Operadores 

### Aritméticos:
         +, -, *, /, %.

### Relacionales:
         >, <, >=, <=, ==, !=.

### Lógicos:
        y, o, no.

### Asignación:
        =

# # Nota:
Por ahora, para el desarrollo y las pruebas, los únicos tokens que se utilizarán serán los siguientes:

    entero, escribir, esperar, repetir, veces, {, }, =, +, configurar_pin
