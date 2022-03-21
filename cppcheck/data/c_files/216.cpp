native_handle* Parcel::readNativeHandle() const
{
int numFds, numInts;
status_t err;
err = readInt32(&numFds);
if (err != NO_ERROR) return 0;
err = readInt32(&numInts);
if (err != NO_ERROR) return 0;

native_handle* h = native_handle_create(numFds, numInts);
if (!h) {
return 0;
}


for (int i=0 ; err==NO_ERROR && i<numFds ; i++) {
h->data[i] = dup(readFileDescriptor());
        if (h->data[i] < 0) err = BAD_VALUE;
}
err = read(h->data + numFds, sizeof(int)*numInts);
if (err != NO_ERROR) {
native_handle_close(h);
native_handle_delete(h);
h = 0;
}
return h;
}
