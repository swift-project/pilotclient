import os
import sys
import platform
import requests

def upload_symbol_files(symbol_path, version):
    print('Uploading symbols')
    url = 'http://crashreports.swift-project.org/symfiles'

    symbol_files = [os.path.join(root, name)
                    for root, dirs, files in os.walk(symbol_path)
                    for name in files
                    if name.endswith('.sym')]

    for symbol_file in symbol_files:
        print ('Uploading ' + symbol_file)
        files = [
            ('symfile', open(symbol_file, 'rb')),
        ]
        data = {'release': version}
        r = requests.post(url, files=files, data=data)
        r.raise_for_status()

def print_help():
    print('Usage: symbolupload.py <symbol path> <version>')


def main(argv):
    print len(argv)
    if len(argv) != 2:
        print_help()
        sys.exit(2)

    symbol_path = sys.argv[1]
    version = sys.argv[2]
    upload_symbol_files(symbol_path, version)


if __name__ == '__main__':
    main(sys.argv[1:])
