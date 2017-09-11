#! /usr/bin/python

import sys
import os
sys.path.append("../../")
from Tablegen import *

same_keys_list = ["git-commit", "graph-name", "target", "max-selections", "timeout"]
different_keys_list = ["random-seed"]
keys_list = ["file-seed", "file-k", "file-am"]
#[data, type, operation, formating]
columns_list = [["target", "float", "none", "{:.3e}"], ["greedy-weight", "int"], ["mwis","float", "average"], ["mwis", "float", "maximum"], ["selections", "float", "average"], ["timeout", "float"], ["time(s)", "float", "average"], ["git-commit", "string", "none", "{:.8}"], ["git-status", "string"]]
title = "Results of Labeling MWIS Experiments"
author = "Darren Strash, Daniel Gathogo (automated)"
column_names = ["seed", "k", "am", "wILP", "wGreedy", "wavg", "wmax", "savg", "timeout", "tavg", "last commit", "status"]
column_heads = [("Graph", 3), ("Weight",2), ("MWIS", 3), ("Time", 2), ("Git", 2)]

compare_cols = [5,6]
experiment_name = "Labeling"
experiments = ["labeling"]
sub_headers = [] #subheadings for each experiment
table_format = "latex_publication" #latex, latex_publication, markdown
caption = ""

data_dir = os.getcwd()
table = table_writer()
table.initialize(experiment_name, table_format, title, author) #packages-optional last argument
for i in range(len(experiments)):
    temp_dir = data_dir + "/" + experiments[i]
    exp = data_cruncher()
    exp.process_dir(temp_dir, keys_list)
    exp.validate_data(same_keys_list, different_keys_list)
    table.add_experiment(exp, experiments[i]) #sub_headers[i] optional
table.write_table(column_names, column_heads, columns_list, compare_cols, caption)
