import argparse
import sys

from classSrc import CompareData

parser = argparse.ArgumentParser(description='Passer une liste de répertoires en paramètre')

parser.add_argument('baseDirectoryPath', type=str, nargs='+', help='Liste des répertoires de base')

args = parser.parse_args()

if len(args.baseDirectoryPath) < 2:
    print("Erreur : Vous devez fournir au moins deux répertoires.")
    sys.exit(1)

outputDirectory = args.baseDirectoryPath[0]
lBaseDirectoryPath = args.baseDirectoryPath[1:]

compareData = CompareData(lBaseDirectoryPath, outputDirectory)
compareData.generateGraph(outputDirectory)
compareData.generateHtml(outputDirectory)
