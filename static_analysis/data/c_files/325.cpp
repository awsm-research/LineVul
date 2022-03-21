bool GDataDirectory::FromProto(const GDataDirectoryProto& proto) {
DCHECK(proto.gdata_entry().file_info().is_directory());
DCHECK(!proto.gdata_entry().has_file_specific_info());

for (int i = 0; i < proto.child_files_size(); ++i) {
    scoped_ptr<GDataFile> file(new GDataFile(NULL, directory_service_));
if (!file->FromProto(proto.child_files(i))) {
RemoveChildren();
return false;
}
AddEntry(file.release());
}
for (int i = 0; i < proto.child_directories_size(); ++i) {
    scoped_ptr<GDataDirectory> dir(new GDataDirectory(NULL,
                                                      directory_service_));
if (!dir->FromProto(proto.child_directories(i))) {
RemoveChildren();
return false;
}
AddEntry(dir.release());
}

// The states of the directory should be updated after children are
// handled successfully, so that incomplete states are not left.
if (!GDataEntry::FromProto(proto.gdata_entry()))
return false;

return true;
}
