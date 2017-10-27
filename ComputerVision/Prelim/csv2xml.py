import numpy as np
import pandas as pd


with open('/home/lpang/Documents/GitHub/LaserTurret/ComputerVision/Data/training_goose.xml','w') as ofile:
	ofile.write('<?xml version=\'1.0\' encoding=\'ISO-8859-1\'?>\n\
<?xml-stylesheet type=\'text/xsl\' href=\'image_metadata_stylesheet.xsl\'?>\n\
<dataset>\n\
<images>\n')
	data = pd.read_csv('/home/lpang/Documents/GitHub/imggen/data/data_goose.csv')
	data['Data'] = data['Data'].apply(lambda x:np.asarray(eval(str(x))[1:]))
	count = 0
	for item in data['Data']:
		ofile.write('\t<image file=\''+str(count)+'.jpg\'>\n\
\t\t<box top=\''+str(item[0])+'\' left=\''+str(item[1])+'\' width=\''+str(item[2])+'\' height=\''+str(item[3])+'\'/>\n\
\t</image>\n')
		count = count+1
	ofile.write('</images>\n\
</dataset>\n')