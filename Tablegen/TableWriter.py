#! /usr/bin/python

import sys
from collections import defaultdict
from LaTeXTableMaker import latex_tablemaker
from MarkdownTableMaker import markdown_tablemaker
from LaTeXPublication import latex_publication

class table_writer(object):
    """docstring for table_writer."""

    def __init__(self, table):
        self.table = table

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
    def str_keys(lst):
        temp_list = []
        for item in lst:
            temp_list.append(str(item))
        return temp_list

    @staticmethod
    def convert_type(data, data_type):
        if data_type == "int" and table_writer.is_float(data):
            data = str(int(float(data)))
        return data

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
        elif table_writer.is_float(string):
            string = float(string)
        if string_format == "none" and table_writer.is_float(string):
            string_format = "{0:.2f}"
        if string_format != "none":
            string = string_format.format(string)
        return string

    def output_format(self, table_format, experiment_name):
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

    def write_table(self, experiment_name, table_format, title, author, column_names, column_heads, columns_list, caption):
        output = self.output_format(table_format, experiment_name)
        rows = self.table.get_rows()
        example = list(rows[0])
        example += table_writer.one_row(self.table.get_data(rows[0]), columns_list) #template for aligning the output
        #packages = ["amssymb,amsfonts,amsmath","fullpage", "booktabs", "numprint"]
        output.add_packages()
        output.set_title(title)
        output.set_author(author)
        output.begin(example, column_heads, caption,)
        output.column_names(column_names, column_heads, example)

        for row in rows:
            reduced_array = list(row)
            reduced_array += table_writer.one_row(self.table.get_data(row), columns_list)
            output.print_row(reduced_array)

        output.footer()
