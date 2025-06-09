import argparse

from classSrc import Data

parser = argparse.ArgumentParser(description='Passer le nom du fichier en paramètre')
parser.add_argument('inputFilePath', type=str, help='Le chemin vers le fichier JSON')
args = parser.parse_args()

baseDirectoryPath = args.inputFilePath

data = Data(baseDirectoryPath, baseDirectoryPath)
data.generateHtml()
