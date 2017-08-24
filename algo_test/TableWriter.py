#! /usr/bin/python

import sys
from collections import defaultdict
from DataCruncher import data_cruncher
from LaTeXTableMaker import latex_tablemaker
from MarkdownTableMaker import markdown_tablemaker
from LaTeXPublication import latex_publication

class table_writer(object):
    """docstring for table_writer."""

    def __init__(self, table):
        super(table_writer, self).__init__()
        self.table = table


    def output_format(self, table_format, experiment_name):
        if table_format == "latex":
            output = latex_tablemaker(experiment_name)
        elif table_format == "markdown":
            output = markdown_tablemaker(experiment_name)
        elif table_format == "latex_publication":
            output = latex_publication(experiment_name)
        return output

    def compare_columns(self, value1, value2):
        for key in self.experiment_list:
            value1 = float(key[value1][0])
            value2 = float(key[value2][0])
            if value1 >= value2:
                value1 = "\\textbf{" + value1 + "}"
            if value2 >= value1:
                value2 = "\\textbf{" + value2 + "}"
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
