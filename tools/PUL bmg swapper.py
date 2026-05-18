print("Pulsar BMG swapper v1.2.BlFa3\nby Blockface3\n")
# VERSION v1.2.BlFa3
# Release notes
# === v1.2.BlFa3 ===
# 14/05/2026
# - Added a BMG extractor to
#   complement the swapper
# - Program now prompts on bootup
#   whether to extract BMG or
#   swap BMG
# === v1.1.BlFa3 ===
# 14/05/2026
# - Fixed an issue where the CUPS offset was
#   incorrectly set to 0x8 instead of 0xC
#   (Would have caused issues with
#    atypical config.pul section orders)
# === v1.0.BlFa3 ===
# 14/05/2026
# - First Release

# Definitions
def tryopen(accessname):
    # Attempts to open a file continuously until it succeeds, and returns the filename.
    # The successfully opened file will be closed.
    newname=str(accessname) # Define as accessname (need str here because of python jank)
    while True:
        try: # try to open file
            file=open(newname,"r")
            break
        except: # if file open fails enter new filename
            toprint="Could not find "+newname+".\nEnter "+accessname+" path:\n" # python input jank
            newname=input(toprint)
    file.close() # close file
    return newname # return filename ready for use
def backupfile(filename):
    # Creates a backup file so data may be restored
    file=open(filename,"br")
    data=file.read()
    file.close()
    backupfilename=filename+".bkp"
    freebackup=2
    try: # check for backup file
        file=open(str(filename)+".bkp","br")
        while True: # increment freebackup until free backup file found
            file.close()
            try: # Try to get file
                file=open(backupfilename+str(freebackup),"br")
                freebackup+=1 # If it succeeds try to get next file
            except: # If it fails then break the loop and make the file
                file.close()
                break
        file=open(backupfilename+str(freebackup),"bw")
        print("Backup config created:",backupfilename+str(freebackup))
    except: # if no backup file found make one without any name extensions
        file=open(str(filename)+".bkp","bw")
        print("Backup config created:"+backupfilename)
    file.write(data)
    file.close
def intbytes(data,offset,length):
    # Converts a section of data to an integer, and returns said integer.
    # Used because getting multiple bytes simultaneously results in different formatting
    output=0 # Define output as int
    for x in range(length): # For every byte
        output+=int(data[offset+x])*(2**(8*(length-x-1))) # Add converted byte of data
    return output # return full integer
def swapbytes(data,offset,length,swap):
    # Swaps a section of bytes from binary data with other bytes of binary data and returns the remaining data
    output=bytes() # Define output as bytes
    output+=bytes(data[0:offset]) # Append bytes until offset
    output+=bytes(swap) # Append swapped bytes
    output+=bytes(data[length+offset:len(data)]) # Append bytes to end of file
    return output
def convbytes(number):
    # Converts numbers to binary formatted as uint bytes
    output=[] # Define output as list
    x=0 # Number of bytes
    while True: # Find number of bytes needed to store value
        if number < 2**(x*8):
            break
        else:
            x+=1
    for y in range(x): # For every byte needed
        output.append(0)
        for z in range(8): # For every bit in the byte
            if 2**(x*8-y*8-z-1)<=number: # If bit should be 1
                number-=2**(x*8-y*8-z-1) # Subtract bit value
                output[y]+=2**(7-z) # Add bit value of byte to int
    return bytes(output) # Convert byte list to bytes
def padbytes(data,length):
    # Appends padding to data and returns it
    # Can also trim binary data if it doesn't fit the specified length
    output=bytes(length) # Define output as length of bytes because padding is jank
    if length>len(data): # If appending data
        output=output[0:length-len(data)]+data # Add data to the end of padding
    else: # If trimming data
        output=data[len(data)-length:len(data)] # Trim data to fit length
    return output


while True:
    mode=input("Would you like to SWAP or EXTRACT? [S/E]\n") # Ask mode choice
    if mode=="S" or mode=="SWAP": # we be a little lenient
        mode="S"
        break
    elif mode=="E" or mode=="EXTRACT":
        mode="E"
        break
print("") # print one line to look fancy!!
# Swap Program
if mode=="S":
    BMGfile=tryopen("PulsarBMG.bmg") # get BMG text file
    CONFIGfile=tryopen("Config.pul") # get pulsar binary
    # Get all file data
    print("Retrieving file info...") # update user on status
    file=open(BMGfile,"br")
    bmgdata=file.read()
    file.close
    file=open(CONFIGfile,"br")
    configdata=file.read()
    file.close
    # Backup config.pul
    print("Backing up config file...") # update user on status
    backupfile(CONFIGfile)
    # Remove BMG file data from Config.pul
    bmgoffset=intbytes(configdata,0x10,4) # Get BMG offset in Config
    # Important to check all other offsets of the file, in case the new BMGs offset them!
    infooffset=intbytes(configdata,0x8,4) # Get INFO offset in Config
    cupsoffset=intbytes(configdata,0xC,4) # Get CUPS offset in Config
    # Swap file data
    print("Swapping file data...") # update user on status
    configdata=swapbytes(configdata,bmgoffset,intbytes(configdata,bmgoffset+0x8,4),bmgdata) # Swap BMG data from Config with PulsarBMG
    if infooffset>bmgoffset or cupsoffset>bmgoffset: # update user on status
        print("Resolving offset conflicts...")
    if infooffset>bmgoffset:
        configdata=swapbytes(configdata,0x8,4,padbytes(convbytes(bmgoffset+infooffset),4))
    if cupsoffset>bmgoffset:
        configdata=swapbytes(configdata,0xC,4,padbytes(convbytes(bmgoffset+cupsoffset),4))
    print("Saving file...") # update user on status
    file=open(CONFIGfile,"bw")
    file.write(configdata)
    file.close()
    print(input("\nDone!\nThis program may now be closed :)"))
# Extract Program
if mode=="E":
    CONFIGfile=tryopen("Config.pul") # get pulsar binary
    print("Retrieving file info...") # update user on status
    file=open(CONFIGfile,"br")
    configdata=file.read()
    file.close
    # Remove BMG file data from Config.pul
    bmgoffset=intbytes(configdata,0x10,4) # Get BMG offset in Config
    bmgdata=configdata[bmgoffset:intbytes(configdata,bmgoffset+0x8,4)+bmgoffset]
    # Save file
    BMGfile="PulsarBMG.bmg"
    try:
        file=open(BMGfile,"br") # See if a file with this name already exists before proceeding
        BMGfile=input("Enter the name for your BMG file.\n(WARNING: FILES WITH THE SAME NAME WILL BE OVERWRITTEN!!)\nBMG export name: ")
        print("Saving file...") # update user on status
    except: # If file doesn't exist use that name.
        print("Saving file...") # update user on status
    file=open(BMGfile,"bw")
    file.write(bmgdata)
    file.close()
    print("\nFile saved as",BMGfile+"!\nThis program may now be closed :)")
