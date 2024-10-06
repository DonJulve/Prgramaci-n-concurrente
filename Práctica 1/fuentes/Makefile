#------------------------------------------------------------------------------
# File:   Makefile
# Author: PSCD-Unizar
# Date:   octubre 2019
# Coms:   Ejemplo de Makefile para la práctica 1 de PSCD
#         "make practica_1_V1" compilará y generará el ejecutable "practica_1_V1"
#         "make practica_1_V2" compilará y generará el ejecutable "practica_1_V2"
#         "make all" compilará y generará ambos ejecutables 
#               (alternativamente, "make", sin parámetros)
#         "make clean" borra todo lo genearado, si existe
#------------------------------------------------------------------------------

#---------------------------------------------------------
#definición de variables
CC=g++
CPPFLAGS=-I. -g -std=c++11      #opciones de compilación
LDFLAGS=-pthread             #opciones de enlazado (linkado)
RM=rm -f

all: practica_1_V1 practica_1_V2
#-----------------------------------------------------------
# Compilacion
practica_1_V1.o: practica_1_V1.cpp
	${CC} -c practica_1_V1.cpp ${CPPFLAGS}
#linkado
practica_1_V1: practica_1_V1.o
	${CC} practica_1_V1.o -o practica_1_V1 ${LDFLAGS}
#-----------------------------------------------------------
# Compilacion
practica_1_V2.o: practica_1_V2.cpp
	${CC} -c practica_1_V2.cpp ${CPPFLAGS}
#linkado
practica_1_V2: practica_1_V2.o
	${CC} practica_1_V2.o -o practica_1_V2 ${LDFLAGS}
#-----------------------------------------------------------	
# LIMPIEZA de todo lo generado. OjO: se borrarán sin preguntar
# RM es una de las variables predefinidas en make: invoca borrado de fichero del SO
clean:
	${RM} practica_1_V1.o practica_1_V1
	${RM} practica_1_V2.o practica_1_V2
