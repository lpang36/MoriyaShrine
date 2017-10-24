from PIL import Image
import os
import pandas as pd
import numpy as np
from keras.models import Sequential
from keras.layers import Dense, Activation, Flatten
from keras.layers import Convolution2D,MaxPooling2D
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
import h5py

dims = (300,400,3)
num_images=100

def load_image(filename):
	im = Image.open(filename)
	pixels = np.asarray(im.getdata())
	return pixels

def load_all(foldername):
	output = np.empty((0,dims[0],dims[1],dims[2]))
	for i in range(0,num_images):
		image = np.reshape(load_image(os.path.join(foldername,str(i)+'.png')),(-1,dims[0],dims[1],dims[2]))
		output = np.concatenate((output,image),0)
	return output.astype(np.uint8)

def display(image,box):
	plt.figure()
	plt.imshow(image)
	ax = plt.gca()
	ax.add_patch(Rectangle((box[0],box[1]),box[2],box[3],ec='r',fc='none'))
	plt.show()

data = pd.read_csv('/home/lpang/Documents/GitHub/imggen/data/data.csv')
data['Data'] = data['Data'].apply(lambda x:np.asarray(eval(str(x))[1:]))
y = np.empty((0,4))
for row in data['Data']:
	y = np.concatenate((y,np.reshape(row,(1,-1))))

all_images = load_all('/home/lpang/Documents/GitHub/imggen/images')

# Model Architecture
model = Sequential()

model.add(Convolution2D(32, (5, 5), activation='relu', input_shape=dims))
model.add(MaxPooling2D(pool_size=(2,2)))

model.add(Convolution2D(64, (3, 3), activation='relu'))
model.add(MaxPooling2D(pool_size=(2,2)))

model.add(Convolution2D(128, (2, 2), activation='relu'))
model.add(MaxPooling2D(pool_size=(2,2)))

model.add(Convolution2D(128, (2, 2), activation='relu'))
model.add(MaxPooling2D(pool_size=(2,2)))

model.add(Convolution2D(128, (2, 2), activation='relu'))
model.add(MaxPooling2D(pool_size=(2,2)))

model.add(Convolution2D(128, (2, 2), activation='relu'))
model.add(MaxPooling2D(pool_size=(2,2)))

model.add(Flatten())
model.add(Dense(128))
model.add(Dense(4))

# Compile model
model.compile(loss='mean_squared_error',optimizer='adam')

n_train = int(len(all_images)*0.75)
X_train = all_images[:n_train]
X_val = all_images[n_train:]
Y_train = y[:n_train]
Y_val = y[n_train:]

model.fit(x=X_train,y=Y_train,batch_size=16,epochs=10,validation_data=(X_val,Y_val))

model.save('/home/lpang/Documents/GitHub/LaserTurret/ComputerVision/Data/model.h5')
results = model.predict(X_val)
for i in range(np.shape(results)[0]):
	display(X_val[i],results[i])