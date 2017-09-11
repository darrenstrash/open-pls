#! /usr/bin/python

import subprocess
from LaTeXTableMaker import latex_tablemaker

class latex_publication(latex_tablemaker):
    """docstring for latex_publication."""
    def __init__(self, experiment_name):
        #super(latex_publication, self).__init__(experiment_name)
        self.file_name = experiment_name+".tex"
        doc_type = "[11pt]{article}\n"
        self.default_packages = ["longtable", "amsmath", "fullpage", "pdflscape", "booktabs"]
        self.table = open(self.file_name, 'w')
        self.table.write("\\documentclass" + doc_type)

    def begin(self, columns_list, heads, caption):
        padding = "18pt"
        string = "\\begin{document}\n"
        string += "\\begin{table}\n"
        string += "\\maketitle\n"
        string += "\\begin{center}\n"
        string += "\\caption{{{}}}\n".format(caption)
        string += "\\scriptsize\n"
        string += "\\setlength{\\tabcolsep}{.9ex}\n"
        string += "\\begin{tabular}[tb]"
        counter = 0
        temp = ""
        for head in heads:
            temp_str = "\\hskip {}".format(padding)
            temp_str = "@{{{}}} ".format(temp_str)
            key_str = ""
            for i in range(head[1]):
                key_str +=latex_tablemaker.align(columns_list[counter+i])
            temp_str = key_str + temp_str
            counter += head[1]
            temp += temp_str
        temp = temp[:temp.rfind("@")]
        string += "{{{}}} \n". format(temp)
        string += "\\toprule\n"
        self.table.write(string)

    def column_names(self, names, heads, columns_list):
        temp_str = "\\multicolumn{{{}}}{{{}}}{{{}}} &"
        end_str = "\\\\ \n"
        string = ""
        center = "c"
        for i in range(len(heads)):
            string += temp_str.format(heads[i][1], center, heads[i][0])

        string = string[:-2]
        string += end_str

        for i in range(len(names)):
            string += temp_str.format(1, center, names[i])

        string = string[:-2]
        string += end_str
        temp = "\\cmidrule({}){{{}-{}}}"
        for i in range(len(columns_list)):
            string += temp.format(latex_tablemaker.align(columns_list[i]), i+1, i+1)
        string += end_str
        self.table.write(string)


    def footer(self):
        string = "\\hline"
        string += "\\end{tabular}\n"
        string += "\\end{center}\n"
        string += "\\end{table}\n"
        string += "\\end{document}\n"
        self.table.write(string)
        self.table.close()
        subprocess.Popen(["pdflatex", self.file_name])
