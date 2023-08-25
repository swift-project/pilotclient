# SPDX-FileCopyrightText: Copyright (C) 2016 swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

#!/usr/bin/env python

import glob
import os
import fnmatch

mappings = []

scriptDir = os.getcwd()

os.chdir("../src")

# Find all headers
subfolders = [ 'blackmisc', 'blacksound', 'blackinput', 'blackcore', 'blackgui' ]

for subfolder in subfolders:
    for root, dirnames, filenames in os.walk(subfolder):
        for header in fnmatch.filter(filenames, '*.h'):
            publicInc = root + '/' + header
            tokens = root.split('/')
            tokens.pop(0)
            tokens.append(header)
            while tokens:
                privateInc = '/'.join(tokens)
                mapping = (privateInc, publicInc)
                mappings.append(mapping)
                tokens.pop(0)

# Create mapping file
filename = scriptDir + '/swift.imp'
if os.path.exists(filename):
    os.remove(filename)

print(filename)
imp = open(filename, 'w')
imp.write('[\n')
for mapping in mappings:
    imp.write("  { include: [ \"\\\"" + mapping[0] + "\\\"\", \"private\", \"\\\"" + mapping[1] + "\\\"\", \"public\" ] },\n")
imp.write("  { ref: \"qt5.imp\" }\n")
imp.write(']\n')
