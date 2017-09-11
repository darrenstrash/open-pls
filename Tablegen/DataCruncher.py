#! /usr/bin/python

import sys
import os
from collections import defaultdict

class data_cruncher(object):
    """docstring for data_cruncher. Reads in data and stores it in a table format."""

    def __init__(self):
        self.experiment_list = defaultdict(lambda : defaultdict(list))
        self.rows = []

    def __repr__(self):
        string = "DataCruncher : "
        for item in self.experiment_list:
            string += "<{} : {}>\n".format(item, str(self.experiment_list[item]))
        return string

    def __str__(self):
        string = "DataCruncher : "
        for item in self.experiment_list:
            string += "<{} : {}>\n".format(item, str(self.experiment_list[item]))
        return string

    @staticmethod
    def all_same(data_array):
        first_element = 1
        first = 1
        for i in data_array:
            if first == 1:
                first_element = i
                first = 0
            if first_element != i:
                return False
        return True

    @staticmethod
    def all_different(data_array):
        value_hash=set()
        for i in data_array:
            value_hash.add(i)
        return len(value_hash) == len(data_array)

    @staticmethod
    def same_element_count(hash_list):
        count = 0
        first_key = 1
        for key in hash_list.keys():
            if first_key == 1:
                first_key = 0
                count = len(hash_list[key])
            if count != len(hash_list[key]):
                return False
        return True

    @staticmethod
    def read_in_data(file_name):
        temp_file = open(file_name, 'r')
        hash_list = defaultdict(list)

        for line in temp_file:
            if line.startswith("#") or line.startswith("best") or len(line) == 0:
                continue
            key_value_pair = line.split(":")
            key = key_value_pair[0].strip()
            value = key_value_pair[1].strip()
            hash_list[key].append(value)
        return hash_list

    @staticmethod
    def get_keys(hash_list, lst):
        temp_list = []
        for key in lst:
            if type(key) == int:
                temp_list.append(int(hash_list[key][0]))
            else:
                temp_list.append(hash_list[key][0])
        return tuple(temp_list)

    def get_rows(self):
        return self.rows

    def get_data(self, key):
        return self.experiment_list[key]

    def update_table(self, hash_list, keys):
        if keys not in self.experiment_list:
            self.rows.append(keys)
            self.experiment_list[keys] = hash_list
        else:
            for key in hash_list:
                self.experiment_list[keys][key].append(hash_list[key][0])

    def process_file(self, data_file, keys_list):
        hash_list = data_cruncher.read_in_data(data_file)
        keys = data_cruncher.get_keys(hash_list, keys_list)
        self.update_table(hash_list, keys)

    def process_dir(self, data_dir, keys_list):
        os.chdir(data_dir)
        for data_file in os.listdir('.'):
            if str(data_file).startswith("log"):
                self.process_file(data_file, keys_list)
        self.rows.sort()

    def validate_data(self, same_keys_list, different_keys_list):
        for key in self.experiment_list:
            for i in same_keys_list:
                if not data_cruncher.all_same(self.experiment_list[key][i]):
                    print "ERROR! different " + i
                    sys.exit(1)

            for i in different_keys_list:
                if not data_cruncher.all_different(self.experiment_list[key][i]):
                    print "ERROR! duplicate " + i
                    sys.exit(1)

            if not data_cruncher.same_element_count(self.experiment_list[key]):
                print "ERROR! not all runs have same number of statistics"
                sys.exit(1)
