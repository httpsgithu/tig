static bool prompt_yesno(const char *prompt);
static int load_refs(void);
#define SIZEOF_REV	41	/* Holds a SHA-1 and an ending NUL. */
#define SIZEOF_ARG	32	/* Default argument array size. */
#define GIT_CONFIG "config"
#define TIG_MAIN_BASE \
	"git log --no-color --pretty=raw --parents --topo-order"

	TIG_MAIN_BASE " %s 2>/dev/null"
static struct ref **get_refs(const char *id);
	REQ_GROUP("View specific requests") \
	REQ_(STATUS_UPDATE,	"Update file status"), \
	REQ_(STATUS_REVERT,	"Revert file changes"), \
	REQ_(STATUS_MERGE,	"Merge file using external tool"), \
	REQ_(STAGE_NEXT,	"Find next chunk to stage"), \
	REQ_(TREE_PARENT,	"Switch to parent directory in tree view"), \
	\
	REQ_GROUP("Option manipulation") \
	REQ_(TOGGLE_LINENO,	"Toggle line numbers"), \
	REQ_(TOGGLE_DATE,	"Toggle date display"), \
	REQ_(TOGGLE_AUTHOR,	"Toggle author display"), \
	REQ_(TOGGLE_REV_GRAPH,	"Toggle revision graph visualization"), \
	REQ_(TOGGLE_REFS,	"Toggle reference display (tags/branches)"), \
	\
	const char *name;
	const char *help;
static int opt_author_cols		= AUTHOR_COLS-1;
static enum request
parse_options(int argc, const char *argv[])
	enum request request = REQ_VIEW_MAIN;
	const char *subcommand;
		return REQ_VIEW_PAGER;
		return REQ_VIEW_MAIN;
		return REQ_VIEW_STATUS;
		return REQ_VIEW_BLAME;
		request = REQ_VIEW_DIFF;
		request = subcommand[0] == 'l' ? REQ_VIEW_LOG : REQ_VIEW_DIFF;
		string_copy(opt_cmd, TIG_MAIN_BASE);
		const char *opt = argv[i];
			return REQ_NONE;
			return REQ_NONE;
	return request;
get_line_type(const char *line)
get_line_info(const char *name)
	{ '!',		REQ_STATUS_REVERT },
	const char *name;
static const char *
	const char *seq = NULL;
	return seq ? seq : "(no key)";
static const char *
add_run_request(enum keymap keymap, int key, int argc, const char **argv)
		const char *argv[1];
static const char *config_msg;
option_color_command(int argc, const char *argv[])
static int
parse_int(const char *s, int default_value, int min, int max)
{
	int value = atoi(s);

	return (value < min || value > max) ? default_value : value;
}

