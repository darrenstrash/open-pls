#! /usr/bin/python

import sys
sys.path.append("../../Tablegen")
from DataCruncher import data_cruncher
from TableWriter import table_writer

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
table_format = "markdown" #latex, latex_publication, markdown
caption = ""

data_dir = sys.argv[1]
experiment = data_cruncher()
experiment.process_dir(data_dir, keys_list)
experiment.validate_data(same_keys_list, different_keys_list)
table = table_writer(experiment)
table.initialize(experiment_name, table_format, title, author) #packages-optional last argument
table.write_table(column_names, column_heads, columns_list, caption)
