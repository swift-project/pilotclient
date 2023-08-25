# SPDX-FileCopyrightText: Copyright (C) 2018 swift Project Community / Contributors
# SPDX-License-Identifier: GPL-3.0-or-later OR LicenseRef-swift-pilot-client-1

import os
import sys
import platform
import requests

def upload_symbol_files(symbols_package):
    print('Uploading symbols')
    url = 'https://swift-project.sp.backtrace.io:6098/post'
    token = '44166372034af002070a1f0ae3c8dfd8632e0210b36577c5269626fe5b87b2f6'

    print ('Uploading ' + symbols_package)
    data = open(symbols_package, 'rb').read()
    params = (
        ('format', 'symbols'),
        ('token', token),
    )
    r = requests.post(url, params=params, data=data)
    r.raise_for_status()

def print_help():
    print('Usage: symbolupload.py <symbol path>')


def main(argv):
    print len(argv)
    if len(argv) != 1:
        print_help()
        sys.exit(2)

    symbols_package = sys.argv[1]
    upload_symbol_files(symbols_package)


if __name__ == '__main__':
    main(sys.argv[1:])
