import numpy as np 
import os


def loadFile(fileName, directory=None, currFile=None):
	if directory != None:
		fileName= directory + '/' + fileName
	if currFile != None:
		fileName = absPath(currFile, fileName)

	return np.load(fileName)['_arr_0']


def absPath(currFile, fileName):
	return os.path.join(os.path.dirname, fileName)
