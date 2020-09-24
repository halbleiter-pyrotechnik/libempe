#!/usr/bin/python3
#
# This program searches for C source code and header files
# recursively and performs several operations on them:
#
# - Converts relative paths to absolute,
# - checks if the included files exists
# - and if not, suggests path changes to existing files.
#
# It is expected to be run from the project root folder, like so:
#
#   project_root/ $ ./<path-to-libempe>/include_wizard.py
#

import sys, os
from subprocess import check_output
from shlex import split
import re

# Optionally exclude certain files from being checked (using regular expressions)
excludeFiles = [".*/Drivers/.*"]

# Primary path where to expect included project files, relative from the project root
includeDirectoryPrefix = "src"

# TODO: Extract from Makefiles
includeDirectories = [
    "src",
    "lib/empe",
    "Drivers/CMSIS/Include",
    "Drivers/CMSIS/Device/ST/STM32F4xx/Include",
    "Drivers/STM32F4xx_HAL_Driver/Inc",
    "/usr/lib/gcc/arm-none-eabi/7.3.1/include",
    "/usr/include/newlib"
    ]

# Loglevel
LOG_NONE = 0
LOG_ERROR = 1
LOG_WARNING = 2
LOG_DEBUG = 3
LOG_EXTENSIVE = 4
loglevel = LOG_WARNING


def log(level, message):
    if loglevel >= level:
        print(message)


#
# Use the console tool find to find files by pattern
#
def findFile(pattern, path=".", ignoreCase=True):
    stdout = check_output(split("find \"{:s}\" -{:s}name \"{:s}\"".format(path, ("i" if ignoreCase else ""), pattern)), encoding="utf-8")
    lines = stdout.split("\n")
    files = []
    for line in lines:
        if len(line.strip()) == 0:
            continue
        files += [line]
    return files


#
# Find the maximum overlapping path between two filenames
#
def findFilePathOverlap(file1, file2):
    log(LOG_EXTENSIVE, "Searching for path overlap between {:s} and {:s} ...".format(file1, file2))
    # print(os.path.commonpath([file1, file2]))
    seg1 = file1.split("/")
    seg2 = file2.split("/")
    length = min(len(seg1), len(seg2))
    index = 0
    while index < length:
        if seg1[index] != seg2[index]:
            # Mismatch
            break
        index += 1
    log(LOG_EXTENSIVE, "First mismatch at index {:d}: {:s}".format(index, seg1[index]))
    if index == 0:
        overlap = ""
    else:
        overlap = "/".join(seg1[:index]) + "/"
    log(LOG_EXTENSIVE, "Overlap: {:s}".format(overlap))
    return overlap


#
# Replaces an old statement in a file with a new statement
#
def replaceInFile(filename, old, new):
    log(LOG_EXTENSIVE, "Replacing {:s} with {:s} in {:s} ...".format(old, new, filename))
    f = open(filename, "r")
    content = f.read()
    f.close()
    f = open(filename, "w")
    f.write(content.replace(old, new))
    f.close()


