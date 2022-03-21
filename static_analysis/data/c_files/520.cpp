void TypingCommand::insertText(Document& document,
const String& text,
Options options,
TextCompositionType composition,
const bool isIncrementalInsertion) {
LocalFrame* frame = document.frame();
DCHECK(frame);

if (!text.isEmpty())
document.frame()->spellChecker().updateMarkersForWordsAffectedByEditing(
isSpaceOrNewline(text[0]));

  insertText(document, text,
             frame->selection().computeVisibleSelectionInDOMTreeDeprecated(),
             options, composition, isIncrementalInsertion);
}
