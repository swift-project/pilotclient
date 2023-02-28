#!/bin/env python

# Copyright (C) 2017
# swift Project Community/Contributors
#
# This file is part of swift project. It is subject to the license terms in the LICENSE file found in the top-level
# directory of this distribution and at http://www.swift-project.org/license.html. No part of swift project,
# including this file, may be copied, modified, propagated, or distributed except according to the terms
# contained in the LICENSE file.

import json
import os
import datetime
import zlib
import base64
import io
import ssl

try:
    from urllib.request import urlopen
    from urllib.error import HTTPError, URLError
except ImportError:
    from urllib2 import urlopen, HTTPError, URLError


class DbInfo:
    def __init__(self):
        self._table_name = ''
        self._file_name = ''
        self._last_updated = datetime.datetime(datetime.MINYEAR, 1, 1)

    @property
    def table_name(self):
        return self._table_name

    @table_name.setter
    def table_name(self, table_name):
        self._table_name = table_name

    @property
    def file_name(self):
        return self._file_name

    @file_name.setter
    def file_name(self, file_name):
        self._file_name = file_name

    @property
    def last_updated(self):
        return self._last_updated

    @last_updated.setter
    def last_updated(self, last_updated):
        self._last_updated = last_updated


class DbInfosLoader:
    def __init__(self, version):
        self.__filename = 'dbinfo.json'
        self.__version = version
        self.__entities_to_file_names = {
            'aircrafticao': 'aircrafticao.json',
            'aircraftmodel': 'models.json',
            'airlineicao': 'airlineicao.json',
            'airport': 'airports.json',
            'country': 'countries.json',
            'distributor': 'distributors.json',
            'livery': 'liveries.json'
        }

    def convert_json_to_db_infos(self, j):
        db_infos = []
        data = j['data']
        for db in data:
            db_info = DbInfo()
            db_info.table_name = db['tablename']
            if db['tablename'] in self.entities_to_file_names:
                db_info.file_name = self.entities_to_file_names[db['tablename']]
                last_updated = datetime.datetime.strptime(db['lastupdated'], '%Y-%m-%d %H:%M:%S')
                db_info.last_updated = last_updated
                db_infos.append(db_info)
        return db_infos

    @property
    def file_name(self):
        return self.__filename

    @property
    def version(self):
        return self.__version

    @property
    def entities_to_file_names(self):
        return self.__entities_to_file_names

    def filename_by_table_name(self, db):
        return self.entities_to_file_names[db]


class DbInfosLoaderLocal(DbInfosLoader):
    def __init__(self, version, target_path):
        DbInfosLoader.__init__(self, version)
        self.__db_infos = []
        file_path = os.path.abspath(os.path.join(target_path, 'shared', 'dbdata', self.file_name))
        if os.path.exists(file_path):
            with io.open(file_path, 'r', encoding='utf-8') as f:
                j = json.load(f)
            self.__db_infos = self.convert_json_to_db_infos(j)

    def last_updated_by_table_name(self, table_name):
        for db in self.__db_infos:
            if db.table_name == table_name:
                return db.last_updated


class DbInfosLoaderRemote(DbInfosLoader):
    def __init__(self, version):
        DbInfosLoader.__init__(self, version)

    def download_from(self, host):
        # Open the url
        url = host + '/shared/' + self.version + '/dbdata/' + self.file_name
        try:
            ctx = ssl.create_default_context()
            ctx.check_hostname = False
            ctx.verify_mode = ssl.CERT_NONE
            response = urlopen(url, context=ctx)
            timestamp = datetime.datetime.strptime(response.headers['last-modified'], '%a, %d %b %Y %H:%M:%S GMT')
            print("Getting db info " + url)
            content = response.read().decode('utf-8')
            j = json.loads(content)
            db_infos = self.convert_json_to_db_infos(j)
            return db_infos, j, timestamp
        except HTTPError as e:
            error = "HTTP Error: " + str(e.code) + ' ' + url
            print(error)
        except URLError as e:
            error = "HTTP Error: " + e.reason + ' ' + url
            print(error)


