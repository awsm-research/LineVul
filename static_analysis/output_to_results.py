import pandas as pd 

results = []

with open("./output.txt", "r") as f:
    lines = f.readlines()

index = 0
line_info = ""
for k in range(628):
    line_info = ""
    for i in range(len(lines)):
        if f"data/c_files/{k}.cpp" in lines[i] and "syntaxError" in lines[i]:
            line_info += lines[i].strip("\n")
            line_info += lines[i+1]
        elif f"data/c_files/{k}.cpp" in lines[i]:
            line_info += lines[i]
    results.append(line_info)

df = pd.read_csv("./data/static_analysis_data.csv")

assert len(results) == len(df)

df["static_analysis_result"] = results
df.to_csv("./data/static_analysis_results.csv")