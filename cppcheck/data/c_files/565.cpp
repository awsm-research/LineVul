int validate_camera_metadata_structure(const camera_metadata_t *metadata,
const size_t *expected_size) {

if (metadata == NULL) {
ALOGE("%s: metadata is null!", __FUNCTION__);
return ERROR;
}

// Check that the metadata pointer is well-aligned first.
{
static const struct {
const char *name;
size_t alignment;
} alignments[] = {
{
.name = "camera_metadata",
.alignment = METADATA_ALIGNMENT
},
{
.name = "camera_metadata_buffer_entry",
.alignment = ENTRY_ALIGNMENT
},
{
.name = "camera_metadata_data",
.alignment = DATA_ALIGNMENT
},
};

for (size_t i = 0; i < sizeof(alignments)/sizeof(alignments[0]); ++i) {
uintptr_t aligned_ptr = ALIGN_TO(metadata, alignments[i].alignment);

if ((uintptr_t)metadata != aligned_ptr) {
ALOGE("%s: Metadata pointer is not aligned (actual %p, "
"expected %p) to type %s",
__FUNCTION__, metadata,
(void*)aligned_ptr, alignments[i].name);
return ERROR;
}
}
}

/**
* Check that the metadata contents are correct
*/

if (expected_size != NULL && metadata->size > *expected_size) {
ALOGE("%s: Metadata size (%" PRIu32 ") should be <= expected size (%zu)",
__FUNCTION__, metadata->size, *expected_size);
return ERROR;
}

if (metadata->entry_count > metadata->entry_capacity) {
ALOGE("%s: Entry count (%" PRIu32 ") should be <= entry capacity "
"(%" PRIu32 ")",
__FUNCTION__, metadata->entry_count, metadata->entry_capacity);

return ERROR;
}

    const metadata_uptrdiff_t entries_end =
        metadata->entries_start + metadata->entry_capacity;
if (entries_end < metadata->entries_start || // overflow check
entries_end > metadata->data_start) {

ALOGE("%s: Entry start + capacity (%" PRIu32 ") should be <= data start "
"(%" PRIu32 ")",
__FUNCTION__,
(metadata->entries_start + metadata->entry_capacity),
metadata->data_start);
return ERROR;
}

const metadata_uptrdiff_t data_end =
metadata->data_start + metadata->data_capacity;
if (data_end < metadata->data_start || // overflow check
data_end > metadata->size) {

ALOGE("%s: Data start + capacity (%" PRIu32 ") should be <= total size "
"(%" PRIu32 ")",
__FUNCTION__,
(metadata->data_start + metadata->data_capacity),
metadata->size);
return ERROR;
}

// Validate each entry
const metadata_size_t entry_count = metadata->entry_count;
camera_metadata_buffer_entry_t *entries = get_entries(metadata);

for (size_t i = 0; i < entry_count; ++i) {

if ((uintptr_t)&entries[i] != ALIGN_TO(&entries[i], ENTRY_ALIGNMENT)) {
ALOGE("%s: Entry index %zu had bad alignment (address %p),"
" expected alignment %zu",
__FUNCTION__, i, &entries[i], ENTRY_ALIGNMENT);
return ERROR;
}

camera_metadata_buffer_entry_t entry = entries[i];

if (entry.type >= NUM_TYPES) {
ALOGE("%s: Entry index %zu had a bad type %d",
__FUNCTION__, i, entry.type);
return ERROR;
}

// TODO: fix vendor_tag_ops across processes so we don't need to special
//       case vendor-specific tags
uint32_t tag_section = entry.tag >> 16;
int tag_type = get_camera_metadata_tag_type(entry.tag);
if (tag_type != (int)entry.type && tag_section < VENDOR_SECTION) {
ALOGE("%s: Entry index %zu had tag type %d, but the type was %d",
__FUNCTION__, i, tag_type, entry.type);
return ERROR;
}

size_t data_size;
if (validate_and_calculate_camera_metadata_entry_data_size(&data_size, entry.type,
entry.count) != OK) {
ALOGE("%s: Entry data size is invalid. type: %u count: %u", __FUNCTION__, entry.type,
entry.count);
return ERROR;
}

if (data_size != 0) {
camera_metadata_data_t *data =
(camera_metadata_data_t*) (get_data(metadata) +
entry.data.offset);

if ((uintptr_t)data != ALIGN_TO(data, DATA_ALIGNMENT)) {
ALOGE("%s: Entry index %zu had bad data alignment (address %p),"
" expected align %zu, (tag name %s, data size %zu)",
__FUNCTION__, i, data, DATA_ALIGNMENT,
get_camera_metadata_tag_name(entry.tag) ?: "unknown",
data_size);
return ERROR;
}

size_t data_entry_end = entry.data.offset + data_size;
if (data_entry_end < entry.data.offset || // overflow check
data_entry_end > metadata->data_capacity) {

ALOGE("%s: Entry index %zu data ends (%zu) beyond the capacity "
"%" PRIu32, __FUNCTION__, i, data_entry_end,
metadata->data_capacity);
return ERROR;
}

} else if (entry.count == 0) {
if (entry.data.offset != 0) {
ALOGE("%s: Entry index %zu had 0 items, but offset was non-0 "
"(%" PRIu32 "), tag name: %s", __FUNCTION__, i, entry.data.offset,
get_camera_metadata_tag_name(entry.tag) ?: "unknown");
return ERROR;
}
} // else data stored inline, so we look at value which can be anything.
}

return OK;
}
