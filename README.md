# GPT2SP Replication Package
<!-- PROJECT LOGO -->
<br />
<p align="center">
    <img src="logo/gpt2sp_logo.png" width="200" height="200">
  </a>
  <h3 align="center">GPT2SP</h3>
  <p align="center">
    A web-based Agile story point estimator
    <br />
    <a href="https://share.streamlit.io/awsm-research/gpt2sp_webapp/main/app.py"><strong>View Demo »</strong></a>
    <br />
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
          <li><a href="#about-the-model-training-process">About the Model Training Process</a></li>
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
All of the datasets for 16 different projects are available in the <a href="https://github.com/awsm-research/gpt2sp/tree/main/sp_dataset/marked_data"> marked_data folder </a>
Each dataset has the following 5 columns:
1. issuekey: Issue ID
2. title: Issue Title
3. description: Issue Description
4. storypoint: Assigned Story Point of the Issue
5. split_mark: Represent whether the row was used as training, validation, or testing  

issuekey | title | description | storypoint | split_mark
| :---: | :---: | :---: | :---: | :---:
...  | ... | ... | ... | ...


### About the Models

#### Model Naming Convention
All of the models on HuggingFace Model Hub and Google Drive has the same naming convention as described in the following table:

Model ID | Model Specification | Experiment Scenario
| :---: | :---: | :---: 
#0  | BPE GPT2 Tokenizer + Custom Pre-trained GPT-2 (GPT2SP) | Within-Project 
#00  | BPE GPT2 Tokenizer + Custom Pre-trained GPT-2 (GPT2SP) | Within-Repository 
#000  | BPE GPT2 Tokenizer + Custom Pre-trained GPT-2 (GPT2SP) | Cross-Repository 
#2  | Word-levelSP Tokenizer + Custom Pretrained GPT-2 | Within-Project  
#22  | Word-levelSP Tokenizer + Custom Pretrained GPT-2 | Within-Repository 
#222  | Word-levelSP Tokenizer + Custom Pretrained GPT-2 | Cross-Repository  
#6  | WordPieceSP Tokenizer + Custom Pretrained GPT-2 | Within-Project  
#66  | WordPieceSP Tokenizer + Custom Pretrained GPT-2 | Within-Repository 
#666  | WordPieceSP Tokenizer + Custom Pretrained GPT-2 | Cross-Repository  
#7  | SentencePieceSP Tokenizer + Custom Pretrained GPT-2 | Within-Project  
#77  | SentencePieceSP Tokenizer + Custom Pretrained GPT-2 | Within-Repository 
#777  | SentencePieceSP Tokenizer + Custom Pretrained GPT-2 | Cross-Repository  

#### How to access the models
* Three different pre-trained tokenizers can be found in the all_tokenizers folder: <a href="https://github.com/awsm-research/gpt2sp/tree/main/all_tokenizers/word_level">Word-levelSP Tokenizer<a/>, <a href="https://github.com/awsm-research/gpt2sp/tree/main/all_tokenizers/word_piece"> WordPieceSP Tokenizer <a/>, and <a href="https://github.com/awsm-research/gpt2sp/tree/main/all_tokenizers/sentence_piece"> SentencePieceSP Tokenizer<a/>
* All of the models included in our experiments can be found on the <a href="https://huggingface.co/MickyMike"> Model Hub </a> provided by HuggingFace
* For your information, the models can also be downloaded from this <a href="https://drive.google.com/drive/folders/1oQgj6wDxNa8oF9RHXQsQQr4g_aAbxdif?usp=sharing"> Google Drive <a/>

### About the Model Training Process
* All of the training scripts for different pre-trained tokenizers included in the experiments (RQ3) can be found in <a href="https://github.com/awsm-research/gpt2sp/blob/main/tokenizer_training_notebook.ipynb">tokenizer_training_notebook.ipynb <a/> 
* The model training scripts can be found in <a href="https://github.com/awsm-research/gpt2sp/blob/main/model_training_notebook.ipynb"> model_training_notebook.ipynb <a/> that contains all of the model training process for our experiments (RQ1 + RQ2 + RQ3)
  
### About the Evaluation Results
* All of the raw predictions from different experiments can be found in the <a href="https://github.com/awsm-research/gpt2sp/tree/main/sp_dataset/testing_results">testing_results folder</a> where we classify the folders based on the experimental scenarios (i.e., within-project and cross-project)and each experimental scenario has four different models as follows:
  * GPT2SP
  * SentencePieceSP+GPT2
  * Word-levelSP+GPT2
  * WordPieceSP+GPT2
  
### About the GPT2SP Web App
* Access the GPT2SP web app <a href="https://share.streamlit.io/awsm-research/gpt2sp_webapp/main/gpt2sp_webapp/app.py"> here </a> to interact with our GPT2SP model and navigate the datasets

## Acknowledgements
* Special thanks to DeepSE's developers for providing the datasets and the replication package.
* Special thanks to developers from PyTorch, HuggingFace, Streamlit, Transformers Interpret for providing amazing frameworks for the community
  
## License 
<a href="https://github.com/awsm-research/gpt2sp/blob/main/LICENSE">MIT License</a>
