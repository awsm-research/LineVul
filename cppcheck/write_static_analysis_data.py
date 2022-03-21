import pandas as pd

df = pd.read_csv("./data/static_analysis_data.csv")
source = df["source"].tolist()
index = 0
for one_func in source:
    with open(f"./data/c_files/{index}.cpp", "w+") as f:
        f.write(one_func)
    index += 1
print("done")