# Sistemas Operativos 2023
## Laboratorio 1: MyBash
Revisión 2023, Pablo Ventura
Revisión 2022, Marco Rocchietti
Revisión 2021, Facundo Bustos
Revisión 2020, Marco Rocchietti, Facundo Bustos
Revisión 2019, Milagro Teruel, Marco Rocchietti, Ignacio Moretti
Revisión 2011, 2012, 2013, Carlos S. Bederián
Revisión 2009, 2010, Daniel F. Moisset
Original 2008, Nicolás Wolovick

## Objetivos
- Utilizar los mecanismos de **concurrencia** y **comunicación** de *gruesa granularidad* que brinda UNIX.
- Comprender que un intérprete de línea de comandos refleja la arquitectura y estructura interna de las primitivas de comunicación y concurrencia. 
- Implementar de manera *sencilla* un intérprete de línea de comandos (*shell*).
- Utilizar **buenas prácticas de programación**: estilo de código, tipos abstractos de datos (*TAD*), prueba unitaria (*unit testing*), prueba de caja cerrada (*black box testing*), programación defensiva; así como herramientas de *debugging* de programas y memoria.

## Objetivos de la implementación
Codificar un ***shell*** al estilo de bash (**B***ourne* **A***gain* **SH***ell*) al que llamaremos ***mybash***. El programa debe tener las siguientes funcionalidades generales:    
- Ejecución de comandos en modo *foreground* y *background*
- Redirección de entrada y salida estándar.
- *Pipe* entre comandos.

Debería poder ejecutar correctamente los siguientes ejemplos:
```
ls -l mybash.c
ls 1 2 3 4 5 6 7 8 9 10 11 12 ... 194
wc -l > out < in
/usr/bin/xeyes &
ls | wc -l
```
En particular deberán:
- Implementar los comandos internos  `cd`, `help` y `exit`.
- Poder salir con `CTRL-D`, el caracter de fin de transmisión (EOT).
- Ser robusto ante entradas incompletas y/o inválidas.

Para la implementación se pide en general:
- Utilizar TADs opacos.
- No perder memoria.
- Seguir buenas prácticas de programación (*coding style*, modularización, buenos comentarios, buenos nombres de variables, uniformidad idiomática, etc).

## Modularización
Para la implementación de este laboratorio se propone una división en 6 módulos:
- `mybash`: módulo principal
- `command`: módulo con las definiciones de los TADs para representar comandos
- `parsing`: módulo de procesamiento de la entrada del usuario usando un parser
- `parser`: módulo que implementa el TAD parser
- `execute`: módulo ejecutor de comandos, administra las llamadas al sistema operativo
- `builtin`: módulo que implementa los comandos internos del intérprete de comandos.

El diseño se puede ilustrar con el siguiente diagrama:
![](foto aqui)

El módulo `command` define el TAD `scommand` y `pipeline` que proveen una representación abstracta de los comandos la cual es utilizada por los otros módulos. El módulo `parsing` se encarga de procesar la entrada del usuario (leída desde la *standard input*) y generar una instancia del TAD *pipeline* que contiene el comando interpretado. Para procesar la entrada debe utilizar accesoriamente el TAD *parser* que está definido en el módulo `parser`. El módulo principal `mybash`, invoca alternadamente en un ciclo al procesador de entrada y al ejecutor. Finalmente el módulo `execute` toma la instancia de `pipeline` y procede a realizar los {`fork`, `execvp`, `wait`, `dup`, `pipe`, etc} necesarios, o bien, en caso de comandos internos, invoca a `builtin`.
Notar que esta modularización funcional depende fuertemente del TAD `pipeline`, por lo tanto el módulo `command` debe ser implementado rápida y correctamente.

Adicionalmente se incluye el módulo `strextra` donde se declara la función `strmerge()` que deberán implementar en `strextra.c`. Esta función será de utilidad para implementar las funciones `scommand_to_string()` y `pipeline_to_string()` de los TADs `scommand` y `pipeline` respectivamente.

## TADs *pipeline* y *scommand*
