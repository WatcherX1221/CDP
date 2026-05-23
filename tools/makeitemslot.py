# Definitions
def bytecon(integer): # Converts a denary integer to a byte-long binary integer returned as a string.
    integer=integer%256
    byte=""
    for x in range(8):
        if integer>=2**(7-x):
            integer-=(2**(7-x))
            byte+="1"
        else: 
            byte+="0"
    return byte
# Main Progam
#print(bytearrayconvert("00110011010101010000111100100101"))
# Opens the WSZST file to get data
filename=input("Enter TXT to convert to ItemSlot.bin: ")
file=open(filename,"r")
filedata=[]
itemdata=[]
for line in enumerate(file):
    filedata.append([])
    x = len(str(line))
    reading = ""
    while x > 0:
        if str(line)[len(line)-x] == "#":
            if reading!="": filedata[len(filedata)-1].append(reading)
            x=0
        elif x<=1:
            if reading!="": filedata[len(filedata)-1].append(reading)
        elif str(line)[len(line)-x] == " ":
            if reading!="": filedata[len(filedata)-1].append(reading)
            reading=""
        reading=reading+str(line)[len(line)-x]
        if reading==" ":
            reading=""
        x-=1
    for x in range(2):
        filedata[len(filedata)-1].pop(0)
    if filedata[len(filedata)-1]==[]:
        filedata.pop(len(filedata)-1)
file.close()
print(filedata)
# Converts data to represent the expected itemslot data.
for x in range(len(filedata)): # x is filedata line
    read=False
    try:
        int(filedata[x][0])
        read=True
    except:
        read=False
        y=0 # y is table row
    if read:
        if int(filedata[x][0])==y: # if in table
            if y==0: itemdata.append([])
            itemdata[len(itemdata)-1].append([])
            y+=1
            for z in range(1, int(len(filedata[x]))): # z is column | itemdata uses z-1
                if filedata[x][z]=='.': itemdata[len(itemdata)-1][len(itemdata[len(itemdata)-1])-1].append(0) # set 0 cell data
                else: itemdata[len(itemdata)-1][len(itemdata[len(itemdata)-1])-1].append(int(filedata[x][z])) # set non-0 cell data
print(itemdata)
# Converts itemslot data to bytearray.
bytecollect=[]
bytecollect.append(len(itemdata)) # write number of tables
for x in range(len(itemdata)): # for every table
    bytecollect.append(len(itemdata[x][0])) # write number of columns in table
    bytecollect.append(len(itemdata[x])) # write number of rows in table
    for y in range(len(itemdata[x])): # for every row?
        for z in range(len(itemdata[x][y])): # for every column?
            bytecollect.append(itemdata[x][y][z]) # write cell data
print(bytecollect)
# Writes the file
filename=input("Enter ItemSlot.bin name: ")
file=open(filename,"bw")
file.write(bytearray(bytecollect))
file.close()