#
# Evaluate the include path and modify if necessary
#
def processInclude(filename, match):
    include = match.group(0)
    path = match.group(1)
    log(LOG_EXTENSIVE, "Processing: {:s}".format(include))

    # Check included file's existence
    filePath = os.path.dirname(filename)
    ifname = os.path.basename(path)
    possiblePaths = []
    possiblePaths += [path]
    possiblePaths += [os.path.join(filePath, ifname)]
    possiblePaths += [os.path.join(includeDirectoryPrefix, path)]
    found = False
    for includePath in includeDirectories:
        p = os.path.join(includePath, path)
        possiblePaths += [p]
    for searchPath in possiblePaths:
        if os.path.exists(searchPath):
            found = True
            log(LOG_DEBUG, "Found {:s} in {:s}.".format(path, searchPath))
            break
    if not found:
        print("In file \"{:s}\": Included file \"{:s}\" not found.".format(filename, path))

        basename = os.path.basename(path)
        log(LOG_EXTENSIVE, "Searching for {:s} ...".format(basename))
        alternatives = findFile(basename, ignoreCase=False)
        if len(alternatives) > 0:
            # Add options with paths relative to the source file
            index = 0
            while index < len(alternatives):
                alternative = alternatives[index]
                # Does the alternative included file's path partially overlap with the file's path?
                # overlap = findFilePathOverlap(filename, alternative)
                # if len(overlap) > 0:
                #    reducedPath = alternative[len(overlap):]
                #    log(LOG_EXTENSIVE, "Stripped overlapping path: {:s}".format(reducedPath))
                #    alternatives.insert(index, reducedPath)
                #    index += 1
                # Doesn't work. Example:
                """
Processing ./tests/control/transformations/alpha-beta/test.c ...
Included file not found: utils/gnuplot.h
Found 3 alternatives:
Do you wish to change this to utils/gnuplot.h (y/n/s/q)? q
Quit.
"""
                # Remove the include directories
                for d in includeDirectories:
                    d += "/"
                    if alternative.find("./" + d) == 0:
                        d = "./" + d
                    if alternative.find(d) == 0:
                        newA = alternative[len(d):]
                        log(LOG_EXTENSIVE, "Stripped include path \"{:s}\" from \"{:s}\". Got \"{:s}\".".format(d, alternative, newA))
                        alternatives.remove(alternative)
                        alternatives.insert(index, newA)
                        alternative = newA
                # Remove ./ prefix
                if (len(alternative) > 2) and (alternative[:2] == "./"):
                    newA = alternative[2:]
                    log(LOG_EXTENSIVE, "Stripped \"./\" from \"{:s}\". Got \"{:s}\".".format(alternative, newA))
                    alternatives.remove(alternative)
                    alternatives.insert(index, newA)
                    alternative = newA
                index += 1

            # Let the user choose an alternative
            l = len(alternatives)
            print("Found {:d} alternative{:s}:".format(l, "s" if (l > 1) else ""))
            for alternative in alternatives:
                answear = None
                while not (answear in ["y", "n", "s", "q"]):
                    print("Do you wish to change this to {:s} (y/n/s/q)? ".format(alternative), end="", flush=True)
                    answear = input().lower()
                if answear == "y":
                    newPath = alternative
                    newInclude = "#include \"{:s}\"".format(newPath)
                    replaceInFile(filename, include, newInclude)
                    include = newInclude
                    path = newPath
                    print("Changed.")
                    break
                elif answear == "s":
                    # Skip this file
                    print("Skipping.")
                    break
                elif answear == "q":
                    # Quit program
                    print("Quit.")
                    sys.exit()
        else:
            log(LOG_EXTENSIVE, "{:s} could not be found.".format(ifname))

    # Convert from relative to absolute
    pos = path.find(includeDirectoryPrefix + "/")
    if pos > -1:
        log(LOG_EXTENSIVE, "Modifying:")
        newPath = path[pos + 4:]
        log(LOG_EXTENSIVE, "{:s} => {:s}".format(path, newPath))
        newInclude = "#include \"{:s}\"".format(newPath)
        log(LOG_EXTENSIVE, "{:s} => {:s}".format(include, newInclude))
        replaceInFile(filename, include, newInclude)

        # Use updated values in the following processing steps:
        include = newInclude
        path = newPath


#
# Open a file and evaluate all #include statements
#
def processFile(filename):
    # Read file
    log(LOG_DEBUG, "Processing {:s} ...".format(filename))
    try:
        f = open(filename)
        content = f.read()
        f.close()
    except:
        log(LOG_ERROR, "Error: Unable to read file {:s}".format(filename))
        return

    # Evaluate all includes
    # content = "abc 123 #include \"\" xyz #include \"hello\" jkl #include<world>"
    r = re.compile("#include[ \t]*[\"<]([a-zA-Z0-9\_\-\.\/]*)[\">]")
    for match in r.finditer(content):
        processInclude(filename, match)


if __name__ == "__main__":
    # Populate expression array
    skipExpressions = []
    for s in excludeFiles:
        skipExpressions += [re.compile(s)]

    # Find all *.c
    results1 = findFile("*.c")
    # Find all *.c
    results2 = findFile("*.h")

    results = results1 + results2
    log(LOG_EXTENSIVE, "Found {:d} source files.".format(len(results)))
    files = []
    for result in results:
        for e in skipExpressions:
            if e.match(result):
                log(LOG_EXTENSIVE, "Skipping excluded file {:s}.".format(result))
                continue
        files += [result]

    # Process all files
    for f in files:
        processFile(f)
