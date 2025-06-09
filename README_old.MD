# Projet CHPS0701 - Timothé KRUK

## Répertoire

Voici un détail de notre archive :

```
|
  README.MD : Explique l'archive et comment faire fonctionner les codes
  CHPS0701_rappor_KRUK_Timothe.pdf : Rapport du projet
  resultats : Dossier contenant un ensemble de résultats obtenus après exécution des différents codes
  |
    resultats.xlsx : Fichiers excels montrant les principaux résultats
    memory_report : Dossier contenant des fichiers visuables avec massif
    * : Les dossiers restant contiennent les données sauvegardées durant des exécutions. Il faut ouvrir les fichiers index.html. De plus des données brutes contenu dans des fichiers json ont été supprimées pour alléger l'archive
  code : Dossier contenant les différents éléments de code
  |
    CMakeLists.txt : Fichier CMake utilisé pour compiler.
    fullMain.cpp : Fichier main pour résoudre le problème de Langford en sauvegardant un maximum de données annexes.
    lightMain.cpp : Fichier main pour résoudre le problème de Langford en sauvegardant le minimum de données annexes. Il y a essentiellement les temps pour résoudre le problème.
    noMain.cpp : Fichier main pour résoudre le problème de Langford sans sauvegarder de données et avec un affichage minimale dans la sortie standard.
    *.py : Code python permettant de post traiter les informations enregistrées
    teste_openmp_multiple_worker.sh : Fichier utilisé pour lancer une campagne de teste pour comparer une stratégie en fonction du nombre de threads. (Se base sur le main lightMain.cpp)
    src : Dossier contenant le code source de notre librarie utilisé par les fichier main
    template : Dossier contenant les templates utilisés pour la génération de fichiers html
    documentation : Dossier contenant diverses éléments liées à la modélisation
```

## Code

### Dépendances

### 1. Dépendances C++

Avant de pouvoir compiler et exécuter ce projet, vous devez installer les dépendances suivantes :

- **CMake** (version 3.12 ou supérieure)
- **nlohmann_json** (bibliothèque JSON pour C++)
- **RapidJSON** (bibliothèque JSON pour C++)
- **OpenMP** (support pour la parallélisation avec OpenMP)
- **MPI** (support pour la parallélisation avec MPI)

Pour installer **nlohmann_json** et **RapidJSON**, vous pouvez exécuter la commande suivante

```bash
sudo apt install nlohmann-json3-dev rapidjson-dev
```

### 2. Dépendances Python

Les scripts Python nécessitent les bibliothèques suivantes :

- `matplotlib`
- `numpy`
- `jinja2`

Pour installer les bibliothèques, exécutez la commande suivante :

```bash
pip install matplotlib numpy jinja2
```

### Compilation

Pour compiler le projet :

1. Depuis le répertoire code créer un répertoire de build :

```bash
mkdir build
cd build
```

2. Exécutez la commande CMake pour configurer le projet :

```bash
cmake ..
```

3. Compilez le projet :

```bash
make
```

Cela va générer les exécutables suivants :
- fullMain
- lightMain
- noMain

4. Pour avoir les dépendances

Appeler la cible qui va générer les dépendances pour l'utilisation des scriptes et autres.

```bash
make all_required_depends
```

5. Pour configurer le problème de Langford

Choisiser le main que vous souhaiter utiliser (fullMain.cpp, lightMain.cpp, noMain.cpp), puis ouvrer le est modifié la valeur de la variable F pour changer la fréquence. Pour changer la stratégie utilisé, comment la ligne qui définis le type de Resolver puis décommenter la ligne qui utilise votre stratégie.

Puis vous pouvez recompiler votre cible.

### Exécution

Pour exécuter le fichier noMain.

```bash
./noMain
```

Pour exécuter le fichier lightMain, passer en paramètre le chemain de répertoire de sortie des fichiers générés.

```bash
./lightMain output/light
```

Pour exécuter le fichier fullMain, passer en paramètre le chemain de répertoire de sortie des fichiers générés.

```bash
./fullMain output/full
```

Si vous utilisez une stratégie implémenté avec OpenMP, vous pouvez changer le nombre de threads avec la commande suivante.

```bash
export OMP_NUM_THREADS=X
```

Si vous utilisez une stratégie implémenté avec MPI, vous devez lancer votre programme en précédant de mpirun -np X.

```bash
mpirun -np X ./lightMain output/light
```

### Exécution de cibles personnalisées

Les différentes cibles personnalisées permettent d'exécuter un programme pour appliquer un post traitement sur les données générées.

