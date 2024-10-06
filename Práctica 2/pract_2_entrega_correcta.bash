#!/bin/bash
#*****************************************************************
# File:   pract_2_entrega_correcta.bash
# Author: PSCD-Universidad de Zaragoza
# Date:   octubre 2020
# Coms:   script de comprobación de entrega de la práctica 2 de PSCD (20/21)
#         Funcionamiento: 
#          1) Dar permisos de ejecución al script: "chmod u+x pract_2_entrega_correcta.bash"
#          2) Ejecutar mediante
#                  ./pract_2_entrega_correcta.bash NIP1_NIP2
#             en el directorio donde está el fichero "practica_2_NIP1_NIP2.zip"
#             Creará un directorio temporal donde descomprimirá el zip,
#             comprobará si están los ficheros que tienen que estar,
#             informando de los problemas. Posteriormente borrará el directorio creado
#             Si todo ha ido bien, terminará con el mensaje
#                  ------> OK: I have found all the mandatory files
#*****************************************************************

DIR_CORRECCION=$(pwd)
PREFIX_PRACT="practica_2"
FUENTE_ZIP="${PREFIX_PRACT}_${1}.zip"
USER=a$1       #usuarios en Hendrix son "a" seguido del NIP
FALTAN=()      #ficheros que faltan
#FICHEROS=(main_p2_e1.cpp main_p2_e2.cpp Makefile_p2_e1 Makefile_p2_e2)
#solo la primera parte es obligatoria
FICHEROS=(main_p2_e1.cpp Makefile_p2_e1)
DIRECTORIOS=(${PREFIX_PRACT}_${1})

#si alguien usa esto como nombre de directorio, se merece el castigo de perderlo
DIR_TEMPORAL=${DIR_CORRECCION}/__PSCD_NO_ES_INTERESANTE__
DIR_COMPILACION=${DIR_TEMPORAL}/${PREFIX_PRACT}_${1}

FORM=" ------> "
SEP="========================================================================="

echo ${SEP}
#----------------------------------------------------------------
#Comprueba que se invoque con un parámetro (que se supone es un NIP)
if [[ "$#" -ne 1 ]]
then
	echo "Invocar como: ${0} miNip"
	echo "------------------------------------------"
	exit
fi
#----------------------------------------------------------------
#  crear directorio temporal. Si existe, lo borra. Si alguien
#  había llamado a un directorio así, se lo merece
echo "${FORM}Creating temporal directory ..."
if [ -d "${DIR_TEMPORAL}" ]
then
	/bin/rm -rf ${DIR_TEMPORAL}
fi
mkdir ${DIR_TEMPORAL}
#----------------------------------------------------------------
#  mirar si ha entregado el fichero "zip"

if [ ! -f "${FUENTE_ZIP}" ]
then
	FALTAN=( ${FUENTE_ZIP} "${FALTAN}[@]")
	echo "${FORM}Falta fichero ${FUENTE_ZIP}"
else #comprobar están todos los directorios requeridos
	cp ${FUENTE_ZIP} ${DIR_TEMPORAL}/${FUENTE_ZIP}
	cd ${DIR_TEMPORAL}
	echo "${FORM}Extracting zip file ..."
	unzip ${FUENTE_ZIP}
	for dir in "${DIRECTORIOS[@]}"
	do
		if [ ! -d "${dir}" ]
		then
			echo "${FORM}Can't find directory ${dir}"
			FALTAN=(${dir} "${FALTAN}[@]")
		fi
	done
fi
#comprobar están todos los ficheros requeridos
if [[ ${#FALTAN[@]} -eq 0 ]] 
then
	for fich in "${FICHEROS[@]}"
	do
		echo "  Checking for ${PREFIX_PRACT}_${1}/${fich}"
		if [ ! -f "${PREFIX_PRACT}_${1}/${fich}" ]
		then
			echo "${FORM}Can't find file ${fich}"
			FALTAN=(${fich} "${FALTAN}[@]")
		fi
	done
fi

#volver al directorio original
cd ${DIR_CORRECCION}
#borrar el temporal
echo "${FORM}Deleting temporary directory ..."
/bin/rm -rf ${DIR_TEMPORAL}
#mensaje final
if [[ ${#FALTAN[@]} -eq 0 ]] 
then
	echo "${FORM}OK: I have found all the mandatory files"
else
	echo "${FORM}ERROR: I can't find some mandatory files"
	echo "${FORM}YOU CANNOT SUBMIT THIS HOMEWORK"
fi
echo ${SEP}
stty erase "^?"
exit 0
