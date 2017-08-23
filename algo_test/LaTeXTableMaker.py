#! /usr/bin/python

import subprocess

class latex_tablemaker(object):
    """docstring for latex_self.tablemaker."""

    def __init__(self, experiment_name):
        self.file_name = experiment_name+".tex"
        doc_type = "journal"
        self.default_packages = ["longtable", "amsmath", "fullpage", "pdflscape", "booktabs"]
        self.table = open(self.file_name, 'w')
        self.table.write("\\documentclass{{{}}}\n".format(doc_type))

    def add_packages(self, lst=[]):
        lst += self.default_packages
        packages = "\\usepackage[table]{xcolor}\n"
        string = "\\usepackage{{{}}}\n"
        for item in lst:
            packages += string.format(item)
        self.table.write(packages)

    def set_title(self, title):
        self.table.write("\\title{{{}}}\n".format(title))

    def set_author(self, author):
        self.table.write("\\author{{{}}}\n".format(author))

    def align(self, key):
        right = "r"
        left = "l"
        if latex_tablemaker.is_float(key):
            return right
        else:
            return left

    def begin(self, columns_list, heads, caption):
        orientation = "landscape"

        string = "\\begin{document}\n"
        string += "\\rowcolors{2}{gray!25}{white}\n"
        string += "\\begin{{{}}}\n".format(orientation)
        string += "\\maketitle\n"
        string += "%\\begin{center}\n"
        string += "\\begin{longtable}[!t]"
        key_alignment = ""
        for key in columns_list:
            key_alignment += self.align(key) + "|"
        key_alignment[:-1]
        string += "{{{}}}\n".format(key_alignment)
        self.table.write(string)

    def column_names(self, names, heads, columns_list):
        temp_str = "\\multicolumn{{{}}}{{{}}}{{{}}} &\n"
        end_str = "\\\\ \\hline\n"
        string = ""
        for i in range(len(heads)):
            string += temp_str.format(heads[i][1], "c", heads[i][0])

        string = string[:-2]
        string += end_str

        for i in range(len(names)):
            if i == 0:
                string += "{{{}}} &\n".format(names[0])
            else:
                string += temp_str.format(1, self.align(columns_list[i]),names[i])

        string = string[:-2]
        string += end_str
        self.table.write(string)

    def print_row(self,reduced_array):
        #TODO validate number of columns
        self.table.write(" & ".join(reduced_array) + " \\\\\n")

    def footer(self):
        orientation = "landscape"
        string = "%\end{center}\n"
        string += "\\end{longtable}\n"
        string += "\\end{{{}}}\n".format(orientation)
        string += "\\end{document}\n"
        self.table.write(string)
        self.table.close()
        self.pdf()

    def pdf(self):
        subprocess.Popen(["pdflatex", self.file_name])
        #subprocess.Popen(["open", self.file_name])

    @staticmethod
    def is_float(string):
        try:
            float(string)
            return True
        except ValueError:
            return False
