ZEND_API void ZEND_FASTCALL _zend_hash_init(HashTable *ht, uint32_t nSize, dtor_func_t pDestructor, zend_bool persistent ZEND_FILE_LINE_DC)
{
GC_REFCOUNT(ht) = 1;
GC_TYPE_INFO(ht) = IS_ARRAY;
ht->u.flags = (persistent ? HASH_FLAG_PERSISTENT : 0) | HASH_FLAG_APPLY_PROTECTION | HASH_FLAG_STATIC_KEYS;
	ht->nTableSize = zend_hash_check_size(nSize);
ht->nTableMask = HT_MIN_MASK;
HT_SET_DATA_ADDR(ht, &uninitialized_bucket);
ht->nNumUsed = 0;
ht->nNumOfElements = 0;
ht->nInternalPointer = HT_INVALID_IDX;
ht->nNextFreeElement = 0;
ht->pDestructor = pDestructor;
}
