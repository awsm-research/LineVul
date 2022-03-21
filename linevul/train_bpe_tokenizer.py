# Initialize a tokenizer
from tokenizers import ByteLevelBPETokenizer
"""
tokenizer = ByteLevelBPETokenizer()
# Customize training
tokenizer.train(files="../data/tokenizer_train_data.txt", 
                vocab_size=50257,
                min_frequency=2, 
                special_tokens=["<s>",
                                "<pad>",
                                "</s>",
                                "<unk>",
                                "<mask>",
                                ])

# Save files to disk
tokenizer.save_model("./bpe_tokenizer/", "bpe_tokenizer")
tokenizer.save('./bpe_tokenizer/config.json')
"""

from transformers import RobertaTokenizer
tokenizer = RobertaTokenizer(vocab_file="bpe_tokenizer/bpe_tokenizer-vocab.json",
                             merges_file="bpe_tokenizer/bpe_tokenizer-merges.txt")