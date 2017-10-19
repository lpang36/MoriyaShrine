from PIL import Image
import os
import pandas as pd
from keras.models import Sequential
from keras.layers import Dense, Activation, Flatten
from keras.layers import Convolution2D,MaxPooling2D

def load_image(filename):
	im = Image.open(filename)
	pixels = np.asarray(im.getdata())
	return pixels

def load_all(foldername):
	return [load_image(os.path.join(foldername,filename)) for filename in os.listdir(foldername)]

all_images = load_all('/home/lpang/Documents/GitHub/LaserTurret/ComputerVision/Images')

data = pd.read_csv('/home/lpang/Documents/GitHub/LaserTurret/ComputerVision/Data/data.csv')
data['Data'] = data['Data'].apply(lambda x:eval(str(x))[1:])
y = data['Data'].values

# Model Architecture
model = Sequential()

model.add(Convolution2D(32, (8, 8), activation='relu', input_shape=(None,None,3)))
model.add(MaxPooling2D(pool_size=(2,2)))

model.add(Convolution2D(32, (8, 8), activation='relu'))
model.add(MaxPooling2D(pool_size=(2,2)))

model.add(Flatten())
model.add(Dense(4))

# Compile model
model.compile(loss='mean_squared_error',optimizer='adam')

n_train = int(len(all_images)*0.75)
X_train = all_images[:n_train]
X_val = all_images[n_train:]
Y_train = y[:n_train]
Y_val = y[n_train:]

model.fit(x=X_train,y=Y_train,batch_size=16,epochs=10,validation_data=(X_val,Y_val))



