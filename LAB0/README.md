# Práctico 0: Shellscripting

## Sistemas operativos - 2023

Según Wikipedia, el Shell *es el programa informático que provee una interfaz de usuario para acceder a los servicios del sistema operativo*. Comenzaremos la materia familiarizandonos con esta herramienta, que usarán el resto de sus carreras, y con las llamadas de sistema más comunes que pueden acceder.
Este práctico consiste de una serie de ejercicios que se resuelven con **una** línea de comando ejecutada en la consola. Necesitaran descargar los archivos de datos dados en sus computadoras para comprobar si el comando que proponen funciona correctamente.
Tendrán que **investigar por su cuenta** cómo funcionan los distintos comandos (como `grep`, `cat`, `sort`, `head`), pipes `|` y redirecciones `>` `<`. Les dejamos un pequeno machete como ayuda (*Bite_Size_Linux.pdf*). Otro recurso que les puede ser de utilidad es el sitio explainshell.com, para comandos que encuentren online y que no sepan que hacen.

## Ejercicios

1. Que modelo de procesador tiene tu equipo? Cuando necesitamos un dato del equipo, como por ejemplo el modelo ddel procesador, podemos buscar en el archivo `proc/cpuinfo`. Sin embargo, este comando nos devuelve un listado muy largo y tienen que encontrar la forma de buscar solo la informacion nnecesaria. Ayuda: ver la definicion y el uso de los comandos `cat` y `grep`.

2. Si la computadore tiene mas de una unidad de ejecucion (multi-core) seguramente el punto anterior se repitio mas de una vez el modelo del procesador. Averiguar como usar `wc` para poder determinar cuantas unidades de ejecucion tien el procesador, aprovechando los comandos utilizados en el ejercicio 1.
3. Descarguen el libro de Alicia en el pais de las marvillas en txt (puede ser de este link), reemplacen todas las ocurrecias de Alice/Alicia por su nombre, y guarden el archivo como `NOMBRE_en_el_pais_de_las_maravillas.txt` o `NOMRE_in_wonderland.txt` dependiendo del idioma en que lo encuentren. Si, todo con un unico comando, incluso la descarga del archivo. Puntos extra por borrar el archivo original que descargaron.
4. El archivo `weather_cordoba.in` contiene las mediciones metereologicas realizadas en un dia en Cordoba. Las primeras 3 columnas corresponden al año, mes y dia de las mediciones. Las restantes 6 columnas son la temperatura media, la maxima, la minima, la presion atmosferica, la humedad y las precipitaciones medidas ese dia. Ordenar los dias segun su temperatura maxima y calcular maximo y minimo. Puntos extra por mostrar en la pantalla solo la fecha de dichos dias (cun comando para cada dia). Tenga nen cuenta que para evitar los números reales, los grados están expresados en décimas de grados (por ejemplo, 15.2 grados está representado por 152 décimas). La presión también ha sido multiplicada por 10 y las precipitaciones por 100, o sea que están expresadas en centésimas de milímetro.
5. El archivo `atpplayers.in` es un listado por orden alfabético de jugadores profesionales de tenis. El nombre del jugador viene acompañado de una abreviatura de su país, el número que ocupa en el ranking, su edad, su puntaje y el número de torneos jugados en el último año. Ordenar el listado de jugadores según la posision en el ranking.
6. El archivo `superliga.in` contiene datos con el siguiente formato: nombre de equipo sin espacios, puntos, partidos jugados, partidos ganados, partidos empatados, partidos perdidos, goles a favor y goles en contra. Ordenar la tabla del campeonato de la Superliga según la cantidad de puntos, y desempatar por diferencia de goles.
**Ayuda**: a todo lo que ya saben del ejercicio anterior, agreguen el uso del comando `awk`.
7. ¿Cómo ver la MAC address de nuestro equipo? Investiguen el comando `ip`. En la manual de `grep` van a encontrar la especificación de muchas operaciones, por ejemplo `-o`, `-i`, y muchas más. Algo muy utilizado son las ***expresiones regulares*** para realizar una búsqueda. En el manual de `grep` tienen un apartado donde explica su uso. Con esta información deberían poder construir una secuencia de comandos de shell para imprimir por consola la MAC address de su interfaz ethernet.
8. Supongamos que bajaron una serie de televisión completa con subtítulos (de forma completamente legal, por supuesto). Sin embargo, los subtítulos tienen el sufijo `_es` en el nombre de cada archivo y para que puedan ser reproducidos en el televisor, que nunca fue demasiado smart, el archivo de subtítulos tiene que tener exactamente el mismo nombre que el archivo de video con la extensión `srt`. La serie tiene más de 100 capítulos, imposible realizar los cambios uno a uno.
a. Para emular la situación, crear una carpeta con el nombre de la serie y dentro de ella crear 10 archivos con un nombre como `fma_S01EXX_es.srt`, donde XX es el número de capítulo, desde 1 hasta 10.
b. Con un segundo comando (el que usarían realmente), cambiar elnombre de cada archivo sacando el sufijo `_es`.

**Ayuda**: Usar un ciclo `for`. Se pueden hacer ciclos en una única línea. Puntos extra si los archivos de video no siguen un patrón, sino que pueden llamarse de cualquier forma.


1. [OPCIONAL] El comando ffmpeg sirve para editar streams de video y audio desde la consola, de forma muchísimo más rápida que otros editores de video. Tienen que descubrir cómo realizar las siguientes tareas:
a. Recortar un video. Pueden usar SimpleScreenRecorder (linux) para grabar un pequeño video de prueba. Luego, usen ffmpeg para sacarle los primeros y los últimos segundos en donde se ve el la pantalla como inician y cortan la grabación.
b. Mezclar streams de audio. Graben un pequeño audio de voz, y descarguen de internet alguna pista de sonido que quieran poner de fondo. Usen ffmpeg para superponer las dos pistas. Tomen como ejemplo los podcasts de Nico!


## Entrega
La resolución de este práctico es individual. Deben entregar un archivo .sh con una línea por comando a través de este formulario. El práctico no se evalúa con nota, pero debe estar aprobado para poder promocionar o regularizar la materia.
