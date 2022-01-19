# LineVul Replication Package
<!-- PROJECT LOGO -->
<br />
<p align="center">
    <img src="logo/linevul_logo.png" width="200" height="200">
  </a>
  <h3 align="center">LineVul</h3>
  <p align="center">
    A Transformer-based Line-Level Vulnerability Prediction Approach
  </p>
</p>

<!-- Table of contents -->
<details open="open">
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#how-to-replicate">How to replicate</a>
        <ul>
          <li><a href="#about-the-datasets">About the Datasets</a></li>
          <li><a href="#about-the-models">About the Models</a></li>
          <li><a href="#about-the-experiment-replication">About the Experiment Replication</a></li>
          <li><a href="#about-the-evaluation-results">About the Evaluation Results</a></li>
          <li><a href="#about-the-gpt2sp-web-app">About the GPT2SP Web App</a></li>
        </ul>
    </li>
    <li>
      <a href="#acknowledgements">Acknowledgements</a>
    </li>
    <li>
      <a href="#license">License</a>
    </li>
  </ol>
</details>

## How to replicate 

### About the Datasets
All of the dataset has the same number of columns (i.e., 39 cols), we focus on the following N columns to conduct our experiments </a>
1. processed_func (str): The original function written in C/C++
2. target (int): The function-level label that determines whether a function is vulnerable or not
3. vul_func_with_fix (str): The fixed function with added in deleted lines labeled

processed_func | target | vul_func_with_fix 
| :---: | :---: | :---:
...  | ... | ... 

For more information of our dataset, please refer to <a href="https://dl.acm.org/doi/10.1145/3379597.3387501">this paper</a> and <a href="https://github.com/ZeoVan/MSR_20_Code_vulnerability_CSV_Dataset">this repository</a>.

### About the Models

#### Model Naming Convention
All of the models on HuggingFace Model Hub and Google Drive has the same naming convention as described in the following table:

Model Name | Model Specification | HuggingFace ID
| :---: | :---: | :---: 
LineVul  | BPE + Pre-training (Codesearchnet) + BERT | MickyMike/LineVul
XX  | BPE GPT2 Tokenizer + Custom Pre-trained GPT-2 (GPT2SP) | MickyMike/
XXX  | BPE GPT2 Tokenizer + Custom Pre-trained GPT-2 (GPT2SP) | MickyMike/
 
#### How to access the models
* Three different pre-trained tokenizers can be found in the all_tokenizers folder: <a href="https://github.com/awsm-research/gpt2sp/tree/main/all_tokenizers/word_level">Word-levelSP Tokenizer<a/>, <a href="https://github.com/awsm-research/gpt2sp/tree/main/all_tokenizers/word_piece"> WordPieceSP Tokenizer <a/>, and <a href="https://github.com/awsm-research/gpt2sp/tree/main/all_tokenizers/sentence_piece"> SentencePieceSP Tokenizer<a/>
* All of the models included in our experiments can be found on the <a href="https://huggingface.co/MickyMike"> Model Hub </a> provided by HuggingFace
* For your information, the models can also be downloaded from this <a href="https://drive.google.com/drive/folders/1oQgj6wDxNa8oF9RHXQsQQr4g_aAbxdif?usp=sharing"> Google Drive <a/>

### About the Experiment Replication
    
#### How to replicate RQ1

#### How to replicate RQ2

#### How to replicate RQ3

  
### About the Evaluation Results
* All of the raw predictions from different experiments can be found in the <a href="https://github.com/awsm-research/gpt2sp/tree/main/sp_dataset/testing_results">testing_results folder</a> where we classify the folders based on the experimental scenarios (i.e., within-project and cross-project)and each experimental scenario has four different models as follows:
  * GPT2SP
  * SentencePieceSP+GPT2
  * Word-levelSP+GPT2
  * WordPieceSP+GPT2
  
## Acknowledgements
* Special thanks to CodeBERT's developers
* Special thanks to BigVulDataset Provider
* Special thanks to developers from PyTorch and HuggingFace for providing amazing frameworks to the community
  
## License 
<a href="https://github.com/awsm-research/LineVul/blob/main/LICENSE">MIT License</a>