def write_text_to_file(file_path, text_content, timestamp):
    if text_content:
        # Normalize the file endings
        text_content = text_content.replace('\r\n', '\n')
        with io.open(file_path, 'w', encoding='utf-8') as f:
            f.write(text_content)
        atime = mtime = (timestamp - datetime.datetime.utcfromtimestamp(0)).total_seconds()
        os.utime(file_path, (atime, mtime))


def write_json_to_file(file_path, json_content, timestamp):
    if json_content:
        with io.open(file_path, 'w', encoding='utf-8') as f:
            f.write(json.dumps(json_content, ensure_ascii=False))
        atime = mtime = (timestamp - datetime.datetime.utcfromtimestamp(0)).total_seconds()
        os.utime(file_path, (atime, mtime))


class BaseSync:
    def __init__(self, host, version, target_path):
        self.__host = host
        self.__version = version
        self.__target_path = target_path
        if self.__target_path is None:
            self.__target_path = ''
        pass

    @staticmethod
    def __uncompress_content(compressed_content):
        compressed_content = base64.b64decode(compressed_content)
        content = zlib.decompress(compressed_content, 0)
        return content

    @property
    def version(self):
        return self.__version

    @property
    def host(self):
        return self.__host

    @property
    def target_path(self):
        return self.__target_path

    def sync_file(self, data_type, file_name):
        path = os.path.join(self.target_path, 'shared', data_type)
        if not os.path.exists(path):
            os.makedirs(path)

        url = self.host + '/shared/' + self.version + '/' + data_type + '/' + file_name
        try:
            ctx = ssl.create_default_context()
            ctx.check_hostname = False
            ctx.verify_mode = ssl.CERT_NONE
            response = urlopen(url, context=ctx)
            timestamp = datetime.datetime.strptime(response.headers['last-modified'], '%a, %d %b %Y %H:%M:%S GMT')
            print("Syncing file " + url)
            file_path = os.path.join(self.target_path, 'shared', data_type, file_name)
            content = response.read().decode('utf-8')
            write_text_to_file(file_path, content, timestamp)

        except HTTPError as e:
            error = "HTTP Error: " + str(e.code) + ' ' + url
            print(error)
        except URLError as e:
            error = "HTTP Error: " + e.reason + ' ' + url
            print(error)


class DbDataSync(BaseSync):
    def __init__(self, host, version, target_path):
        BaseSync.__init__(self, host, version, target_path)
        self.loader_local = DbInfosLoaderLocal(version, target_path)
        self.loader_remote = DbInfosLoaderRemote(version)

    def sync(self):
        db_infos, j, timestamp = self.loader_remote.download_from(self.host)
        db_updated = False
        for db_info in db_infos:
            remote_last_updated = db_info.last_updated
            table_name = db_info.table_name
            file_name = self.loader_remote.filename_by_table_name(table_name)
            local_last_updated = self.loader_local.last_updated_by_table_name(table_name)
            if not local_last_updated or remote_last_updated > local_last_updated:
                self.sync_file('dbdata', db_info.file_name)
                db_updated = True
            else:
                print(file_name + " is up to date.")

        if db_updated:
            print('DB data successfully updated')

        file_path = os.path.join(self.target_path, 'shared', 'dbdata', self.loader_local.file_name)
        write_json_to_file(file_path, j, timestamp)


class UpdateInfoSync(BaseSync):
    def __init__(self, host, version, target_path):
        BaseSync.__init__(self, host, version, target_path)

    def sync(self):
        self.sync_file('updateinfo', 'distribution.json')


def update_shared(host, version, target_path):
    DbDataSync(host, version, target_path).sync()
    UpdateInfoSync(host, version, target_path).sync()


def main():
    host = 'https://datastore.swift-project.org'
    version = '0.7.0'
    target_path = os.path.abspath(os.curdir)

    DbDataSync(host, version, target_path).sync()
    UpdateInfoSync(host, version, target_path).sync()


if __name__ == '__main__':
    main()
