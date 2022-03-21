R_API RConfigNode* r_config_set(RConfig *cfg, const char *name, const char *value) {
RConfigNode *node = NULL;
char *ov = NULL;
ut64 oi;
if (!cfg || STRNULL (name)) {
return NULL;
}
node = r_config_node_get (cfg, name);
if (node) {
if (node->flags & CN_RO) {
eprintf ("(error: '%s' config key is read only)\n", name);
return node;
}
oi = node->i_value;
if (node->value) {
ov = strdup (node->value);
if (!ov) {
goto beach;
}
} else {
free (node->value);
node->value = strdup ("");
}
if (node->flags & CN_BOOL) {
bool b = is_true (value);
node->i_value = (ut64) b? 1: 0;
char *value = strdup (r_str_bool (b));
if (value) {
free (node->value);
node->value = value;
}
} else {
if (!value) {
free (node->value);
node->value = strdup ("");
node->i_value = 0;
} else {
if (node->value == value) {
goto beach;
}
				free (node->value);
node->value = strdup (value);
if (IS_DIGIT (*value)) {
if (strchr (value, '/')) {
node->i_value = r_num_get (cfg->num, value);
} else {
node->i_value = r_num_math (cfg->num, value);
}
} else {
node->i_value = 0;
}
node->flags |= CN_INT;
}
}
} else { // Create a new RConfigNode
oi = UT64_MAX;
if (!cfg->lock) {
node = r_config_node_new (name, value);
if (node) {
if (value && is_bool (value)) {
node->flags |= CN_BOOL;
node->i_value = is_true (value)? 1: 0;
}
if (cfg->ht) {
ht_insert (cfg->ht, node->name, node);
r_list_append (cfg->nodes, node);
cfg->n_nodes++;
}
} else {
eprintf ("r_config_set: unable to create a new RConfigNode\n");
}
} else {
eprintf ("r_config_set: variable '%s' not found\n", name);
}
}

if (node && node->setter) {
int ret = node->setter (cfg->user, node);
if (ret == false) {
if (oi != UT64_MAX) {
node->i_value = oi;
}
free (node->value);
node->value = strdup (ov? ov: "");
}
}
beach:
free (ov);
return node;
}
