#!/bin/bash

if [ $# -lt 2 ] || [ $# -gt 3 ]; then
    echo "Usage: $0 <nb_worker_min> <nb_worker_max> [optionnal_param]"
    exit 1
fi

nb_worker_min=$1
nb_worker_max=$2

if [ $nb_worker_min -gt $nb_worker_max ]; then
    echo "Erreur : Le nombre minimum de workers doit être inférieur ou égal au nombre maximum."
    exit 1
fi

optionnal_param=""
if [ $# -eq 3 ]; then
    optionnal_param=$3
fi

params=""

make lightMain

for ((i=nb_worker_min; i<=nb_worker_max; i++)); do
    echo "Nb workers : $i"

    #make run_light_tracker

    output_dir="${optionnal_param}/light_${i}"
    mkdir -p "$output_dir"
    mpirun -np "$i" ./lightMain "$output_dir"

    params="$params $output_dir"
done

if [ -z "$optionnal_param" ]; then
    command="python3 compare.py $params"
else
    command="python3 compare.py $optionnal_param $params"
fi

echo "Commande exécutée : $command"

$command
