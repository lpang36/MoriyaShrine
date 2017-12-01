#!usr/bin/ash

#Yay fun ASCII art here
echo "#############################################################################"
echo "#                       WELCOME TO THE MORIYA SHRINE                        #"
echo "#     *****\_____________________________________________________/*****     #"
echo "#      /#############################################################\      #"
echo "#    /#################################################################\    #"
echo "#            ###################################################            #"
echo "#                      ######       ######       ######                     #"
echo "#                      ######        ####        ######                     #"
echo "#           ***\      <<|##|>>        ##        <<|##|>>     /***           #"
echo "#             #################################################             #"
echo "#             #################################################             #"
echo "#                       |##|        .,            |##|                      #"
echo "#                       |##|  ,'',.'  '.,         |##|                      #"
echo "#                  _._  |##|,'_._        ',  _._  |##|  _._                 #"
echo "#                 /###\'|##| /###\         '/###\ |##| /###\                #"
echo "#       ,''',.   ,'|#|  =##=  |#|            |#|',=##=  |#|   _            ,#"
echo "#-, ..''      '.' ################          ################,' '',.'',. .,' #"
echo "#__'______________################__________################___________'____#"
echo "#############################################################################"

#Take picture using fswebcam
#Sets colourspace to YUYV (otherwise it crashes with the Logitech webcam)
#Disables the default banner (which just overlays date/time/etc onto image)
#Stores the image as "image.jpg"
fswebcam -p YUYV --no-banner -r 300x300 image.jpg

#Clear the logfile from the previous run
rm logfile.txt

#Call the main program (Image processor) with parameters 1 and 2
./Main $1 $2

#Set up script to read from output.txt
filename="output.txt"

#Create a variable to use as a counter
COUNTERVAR=0

#Iterates through the lines of the output file
while read -r line
do
	name="$line"
	#Writes the value to the corresponding PWM pin
	#PWM set to be at 500Hz, countervar sets which pin, name sets value
	fast-gpio pwm $COUNTERVAR 500  $name
	echo "Writing $name to $COUNTERVAR"
	COUNTERVAR=$(expr $COUNTERVAR + 1)
done < "output.txt"

#Open logfile afterwards for demo
cat logfile.txt
