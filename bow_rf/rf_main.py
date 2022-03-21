import pandas as pd
from sklearn.feature_extraction.text import TfidfVectorizer
from sklearn.ensemble import RandomForestClassifier
from sklearn.metrics import f1_score, precision_score, recall_score
import pickle
import numpy as np


# load train, val data
train = pd.read_csv('../data/big-vul_dataset/train.csv')
val = pd.read_csv('../data/big-vul_dataset/val.csv')
# use train + val data to fit the model
train_data = pd.concat([train, val])
# load test data
test_data = pd.read_csv('../data/big-vul_dataset/test.csv')
# textual code data
X_train = train_data["processed_func"]
X_test = test_data["processed_func"]
# labels
y_train = train_data["target"]
y_test = test_data["target"]
# apply BoW feature extraction
vectorizer = TfidfVectorizer(norm='l2', max_features=1000)
vectorizer = vectorizer.fit(X_train)
X_train = vectorizer.transform(X_train).todense()
X_test = vectorizer.transform(X_test).todense()
# train the model
rf = RandomForestClassifier(n_estimators=1000,
                            n_jobs=-1,
                            verbose=1)
rf.fit(X_train, y_train)
preds = rf.predict(X_test)
f1 = f1_score(y_true=y_test, y_pred=preds)
precision = precision_score(y_true=y_test, y_pred=preds)
recall = recall_score(y_true=y_test, y_pred=preds)
print(f"F1 Score: {f1}")
print(f"Precision: {precision}")
print(f"Recall: {recall}")

with open('./saved_models/best_f1_rf.pkl', 'wb') as f:
    pickle.dump(rf, f)

print("done")
