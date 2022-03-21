size_t compile_tree(struct filter_op **fop)
{
int i = 1;
struct filter_op *array = NULL;
struct unfold_elm *ue;

   BUG_IF(tree_root == NULL);

fprintf(stdout, " Unfolding the meta-tree ");
fflush(stdout);

/* start the recursion on the tree */
unfold_blk(&tree_root);

fprintf(stdout, " done.\n\n");

/* substitute the virtual labels with real offsets */
labels_to_offsets();

/* convert the tailq into an array */
TAILQ_FOREACH(ue, &unfolded_tree, next) {

/* label == 0 means a real instruction */
if (ue->label == 0) {
SAFE_REALLOC(array, i * sizeof(struct filter_op));
memcpy(&array[i - 1], &ue->fop, sizeof(struct filter_op));
i++;
}
}

/* always append the exit function to a script */
SAFE_REALLOC(array, i * sizeof(struct filter_op));
array[i - 1].opcode = FOP_EXIT;

/* return the pointer to the array */
*fop = array;

return (i);
}
