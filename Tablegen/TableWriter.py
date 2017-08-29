#! /usr/bin/python

import sys
from collections import defaultdict
from LaTeXTableMaker import latex_tablemaker
from MarkdownTableMaker import markdown_tablemaker
from LaTeXPublication import latex_publication
from DataCruncher import data_cruncher

class table_writer(object):
    """docstring for table_writer."""

    def __init__(self):
        self.experiment_list = defaultdict(data_cruncher)
        self.output = None
        self.names = [] #identify different subtables in an experiment
        self.sub_headers = [] #displayed names for the subtables

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

            data = table_writer.data_operation(hash_list, data, operation)
            data = table_writer.format_data(data, string_format)
            data = table_writer.convert_type(data, data_type)
            temp_array.append(data)

        return temp_array

    @staticmethod
    def data_operation(hash_list, keyword, operation):
        if operation == "average":
            return table_writer.get_avg(hash_list, keyword)
        elif operation == "maximum":
            return table_writer.get_max(hash_list, keyword)
        else:
            return hash_list[keyword][0]

    @staticmethod
    def format_data(string, string_format):
        if type(string) == "int":
            string_format = "none"
        elif table_writer.is_float(string):
            string = float(string)
        if string_format == "none" and table_writer.is_float(string):
            string_format = "{0:.2f}"
        if string_format != "none":
            string = string_format.format(string)
        return string

    @staticmethod
    def convert_type(data, data_type):
        if data_type == "int" and table_writer.is_float(data):
            data = str(int(float(data)))
        return data

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
    def output_format(table_format, experiment_name):
        if table_format == "latex":
            output = latex_tablemaker(experiment_name)
        elif table_format == "markdown":
            output = markdown_tablemaker(experiment_name)
        elif table_format == "latex_publication":
            output = latex_publication(experiment_name)
        return output

    def compare_columns(self, value1, value2):
        for key in self.table:
            value1 = float(key[value1][0])
            value2 = float(key[value2][0])
            if value1 >= value2:
                value1 = "\\textbf{" + value1 + "}"
            if value2 >= value1:
                value2 = "\\textbf{" + value2 + "}"


    def add_experiment(self, experiment, experiment_name, sub_header=""):
        self.experiment_list[experiment_name] = experiment
        self.names.append(experiment_name)
        self.sub_headers.append(sub_header)

    def initialize(self, experiment_name, table_format, title, author, packages=[]):
        self.output = table_writer.output_format(table_format, experiment_name)
        self.output.add_packages(packages)
        self.output.set_title(title)
        self.output.set_author(author)

    def write_sub_table(self, name, table, columns_list):
        rows = table.get_rows()
        if(len(self.experiment_list) > 1):
            self.output.print_name(len(table.get_data(rows[0])), name)
        for row in rows:
            reduced_array = list(row)
            reduced_array += table_writer.one_row(table.get_data(row), columns_list)
            self.output.print_row(reduced_array)

    def write_table(self, column_names, column_heads, columns_list, caption):
        rows = self.experiment_list[self.names[0]].get_rows()
        example = list(rows[0])
        example += table_writer.one_row(self.experiment_list[self.names[0]].get_data(rows[0]), columns_list) #template for aligning the output
        self.output.begin(example, column_heads, caption)
        self.output.column_names(column_names, column_heads, example)

        for i in range(len(self.names)):
            self.write_sub_table(self.sub_headers[i], self.experiment_list[self.names[i]], columns_list)

        self.output.footer()
