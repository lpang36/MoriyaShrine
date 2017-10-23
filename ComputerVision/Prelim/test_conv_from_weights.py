from PIL import Image
import os
import pandas as pd
import numpy as np
from keras.models import load_model
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
import h5py

dims = (300,400,3)

def load_image(filename):
	im = Image.open(filename)
	pixels = np.asarray(im.getdata())
	return pixels

def load_all(foldername):
	output = np.empty((0,dims[0],dims[1],dims[2]))
	for filename in os.listdir(foldername):
		image = np.reshape(load_image(os.path.join(foldername,filename)),(-1,dims[0],dims[1],dims[2]))
		output = np.concatenate((output,image),0)
	return output.astype(np.uint8)

def display(image,box):
	plt.figure()
	plt.imshow(image)
	ax = plt.gca()
	ax.add_patch(Rectangle((box[0],box[1]),box[2],box[3],ec='r',fc='none'))
	plt.show()

all_images = load_all('/home/lpang/Documents/GitHub/LaserTurret/ComputerVision/Images')

data = pd.read_csv('/home/lpang/Documents/GitHub/LaserTurret/ComputerVision/Data/data.csv')
data['Data'] = data['Data'].apply(lambda x:np.asarray(eval(str(x))[1:]))
y = np.empty((0,4))
for row in data['Data']:
	y = np.concatenate((y,np.reshape(row,(1,-1))))
	
n_train = int(len(all_images)*0.75)
X_train = all_images[:n_train]
X_val = all_images[n_train:]
Y_train = y[:n_train]
Y_val = y[n_train:]
	
model = load_model('/home/lpang/Documents/GitHub/LaserTurret/ComputerVision/Data/model.h5')
	
results = model.predict(X_train)
for i in range(np.shape(results)[0]):
	print(results[i])
	#display(X_val[i],results[i])