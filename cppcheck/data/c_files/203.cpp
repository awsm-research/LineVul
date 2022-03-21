void AllocateDataSet(cmsIT8* it8)
{
TABLE* t = GetTable(it8);

if (t -> Data) return;    // Already allocated

t-> nSamples   = atoi(cmsIT8GetProperty(it8, "NUMBER_OF_FIELDS"));
t-> nPatches   = atoi(cmsIT8GetProperty(it8, "NUMBER_OF_SETS"));

    t-> Data = (char**)AllocChunk (it8, ((cmsUInt32Number) t->nSamples + 1) * ((cmsUInt32Number) t->nPatches + 1) *sizeof (char*));
    if (t->Data == NULL) {

        SynError(it8, "AllocateDataSet: Unable to allocate data array");
}

}
