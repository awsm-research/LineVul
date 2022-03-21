bool AppCacheDatabase::FindEntriesForCache(int64_t cache_id,
std::vector<EntryRecord>* records) {
DCHECK(records && records->empty());
if (!LazyOpen(kDontCreate))
return false;

static const char kSql[] =
      "SELECT cache_id, url, flags, response_id, response_size FROM Entries"
"  WHERE cache_id = ?";

sql::Statement statement(db_->GetCachedStatement(SQL_FROM_HERE, kSql));
statement.BindInt64(0, cache_id);

while (statement.Step()) {
records->push_back(EntryRecord());
ReadEntryRecord(statement, &records->back());
DCHECK(records->back().cache_id == cache_id);
}

return statement.Succeeded();
}
