#! /usr/bin/python

import sys
from collections import defaultdict
import os
from LaTeXTableMaker import latex_tablemaker
from MarkdownTableMaker import markdown_tablemaker
from LaTeXPublication import latex_publication

class data_cruncher(object):
    """docstring for data_cruncher.""" #TODO:write a docstring

    def __init__(self, keys_list):
        self.experiment_list = defaultdict(lambda : defaultdict(list))
        self.rows = []
        self.keys_list = keys_list

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
    def is_float(string):
        try:
            float(string)
            return True
        except ValueError:
            return False

    @staticmethod
    def get_avg(hash_list, value):
        #strip is for time calculations
        return sum([float(x.strip("s")) for x in hash_list[value]])/len(hash_list[value])


    @staticmethod
    def get_max(hash_list, value):
        return max([float(x) for x in hash_list[value]])

    @staticmethod
    def format_data(string, string_format):
        if type(string) == "int":
            string_format = "none"
        elif data_cruncher.is_float(string):
            string = float(string)
        if string_format == "none" and data_cruncher.is_float(string):
            string_format = "{0:.2f}"
        if string_format != "none":
            string = string_format.format(string)
        return string


    @staticmethod
    def data_operation(hash_list, keyword, operation):
        if operation == "average":
            return data_cruncher.get_avg(hash_list, keyword)
        elif operation == "maximum":
            return data_cruncher.get_max(hash_list, keyword)
        else:
            return hash_list[keyword][0]

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
    def one_row(hash_list, columns_list):
        temp_array = []
        for col in columns_list:
            data = col[0].strip()
            #TODO handle multiple operations per key
            data_type = col[1]
            if len(col) > 2:
                operation = col[2].strip()
            else:
                operation = "none"
            if len(col) > 3:
                string_format = col[3].strip()
            else:
                string_format = "none"

            data = data_cruncher.data_operation(hash_list, data, operation)
            data = data_cruncher.format_data(data, string_format)
            data = data_cruncher.convert_type(data, data_type)
            temp_array.append(data)

        return temp_array

    @staticmethod
    def get_keys(hash_list, lst):
        temp_list = []
        for key in lst:
            if type(key) == int:
                temp_list.append(int(hash_list[key][0]))
            else:
                temp_list.append(hash_list[key][0])
        return tuple(temp_list)

    @staticmethod
    def str_keys(lst):
        temp_list = []
        for item in lst:
            temp_list.append(str(item))
        return temp_list

    @staticmethod
    def convert_type(data, data_type):
        if data_type == "int" and data_cruncher.is_float(data):
            data = str(int(float(data)))
        return data

    def output_format(self, table_format, experiment_name):
        if table_format == "latex":
            output = latex_tablemaker(experiment_name)
        elif table_format == "markdown":
            output = markdown_tablemaker(experiment_name)
        elif table_format == "latex_publication":
            output = latex_publication(experiment_name)
        return output

    def update_table(self, hash_list, keys):
        if keys not in self.experiment_list:
            self.rows.append(keys)
            self.experiment_list[keys] = hash_list
        else:
            for key in hash_list:
                self.experiment_list[keys][key].append(hash_list[key][0])

    def compare_columns(self, value1, value2):
        for key in self.experiment_list:
            value1 = float(key[value1][0])
            value2 = float(key[value2][0])
            if value1 >= value2:
                value1 = "\\textbf{" + value1 + "}"
            if value2 >= value1:
                value2 = "\\textbf{" + value2 + "}"

    def process_file(self, data_file):
        hash_list = data_cruncher.read_in_data(data_file)
        keys = data_cruncher.get_keys(hash_list, self.keys_list)
        self.update_table(hash_list, keys)

    def process_dir(self, data_dir):
        os.chdir(data_dir)
        for data_file in os.listdir('.'):
            if str(data_file).startswith("log"):
                self.process_file(data_file)
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

    def write_table(self, experiment_name, table_format, title, author, column_names, column_heads, columns_list, caption):
        output = self.output_format(table_format, experiment_name)
        example = list(self.rows[0])
        example += data_cruncher.one_row(self.experiment_list[self.rows[0]], columns_list) #template for aligning the output
        #packages = ["amssymb,amsfonts,amsmath","fullpage", "booktabs", "numprint"]
        output.add_packages()
        output.set_title(title)
        output.set_author(author)
        output.begin(example, column_heads, caption,)
        output.column_names(column_names, column_heads, example)

        for row in self.rows:
            reduced_array = data_cruncher.str_keys(data_cruncher.get_keys(self.experiment_list[row], self.keys_list))
            reduced_array += data_cruncher.one_row(self.experiment_list[row], columns_list)
            output.print_row(reduced_array)

        output.footer()
