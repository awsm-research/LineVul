void HeapAllocator::backingFree(void* address) {
if (!address)
return;

ThreadState* state = ThreadState::current();
if (state->sweepForbidden())
return;
ASSERT(!state->isInGC());

// Don't promptly free large objects because their page is never reused.
// Don't free backings allocated on other threads.
BasePage* page = pageFromObject(address);
if (page->isLargeObjectPage() || page->arena()->getThreadState() != state)
return;

HeapObjectHeader* header = HeapObjectHeader::fromPayload(address);
  ASSERT(header->checkHeader());
NormalPageArena* arena = static_cast<NormalPage*>(page)->arenaForNormalPage();
state->promptlyFreed(header->gcInfoIndex());
arena->promptlyFreeObject(header);
}
