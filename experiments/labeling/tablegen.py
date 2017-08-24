#! /usr/bin/python

import sys
sys.path.append("../../algo_test")
from DataCruncher import data_cruncher

same_keys_list = ["git-commit", "graph-name", "target", "max-selections", "timeout"]
different_keys_list = ["random-seed"]
keys_list = ["file-seed", "file-k", "file-am"]
#[data, type, operation, formating]
columns_list = [["target", "float", "none", "{:.3e}"], ["greedy-weight", "int"], ["mwis","float", "average"], ["mwis", "float", "maximum"], ["selections", "float", "average"], ["timeout", "float"], ["time(s)", "float", "average"], ["git-commit", "string", "none", "{:.8}"], ["git-status", "string"]]
title = "Results of Labeling MWIS Experiments"
author = "Darren Strash, Daniel Gathogo (automated)"
column_names = ["seed", "k", "am", "wILP", "wGreedy", "wavg", "wmax", "savg", "timeout", "tavg", "last commit", "status"]
column_heads = [("Graph", 3), ("Weight",2), ("MWIS", 3), ("Time", 2), ("Git", 2)]
#column_heads = [] #empty for non publication table
experiment_name = "labeling"
table_format = "latex_publication" #latex, latex_publication, markdown
caption = ""

data_dir = sys.argv[1]
table = data_cruncher(keys_list)
table.process_dir(data_dir)
table.validate_data(same_keys_list, different_keys_list)
table.write_table(experiment_name, table_format, title, author, column_names, column_heads, columns_list, caption)