option_set_command(int argc, const char *argv[])
		opt_num_interval = parse_int(argv[2], opt_num_interval, 1, 1024);
		return OK;
	}

	if (!strcmp(argv[0], "author-width")) {
		opt_author_cols = parse_int(argv[2], opt_author_cols, 0, 1024);
		opt_tab_size = parse_int(argv[2], opt_tab_size, 1, 1024);
		const char *arg = argv[2];
		int arglen = strlen(arg);
		switch (arg[0]) {
			if (arglen == 1 || arg[arglen - 1] != arg[0]) {
				config_msg = "Unmatched quotation";
				return ERR;
			}
			arg += 1; arglen -= 2;
option_bind_command(int argc, const char *argv[])
set_option(const char *opt, char *value)
	const char *argv[SIZEOF_ARG];
	const char *home = getenv("HOME");
	const char *tigrc_user = getenv("TIGRC_USER");
	const char *tigrc_system = getenv("TIGRC_SYSTEM");
static struct view_ops blob_ops;
static struct view_ops log_ops;
static struct view_ops main_ops;
static struct view_ops pager_ops;
static struct view_ops status_ops;
static struct view_ops tree_ops;
	VIEW_(LOG,    "log",    &log_ops,    TRUE,  ref_head),
draw_field(struct view *view, enum line_type type, const char *text, int len, bool trim)
reset_view(struct view *view)
{
	int i;

	for (i = 0; i < view->lines; i++)
		free(view->line[i].data);
	free(view->line);

	view->line = NULL;
	view->offset = 0;
	view->lines  = 0;
	view->lineno = 0;
	view->line_size = 0;
	view->line_alloc = 0;
	view->vid[0] = 0;
}

static void
end_update(struct view *view, bool force)
	while (!view->ops->read(view, NULL))
		if (!force)
			return;
begin_update(struct view *view, bool refresh)
	} else if (!refresh) {
	reset_view(view);
	if (ferror(view->pipe) && errno != 0) {
		end_update(view, TRUE);
		end_update(view, FALSE);
	end_update(view, TRUE);
add_line_text(struct view *view, const char *text, enum line_type type)
	char *data = text ? strdup(text) : NULL;
	OPEN_NOMAXIMIZE = 8,	/* Do not maximize the current view. */
	OPEN_REFRESH = 16,	/* Refresh view using previous command. */
	bool reload = !!(flags & (OPEN_RELOAD | OPEN_REFRESH));
	bool nomaximize = !!(flags & (OPEN_NOMAXIMIZE | OPEN_REFRESH));
	if (view->pipe)
		end_update(view, TRUE);

		   !begin_update(view, flags & OPEN_REFRESH)) {
	} else if (view_is_displayed(view)) {
static bool
run_confirm(const char *cmd, const char *prompt)
{
	bool confirmation = prompt_yesno(prompt);

	if (confirmation)
		system(cmd);

	return confirmation;
}

	const char *editor;
		    view == VIEW(REQ_VIEW_MAIN) ||
		    view == VIEW(REQ_VIEW_LOG) ||
			end_update(view, TRUE);
			report("");
add_describe_ref(char *buf, size_t *bufpos, const char *commit_id, const char *sep)
		const char *fmt = ref->tag    ? "%s[%s]" :
		                  ref->remote ? "%s<%s>" : "%s%s";
static enum request
log_request(struct view *view, enum request request, struct line *line)
{
	switch (request) {
	case REQ_REFRESH:
		load_refs();
		open_view(view, REQ_VIEW_LOG, OPEN_REFRESH);
		return REQ_NONE;
	default:
		return pager_request(view, request, line);
	}
}

static struct view_ops log_ops = {
	"line",
	NULL,
	pager_read,
	pager_draw,
	log_request,
	pager_grep,
	pager_select,
};

		const char *key;
		const char *key;
push_tree_stack_entry(const char *name, unsigned long lineno)
tree_compare_entry(enum line_type type1, const char *name1,
		   enum line_type type2, const char *name2)
static const char *
	const char *path = line->data;
		const char *path1 = tree_path(line);
		const char *filename = tree_path(line);
			const char *basename = tree_path(line);
#define BLAME_INCREMENTAL_CMD "git blame --incremental %s -- %s"
	reset_view(view);
parse_number(const char **posref, size_t *number, size_t min, size_t max)
	const char *pos = *posref;
parse_blame_commit(struct view *view, const char *text, int *blamed)
	const char *pos = text + SIZEOF_REV - 1;
blame_read_file(struct view *view, const char *line)
	const char *id = NULL, *author = NULL;
	    draw_field(view, LINE_MAIN_AUTHOR, author, opt_author_cols, TRUE))
/* This should work even for the "On branch" line. */
static inline bool
status_has_none(struct view *view, struct line *line)
{
	return line < view->line + view->lines && !line[1].data;
}

status_get_diff(struct status *file, const char *buf, size_t bufsize)
	const char *old_mode = buf +  1;
	const char *new_mode = buf +  8;
	const char *old_rev  = buf + 15;
	const char *new_rev  = buf + 56;
	const char *status   = buf + 97;
	"git ls-files -z --others --exclude-standard"
	"git ls-files -z --cached --exclude-standard"
	reset_view(view);
	system("git update-index -q --refresh >/dev/null 2>/dev/null");
	if (opt_no_head) {
		if (!status_run(view, STATUS_LIST_NO_HEAD_CMD, 'A', LINE_STAT_STAGED))
	} else if (!status_run(view, STATUS_DIFF_INDEX_CMD, 0, LINE_STAT_STAGED)) {
		return FALSE;
	if (!status_run(view, STATUS_DIFF_FILES_CMD, 0, LINE_STAT_UNSTAGED) ||
	    !status_run(view, STATUS_LIST_OTHER_CMD, '?', LINE_STAT_UNTRACKED))
	const char *text;
	const char *info;
static bool
status_revert(struct status *status, enum line_type type, bool has_none)
{
	if (!status || type != LINE_STAT_UNSTAGED) {
		if (type == LINE_STAT_STAGED) {
			report("Cannot revert changes to staged files");
		} else if (type == LINE_STAT_UNTRACKED) {
			report("Cannot revert changes to untracked files");
		} else if (has_none) {
			report("Nothing to revert");
		} else {
			report("Cannot revert changes to multiple files");
		}
		return FALSE;

	} else {
		char cmd[SIZEOF_STR];
		char file_sq[SIZEOF_STR];

		if (sq_quote(file_sq, 0, status->old.name) >= sizeof(file_sq) ||
		    !string_format(cmd, "git checkout %s%s", opt_cdup, file_sq))
			return FALSE;

		return run_confirm(cmd, "Are you sure you want to overwrite any changes?");
	}
}

	case REQ_STATUS_REVERT:
		if (!status_revert(status, line->type, status_has_none(view, line)))
			return REQ_NONE;
		break;

	const char *text;
	const char *key;
		const char *text;
	const char *buf = line->data;
stage_apply_chunk(struct view *view, struct line *chunk, bool revert)
				"git apply --whitespace=nowarn %s %s - && "
				revert ? "" : "--cached",
				revert || stage_line_type == LINE_STAT_STAGED ? "-R" : ""))
		if (!stage_apply_chunk(view, chunk, FALSE)) {
static bool
stage_revert(struct view *view, struct line *line)
{
	struct line *chunk = NULL;

	if (!opt_no_head && stage_line_type == LINE_STAT_UNSTAGED)
		chunk = stage_diff_find(view, line, LINE_DIFF_CHUNK);

	if (chunk) {
		if (!prompt_yesno("Are you sure you want to revert changes?"))
			return FALSE;

		if (!stage_apply_chunk(view, chunk, TRUE)) {
			report("Failed to revert chunk");
			return FALSE;
		}
		return TRUE;

	} else {
		return status_revert(stage_status.status ? &stage_status : NULL,
				     stage_line_type, FALSE);
	}
}


	case REQ_STATUS_REVERT:
		if (!stage_revert(view, line))
			return REQ_NONE;
		break;

	open_view(view, REQ_VIEW_STAGE, OPEN_REFRESH);
static void
clear_rev_graph(struct rev_graph *graph)
{
	graph->boundary = 0;
	graph->size = graph->pos = 0;
	graph->commit = NULL;
	memset(graph->parents, 0, sizeof(*graph->parents));
}

	clear_rev_graph(graph);
push_rev_graph(struct rev_graph *graph, const char *parent)
	    draw_field(view, LINE_MAIN_AUTHOR, commit->author, opt_author_cols, TRUE))
		int i;

		if (view->lines > 0) {
			commit = view->line[view->lines - 1].data;
			if (!*commit->author) {
				view->lines--;
				free(commit);
				graph->commit = NULL;
			}
		}

		for (i = 0; i < ARRAY_SIZE(graph_stacks); i++)
			clear_rev_graph(&graph_stacks[i]);
	switch (request) {
	case REQ_ENTER:
		break;
	case REQ_REFRESH:
		load_refs();
		open_view(view, REQ_VIEW_MAIN, OPEN_REFRESH);
		break;
	default:
	}
static bool
prompt_yesno(const char *prompt)
{
	enum { WAIT, STOP, CANCEL  } status = WAIT;
	bool answer = FALSE;

	while (status == WAIT) {
		struct view *view;
		int i, key;

		input_mode = TRUE;

		foreach_view (view, i)
			update_view(view);

		input_mode = FALSE;

		mvwprintw(status_win, 0, 0, "%s [Yy]/[Nn]", prompt);
		wclrtoeol(status_win);

		/* Refresh, accept single keystroke of input */
		key = wgetch(status_win);
		switch (key) {
		case ERR:
			break;

		case 'y':
		case 'Y':
			answer = TRUE;
			status = STOP;
			break;

		case KEY_ESC:
		case KEY_RETURN:
		case KEY_ENTER:
		case KEY_BACKSPACE:
		case 'n':
		case 'N':
		case '\n':
		default:
			answer = FALSE;
			status = CANCEL;
		}
	}

	/* Clear the status window */
	status_empty = FALSE;
	report("");

	return answer;
}

static int
compare_refs(const void *ref1_, const void *ref2_)
{
	const struct ref *ref1 = *(const struct ref **)ref1_;
	const struct ref *ref2 = *(const struct ref **)ref2_;

	if (ref1->tag != ref2->tag)
		return ref2->tag - ref1->tag;
	if (ref1->ltag != ref2->ltag)
		return ref2->ltag - ref2->ltag;
	if (ref1->head != ref2->head)
		return ref2->head - ref1->head;
	if (ref1->tracked != ref2->tracked)
		return ref2->tracked - ref1->tracked;
	if (ref1->remote != ref2->remote)
		return ref2->remote - ref1->remote;
	return strcmp(ref1->name, ref2->name);
}

get_refs(const char *id)
		ref_list[ref_list_size]->next = 1;

	if (ref_list) {
		qsort(ref_list, ref_list_size, sizeof(*ref_list), compare_refs);
		ref_list[ref_list_size - 1]->next = 0;
	}
		 * the commit id of an annotated tag right before the commit id
	if (!*opt_git_dir)
		return OK;

	while (refs_size > 0)
		free(refs[--refs_size].name);
	while (id_refs_size > 0)
		free(id_refs[--id_refs_size]);

	return read_properties(popen("git " GIT_CONFIG " --list", "r"),
main(int argc, const char *argv[])
	request = parse_options(argc, argv);
	if (request == REQ_NONE)
	if (!opt_git_dir[0] && request != REQ_VIEW_PAGER)
	if (load_refs() == ERR)
					request = REQ_VIEW_DIFF;
					request = REQ_VIEW_PAGER;

				/* Always reload^Wrerun commands from the prompt. */
				open_view(view, request, OPEN_RELOAD);
			const char *prompt = request == REQ_SEARCH ? "/" : "?";