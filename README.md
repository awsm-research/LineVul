# LineVul Replication Package
<!-- PROJECT LOGO -->
<br />
<p align="center">
    <img src="logo/todo" width="200" height="200">
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
All of the dataset has the same number of columns (i.e., 39 cols), we focus on the following 3 columns to conduct our experiments:
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

Model Name | Model Specification | HuggingFace Model ID
| :---: | :---: | :---: 
LineVul  | BPE Tokenizer + Pre-training (Codesearchnet) + BERT | MickyMike/LineVul
BPEBERT  | BPE Tokenizer + No Pre-training + BERT | MickyMike/BPEBERT
WordlevelPretrainedBERT  | Wordlevel Tokenizer + Pre-training (Codesearchnet) + BERT | MickyMike/WordlevelPretrainedBERT
WordlevelBERT | Wordlevel Tokenizer + No Pre-training + BERT | MickyMike/WordlevelBERT
BoWRF | Bag of Words + Random Forest | N/A

#### How to access the models
* All of the models included in our experiments can be found on the <a href="https://huggingface.co/MickyMike"> Model Hub </a> provided by HuggingFace
* For your information, the models can also be downloaded from this <a href="todo"> Google Drive <a/>

### About the Experiment Replication
  We provide <a href="todo">a csv file</a> that contains all of the raw function-level predictions by LineVul.

#### How to replicate RQ1
  To reproduce the RQ1 result, run the following commands (Inference only):
  
  To retrain the RQ1 model, run the following commands (Training + Inference):

#### How to replicate RQ2
  To reproduce the RQ2 result of Top-10 Accuracy, run the following commands:
    
  To reproduce the RQ2 result of IFA Accuracy, run the following commands:
    
#### How to replicate RQ3
  To reproduce the RQ3 result of Effort@20%Recall, run the following commands:
    
  To reproduce the RQ3 result of Recall@1%LOC, run the following commands:

#### How to replicate the ablation study in the discussion section
  To reproduce the result of XX model in the ablation study, run the following commands:
    
  To reproduce the result of XX model in the ablation study, run the following commands:
    
  To reproduce the result of XX model in the ablation study, run the following commands:
    
  To reproduce the result of XX model in the ablation study, run the following commands:
    
## Acknowledgements
* Special thanks to CodeBERT's developers
* Special thanks to BigVulDataset Provider
* Special thanks to developers from PyTorch and HuggingFace for providing amazing frameworks to the community
  
## License 
<a href="https://github.com/awsm-research/LineVul/blob/main/LICENSE">MIT License</a>