1. Générer les fichiers nécessaires

Pour avoir les scripts dans votre répertoire build vous pouvez appeller la cible suivante.

```bash
make all_required_depends
```

Les différentes cibles suivantes ne fonctionneront uniquement si la stratégie choisie ne repose pas sur MPI sauf exception spécifié.

Pour appeler exécuter le fichier lightMain et d'y générer un fichier html et graphiques permettant de lire les données. Vous pouvez exécuter la cible suivante, les fichiers générés se trouveront dans le dossier output/light.

```bash
make run_light_tracker
```

Pour appeler exécuter le fichier fullMain et d'y générer un fichier html et graphiques permettant de lire les données. Vous pouvez exécuter la cible suivante, les fichiers générés se trouveront dans le dossier output/full. Attention, si la fréquence du problème de Langford est élevé il peut y a voir une utilisation excéssive de la mémoire et un temps grand pour stocker les données et générées les fichiers html et graphique.

```bash
make run_full_tracker
```

Pour appeler exécuter le fichier fullMain et d'y générer un fichier html sans graphiques permettant de lire les données. Vous pouvez exécuter la cible suivante, les fichiers générés se trouveront dans le dossier output/full. Attention, si la fréquence du problème de Langford est élevé il peut y a voir une utilisation excéssive de la mémoire et un temps grand pour stocker les données et générées les fichiers html. Le temps nécessaire sont plus court que par rapport à la cible run_full_tracker.

```bash
make run_full_no_graph_tracker
```

Pour lancer une campagne de teste pour tester le nombre de threads en fonction d'une stratégie implémenter avec MPI, vous pouvez exécuter le script teste_mpi_multiple_worker.sh. En paramètre vous devez passer le minimum de threads suivie du maximum, l'incrément est de 1. Ensuite, passez le répertoire de sortie. Le fichier permettant de visualiser la comparaison se trouve dans le répertoire de sortie. Enfin, dans les dossiers de la forme light_*/ se trouve les résultats pour un thread spécifié.

```bash
./teste_openmp_multiple_worker.sh 1 12 output/
```

Pour lancer une campagne de teste pour tester le nombre de procésus en fonction d'une stratégie implémenter avec MPI, vous pouvez exécuter le script teste_mpi_multiple_worker.sh. En paramètre vous devez passer le minimum de threads suivie du maximum, l'incrément est de 1. Ensuite, passez le répertoire de sortie. Le fichier permettant de visualiser la comparaison se trouve dans le répertoire de sortie. Enfin, dans les dossiers de la forme light_*/ se trouve les résultats pour un thread spécifié. A noter, qu'il s'agit de la seule cible appelable si l'implémentation repose sur MPI.

```bash
./teste_mpi_multiple_worker.sh 1 12 output/
```

Pour supprimer le contenu des répertoires output/ et output/full et ouptut/light vous pouvez appeler la cible suivantes.

```bash
make clean_base_output_directory
```

Pour supprimer l'ensemble des fichiers générés, appelez la cible suivante.

```bash
make clean
```

### Utilisation d'outils externes

Cette partie explique comment utiliser Intel VTune et Valgrind pour profiler et analyser les performances des programmes

1. Profilage avec Intel VTune

Avant d'utiliser Intel VTune et après l'avoir installé, vous devez configurer l'environnement en exécutant la commande suivante :

```bash
source /opt/intel/oneapi/vtune/latest/vtune-vars.sh
```

Vous pouvez utiliser VTune pour collecter un ensemble de données.

Cette commande permet de collecter des informations sur l'utilisation des threads par le programme :

```bash
vtune -collect threading -- ./noMain
```

Cette commande identifie les zones de votre code où le temps d'exécution est le plus important (hotspots) :

```bash
vtune -collect hotspots -- ./noMain
```

Pour visualiser les résultats de l’analyse avec une interface graphique, lancez la commande suivante :

```bash
vtune-gui
```

2. Analyse de la mémoire avec Valgrind

Utilisez l’outil Massif de Valgrind pour analyser la consommation de mémoire de votre programme :

```bash
valgrind --tool=massif ./noMain
```

Cette commande génère un fichier de sortie contenant les données sur l’utilisation de la mémoire. Le nom du fichier généré sera sous la forme massif.out.<pid>, où <pid> est l’identifiant de processus.

Pour visualiser les résultats de l’analyse de mémoire, utilisez Massif-Visualizer :

```bash
massif-visualizer massif.out.203819
```

Remplacez massif.out.203819 par le nom du fichier de sortie généré par Valgrind.
