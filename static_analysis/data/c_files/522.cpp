void CheckNodeValue(const BaseNode& node, const DictionaryValue& value,
bool is_detailed) {
ExpectInt64Value(node.GetId(), value, "id");
{
bool is_folder = false;
EXPECT_TRUE(value.GetBoolean("isFolder", &is_folder));
EXPECT_EQ(node.GetIsFolder(), is_folder);
}
ExpectDictStringValue(node.GetTitle(), value, "title");
{
ModelType expected_model_type = node.GetModelType();
std::string type_str;
EXPECT_TRUE(value.GetString("type", &type_str));
if (expected_model_type >= syncable::FIRST_REAL_MODEL_TYPE) {
ModelType model_type =
syncable::ModelTypeFromString(type_str);
EXPECT_EQ(expected_model_type, model_type);
} else if (expected_model_type == syncable::TOP_LEVEL_FOLDER) {
EXPECT_EQ("Top-level folder", type_str);
} else if (expected_model_type == syncable::UNSPECIFIED) {
EXPECT_EQ("Unspecified", type_str);
} else {
ADD_FAILURE();
}
}
if (is_detailed) {
ExpectInt64Value(node.GetParentId(), value, "parentId");
    ExpectTimeValue(node.GetModificationTime(), value, "modificationTime");
ExpectInt64Value(node.GetExternalId(), value, "externalId");
ExpectInt64Value(node.GetPredecessorId(), value, "predecessorId");
ExpectInt64Value(node.GetSuccessorId(), value, "successorId");
ExpectInt64Value(node.GetFirstChildId(), value, "firstChildId");
{
scoped_ptr<DictionaryValue> expected_entry(node.GetEntry()->ToValue());
Value* entry = NULL;
EXPECT_TRUE(value.Get("entry", &entry));
EXPECT_TRUE(Value::Equals(entry, expected_entry.get()));
}
EXPECT_EQ(11u, value.size());
} else {
EXPECT_EQ(4u, value.size());
}
}
