import pandas as pd 

df = pd.read_csv("./data/static_analysis_results.csv")

static_results = df["static_analysis_result"].tolist()
line_level_truth = df["flaw_lines"].tolist()
source_func = df["source"].tolist()

correct_count = 0
top_10_count = 0
ifa_records = []

recall_20_goal = 179
effort_20_recall = []
catched_flaw_lines = 0

recall_1_loc = int(12430 * 0.01)
record_recall_1_loc = False
total_catched_recall_1_loc = 0

total_loc = 0
total_flaws = 0

achieve_effort_goal = False

for i in range(len(df)):
    get_error = False
    top_10 = False

    if catched_flaw_lines >= recall_20_goal:
        achieve_effort_goal = True
    
    if not record_recall_1_loc and sum(effort_20_recall) >= recall_1_loc:
        record_recall_1_loc = True
        total_catched_recall_1_loc = catched_flaw_lines        
        
    stc_result = static_results[i].split("\n")
    stc_result = [x for x in stc_result if x != "" and "error:" in x]
    if stc_result == []:
        continue
    flaw_lines_truth = line_level_truth[i]
    source = source_func[i]
    
    # calculate default IFA that is used when static tool can't catch any flaw lines
    ifa = None

    effort = None
    
    total_flaw_lines = len(flaw_lines_truth.split("/~/"))
    total_flaws += total_flaw_lines

    last_flaw_line = flaw_lines_truth.split("/~/")[-1].strip("\n").strip()

    first_flaw_line = flaw_lines_truth.split("/~/")[0].strip("\n").strip()
    lines_of_source_func = source.split("\n")
    lines_of_source_func = [line.strip() for line in lines_of_source_func if line != ""]
    total_loc += len(lines_of_source_func)
    for i in range(len(lines_of_source_func)):
        # IFA
        if first_flaw_line == lines_of_source_func[i]:
            ifa = i
        # effort@20 recall
        if not achieve_effort_goal:
            if last_flaw_line == lines_of_source_func[i]:
                effort = i
        if first_flaw_line == lines_of_source_func[i] and i < 10:
            top_10_count += 1
            top_10 = True

    assert ifa is not None

    for error in stc_result:
        if get_error is not True:
            error = error.split("error:")[1] 
            errs = error.split(" ")
            errs = [x for x in errs if x != "syntax" and x != "error" and x != ""]
            errs = [x.replace("[syntaxError]", "") for x in errs]
            for err in errs:
                if err in flaw_lines_truth:
                    correct_count += 1 
                    get_error = True
                    ifa = 0
                    effort = 1
                    if top_10 is False:
                        top_10_count += 1
                        top_10 = True 
                    break
    ifa_records.append(ifa)

    if not achieve_effort_goal:
        effort_20_recall.append(effort)
        if effort == 1:
            catched_flaw_lines += 1
        else:
            catched_flaw_lines += total_flaw_lines

ifa0 = len([x for x in ifa_records if x == 0])
assert ifa0 == correct_count

print(f"Top-1 Accuracy: {correct_count/len(df)} ({correct_count} / {len(df)})")
print(f"Top-10 Accuracy: {top_10_count/len(df)} ({top_10_count} / {len(df)})")
print(f"IFA: {sum(ifa_records) / len(ifa_records)}")
print(f"Effort@20Recall: {sum(effort_20_recall) / total_loc}")
print(f"Recall@1LOC: {total_catched_recall_1_loc / total_flaws}")

# for boxplot only 
#model_type = ["CppCheck" for _ in range(len(ifa_records))]
#pd.DataFrame({"model_type": model_type, "ifa": ifa_records}).to_csv("./ifa_boxplot.csv", index=False)
