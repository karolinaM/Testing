import sys
import fileinput
import re

def RemoveFirstNumber(original, filename):
    f_original = open(original, "r")
    f = open(filename, "w")
    for line in f_original.readlines():
        line = re.sub(r" *[0-9]* *", "", line, 1)
        f.write(line)

if __name__ == "__main__":
  RemoveFirstNumber(str(sys.argv[1]), str(sys.argv[2]))

