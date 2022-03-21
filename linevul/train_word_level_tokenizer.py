from tokenizers import Tokenizer
from tokenizers.models import WordLevel
from tokenizers.pre_tokenizers import Whitespace
from tokenizers.trainers import WordLevelTrainer
import pandas as pd

"""
# prepare training data
df = pd.read_csv("../data/big-vul_dataset/train.csv")
all_funcs = df["processed_func"].tolist()
for func in all_funcs:
    func = func.split("\n")
    func = [line for line in func if len(line) > 0]
    for line in func:
        with open("../data/tokenizer_train_data.txt", "a") as f:
            f.write(line + "\n")
print("Done")
"""

tokenizer = Tokenizer(WordLevel(unk_token="[UNK]"))
tokenizer.pre_tokenizer = Whitespace()

trainer = WordLevelTrainer(vocab_size=50257,
                           special_tokens=["[UNK]", "[CLS]", "[SEP]", "[PAD]", "[MASK]"],
                           min_frequency=2)

tokenizer.train(files=['../data/tokenizer_train_data.txt'], trainer=trainer)

# Save the files
tokenizer.save("./word_level_tokenizer/wordlevel.json")

print('done')

tokenizer = Tokenizer.from_file('./word_level_tokenizer/wordlevel.json')

encoded = tokenizer.encode("hello how are you hello how are you hello how are you hello how are you hello how are you hello how are you hello how are you hello how are you",)
print(encoded.ids)

