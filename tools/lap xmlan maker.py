PANENAMES=[["lap_left01","lap_left02","lap_left03","lap_left04","lap_left05"],["lap_right01","lap_right02","lap_right03","lap_right04","lap_right05"]]
frames=int(input("laps?"))
panes=int(input("panes?"))
filedata='''
<?xml version="1.0" encoding="utf-8"?>
<xmlan version="2.1.12BETA" brlan_version="000a">
<pat1>
	<unk1>0000</unk1>
	<unk5a>0000</unk5a>
	<unk5b>0008</unk5b>
	<isDecendingBind>01</isDecendingBind>
	<padding>01</padding>
	<first>texture_pattern</first>
	<seconds />
</pat1>
<pai1 framesize="'''
filedata+=str(frames+1)
filedata+='''" flags="00">
	<timg name="tt_d_number_3d_00.tpl" />
	<timg name="tt_d_number_3d_01.tpl" />
	<timg name="tt_d_number_3d_02.tpl" />
	<timg name="tt_d_number_3d_03.tpl" />
	<timg name="tt_d_number_3d_04.tpl" />
	<timg name="tt_d_number_3d_05.tpl" />
	<timg name="tt_d_number_3d_06.tpl" />
	<timg name="tt_d_number_3d_07.tpl" />
	<timg name="tt_d_number_3d_08.tpl" />
	<timg name="tt_d_number_3d_09.tpl" />
	<timg name="tt_d_number_3d_none.tpl" />
'''
for p in range(int(panes)):
    for i in range(2):
        filedata+='''<pane name="'''
        filedata+=PANENAMES[i][p]
        filedata+='''" type="1">
<tag type="RLTP"><entry type1="0" type2="PaletteZero">'''
        x=0
        y=-1
        z=False
        for f in range(int(frames+1)):
            filedata+='''<pair><data1>'''
            filedata+=str(f)
            filedata+='''</data1><data2>'''
            y+=1
            if y==10**p:
                y=0
                x+=1
                z=True
            if x==10:
                x=0
            if z:
                filedata+=str(x)
            else:
                filedata+="A"
            filedata+='''</data2><padding>0000</padding></pair>'''
        filedata+='''</entry></tag></pane>'''
    print(p)
filedata+='''</pai1></xmlan>'''

file=open("fap.xmlan","w")
file.write(filedata)
file.close()
print("done!")
