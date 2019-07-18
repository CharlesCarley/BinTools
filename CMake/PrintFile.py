import sys,os
if (len(sys.argv) == 1):
    script = os.path.basename(sys.argv[0])
    print ("Usage:\n\t" + script + " input_file")
else:
    ifile = open(sys.argv[1], 'r')
    lines = ifile.readlines()
    strip = []
    max = -1

    for line in lines:
        line = line.replace("\r", "")
        line = line.replace("\n", "")
        strip.append(line)
        if (max < len(line)):
            max = len(line)
    for line in strip:
        ll = max - len(line)
        spc = "".ljust(ll, ' ')
        print("    std::cout <<\"" + line + spc+"\\n\";") 

    ifile.close()



