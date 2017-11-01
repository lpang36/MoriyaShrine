import numpy as np
import pandas as pd

cols = range(0,3)
with open('/home/lpang/Documents/GitHub/LaserTurret/ComputerVision/Data/training_goose.xml','w') as ofile:
	ofile.write('<?xml version=\'1.0\' encoding=\'ISO-8859-1\'?>\n\
<?xml-stylesheet type=\'text/xsl\' href=\'image_metadata_stylesheet.xsl\'?>\n\
<dataset>\n\
<images>\n')
	data = pd.read_csv('/home/lpang/Documents/GitHub/imggen/images_goose/data_goose.csv')
	for i in cols:
		data[str(i+1)] = data[str(i+1)].apply(lambda x:np.asarray(eval(str(x))[1:]))
	count = 0
	for item in data[[str(col+1) for col in cols]].itertuples():
		ofile.write('\t<image file=\'/home/lpang/Documents/GitHub/imggen/images_goose/'+str(count)+'.png\'>\n')
		for i in range(0,3):
			ofile.write('\t\t<box top=\''+str(item[i+1][1])+'\' left=\''+str(item[i+1][0])+'\' width=\''+str(item[i+1][2])+'\' height=\''+str(item[i+1][3])+'\'/>\n')
		ofile.write('\t</image>\n')
		count = count+1
	ofile.write('</images>\n\
</dataset>\n')