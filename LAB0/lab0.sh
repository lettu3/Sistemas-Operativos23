#!/bin/bash

#Ejercicio 1
cat /proc/cpuinfo | grep "model name" | uniq

#Ejercicio 2
cat /proc/cpuinfo | grep "model name" | wc -l

#Ejercicio 3
curl https://www.gutenberg.org/files/11/11-0.txt | sed -e 's:Alice:Ignacio:g'> Ignacio_in_wonderland.txt ; printf "Archivo ejercicio 3 creado\n"

#Ejercicio 4
#Temperatura Maxima historica
sort -n -k5,5  Archivos\ de\ datos-20230821/weather_cordoba.in | tail -n 1 > maximo_historico ; printf "Archivo maximo historico creado\n"
#Temperatura Minima historica
sort -n -k6,6  Archivos\ de\ datos-20230821/weather_cordoba.in | head -n 1 > minimo_historico;  printf "Archivo minimo historico creado\n"

#Ejercicio 5
sort -n -k3,3 Archivos\ de\ datos-20230821/atpplayers.in > ranking_jugadores;  printf "Archivo ejercicio 5 creado\n"

#Ejercicio 6

#Ejercicio 7
ip address | grep ether | cut -d " " -f6

#Ejercicio 8
#a)

#b)

#OPCIONAL
#1
#a)
# ffmpeg -i (mi input)  -ss  (xx:xx:xx horas/minutos/segundos de inicio) -to (xx:xx:xx horas/minutos/segundos de finalizacion) (mi output)

#b)

