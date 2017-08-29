#! /usr/bin/python

class markdown_tablemaker(object):
    """docstring for MarkdownTableMaker."""
    def __init__(self, experiment_name):
        self.file_name = experiment_name+".md"
        self.table = open(self.file_name, "w")
        self.newline = "  \n"

    def add_packages(self, lst=[]):
        pass

    def set_title(self, title):
        string = "## {}".format(title) + self.newline
        self.table.write(string)

    def set_author(self, author):
        string = "### *{}*".format(author) + self.newline
        self.table.write(string)

    def align(self, key):
        right = ":"
        left = ""
        if markdown_tablemaker.is_float(key):
            return right
        else:
            return left

    def begin(self, columns_list, heads, caption):
        self.table.write(caption)

    def column_names(self, names, heads, columns_list):
        string = ""
        hyphen = "---"
        #TODO support for colspan
#        for head in heads:
#            string += head[0] + "|"*head[1]
#        string = string + self.newline
#        for i in range(len(heads)):
#            string += ":" + hyphen + ": | "
#        string = string[:-2] + self.newline

        for name in names:
            string +=  name + " | "
        string = string[:-2] + self.newline
        for i in range(len(names)):
            string += hyphen +self.align(columns_list[i]) + " | "
        string = string[:-2] + self.newline
        self.table.write(string)

    def print_name(self, name):
        self.table.write(name) #TODO support for colspan

    def print_row(self,reduced_array):
        string = ""
        for item in reduced_array:
            string += item + " | "
        string = string [:-2] + self.newline
        self.table.write(string)

    def footer(self):
        self.table.write(self.newline)
        self.table.close()

    @staticmethod
    def is_float(string):
        try:
            float(string)
            return True
        except ValueError:
            return False
