int SoundPool::load(int fd, int64_t offset, int64_t length, int priority __unused)
{
ALOGV("load: fd=%d, offset=%" PRId64 ", length=%" PRId64 ", priority=%d",
fd, offset, length, priority);
    Mutex::Autolock lock(&mLock);
    sp<Sample> sample = new Sample(++mNextSampleID, fd, offset, length);
    mSamples.add(sample->sampleID(), sample);
    doLoad(sample);
    return sample->sampleID();
}
