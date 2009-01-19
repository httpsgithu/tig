/* Copyright (c) 2006-2009 Jonas Fonseca <fonseca@diku.dk>
#include <sys/wait.h>
#include <sys/select.h>
#include <fcntl.h>
	IO_BG,			/* Execute command in the background. */
	pid_t pid;		/* Pipe for reading or writing. */
	int pipe;		/* Pipe end for reading or writing. */
	const char *argv[SIZEOF_ARG];	/* Shell command arguments. */
	char *buf;		/* Read buffer. */
	size_t bufsize;		/* Buffer content size. */
	char *bufpos;		/* Current buffer position. */
	unsigned int eof:1;	/* Has end of file been reached. */
	io->pipe = -1;
	io->pid = 0;
	io->buf = io->bufpos = NULL;
	io->bufalloc = io->bufsize = 0;
	io->eof = 0;
	return format_argv(io->argv, argv, flags);
io_open(struct io *io, const char *name)
	io->pipe = *name ? open(name, O_RDONLY) : STDIN_FILENO;
	return io->pipe != -1;
}

static bool
kill_io(struct io *io)
{
	return kill(io->pid, SIGKILL) != -1;
	pid_t pid = io->pid;

	if (io->pipe != -1)
		close(io->pipe);

	while (pid > 0) {
		int status;
		pid_t waiting = waitpid(pid, &status, 0);

		if (waiting < 0) {
			if (errno == EINTR)
				continue;
			report("waitpid failed (%s)", strerror(errno));
			return FALSE;
		}

		return waiting == pid &&
		       !WIFSIGNALED(status) &&
		       WIFEXITED(status) &&
		       !WEXITSTATUS(status);
	}

	int pipefds[2] = { -1, -1 };
	if (io->type == IO_FD)
		return TRUE;
	if ((io->type == IO_RD || io->type == IO_WR) &&
	    pipe(pipefds) < 0)
	if ((io->pid = fork())) {
		if (pipefds[!(io->type == IO_WR)] != -1)
			close(pipefds[!(io->type == IO_WR)]);
		if (io->pid != -1) {
			io->pipe = pipefds[!!(io->type == IO_WR)];
			return TRUE;
		}

	} else {
		if (io->type != IO_FG) {
			int devnull = open("/dev/null", O_RDWR);
			int readfd  = io->type == IO_WR ? pipefds[0] : devnull;
			int writefd = io->type == IO_RD ? pipefds[1] : devnull;

			dup2(readfd,  STDIN_FILENO);
			dup2(writefd, STDOUT_FILENO);
			dup2(devnull, STDERR_FILENO);

			close(devnull);
			if (pipefds[0] != -1)
				close(pipefds[0]);
			if (pipefds[1] != -1)
				close(pipefds[1]);
		}

		if (io->dir && *io->dir && chdir(io->dir) == -1)
			die("Failed to change directory: %s", strerror(errno));

		execvp(io->argv[0], (char *const*) io->argv);
		die("Failed to execute program: %s", strerror(errno));
	}
	if (pipefds[!!(io->type == IO_WR)] != -1)
		close(pipefds[!!(io->type == IO_WR)]);
	return FALSE;
run_io(struct io *io, const char **argv, const char *dir, enum io_type type)
	init_io(io, dir, type);
	if (!format_argv(io->argv, argv, FORMAT_NONE))
		return FALSE;
static int
run_io_bg(const char **argv)
{
	struct io io = {};

	init_io(&io, NULL, IO_BG);
	if (!format_argv(io.argv, argv, FORMAT_NONE))
		return FALSE;
	return run_io_do(&io);
}

	if (!format_argv(io.argv, argv, FORMAT_NONE))
	return io->eof;
static bool
io_can_read(struct io *io)
{
	struct timeval tv = { 0, 500 };
	fd_set fds;

	FD_ZERO(&fds);
	FD_SET(io->pipe, &fds);

	return select(io->pipe + 1, &fds, NULL, NULL, &tv) > 0;
}

static ssize_t
io_read(struct io *io, void *buf, size_t bufsize)
{
	do {
		ssize_t readsize = read(io->pipe, buf, bufsize);

		if (readsize < 0 && (errno == EAGAIN || errno == EINTR))
			continue;
		else if (readsize == -1)
			io->error = errno;
		else if (readsize == 0)
			io->eof = 1;
		return readsize;
	} while (1);
}

io_get(struct io *io, int c, bool can_read)
	char *eol;
	ssize_t readsize;

		io->buf = io->bufpos = malloc(BUFSIZ);
		io->bufsize = 0;
	}

	while (TRUE) {
		if (io->bufsize > 0) {
			eol = memchr(io->bufpos, c, io->bufsize);
			if (eol) {
				char *line = io->bufpos;

				*eol = 0;
				io->bufpos = eol + 1;
				io->bufsize -= io->bufpos - line;
				return line;
			}
		}

		if (io_eof(io)) {
			if (io->bufsize) {
				io->bufpos[io->bufsize] = 0;
				io->bufsize = 0;
				return io->bufpos;
			}
			return NULL;
		}

		if (!can_read)
			return NULL;

		if (io->bufsize > 0 && io->bufpos > io->buf)
			memmove(io->buf, io->bufpos, io->bufsize);

		io->bufpos = io->buf;
		readsize = io_read(io, io->buf + io->bufsize, io->bufalloc - io->bufsize);
		if (io_error(io))
			return NULL;
		io->bufsize += readsize;
}
static bool
io_write(struct io *io, const void *buf, size_t bufsize)
{
	size_t written = 0;

	while (!io_error(io) && written < bufsize) {
		ssize_t size;

		size = write(io->pipe, buf + written, bufsize - written);
		if (size < 0 && (errno == EAGAIN || errno == EINTR))
			continue;
		else if (size == -1)
		else
			written += size;
	return written == bufsize;
static bool
run_io_buf(const char **argv, char buf[], size_t bufsize)
{
	struct io io = {};
	bool error;

	if (!run_io_rd(&io, argv, FORMAT_NONE))
		return FALSE;

	io.buf = io.bufpos = buf;
	io.bufalloc = bufsize;
	error = !io_get(&io, '\n', TRUE) && io_error(&io);
	io.buf = NULL;

	return done_io(&io) || error;
}

static int read_properties(struct io *io, const char *separators, int (*read)(char *, size_t, char *, size_t));
parse_options(int argc, const char *argv[], const char ***run_argv)
	/* XXX: This is vulnerable to the user overriding options
	 * required for the main view parser. */
	static const char *custom_argv[SIZEOF_ARG] = {
		"git", "log", "--no-color", "--pretty=raw", "--parents",
			"--topo-order", NULL
	};
	int i, j = 6;
	if (!isatty(STDIN_FILENO))
	if (subcommand) {
		custom_argv[1] = subcommand;
		j = 2;
	}
		custom_argv[j++] = opt;
		if (j >= ARRAY_SIZE(custom_argv))
	custom_argv[j] = NULL;
	*run_argv = custom_argv;
	unsigned int cleareol:1;
	struct io io = {};
	if (!io_open(&io, path))
	if (read_properties(&io, " \t", read_option) == ERR ||
	time_t update_secs;
	if (line->cleareol)
		wclrtoeol(view->win);
	line->dirty = line->cleareol = 0;
		if (view->offset + lineno >= view->lines)
			break;
		if (!view->line[view->offset + lineno].dirty)
	if (view != VIEW(REQ_VIEW_STATUS) && view->lines) {
		string_format_from(state, &statelen, " - %s %d of %d (%d%%)",
	}
	if (view->pipe) {
		time_t secs = time(NULL) - view->start_time;

		/* Three git seconds are a long time ... */
		if (secs > 2)
			string_format_from(state, &statelen, " loading %lds", secs);
		string_format_from(buf, &bufpos, "%s", state);
static void
toggle_view_option(bool *option, const char *help)
{
	*option = !*option;
	redraw_display();
	report("%sabling %s", *option ? "En" : "Dis", help);
}

	view->update_secs = 0;
	if (force)
		kill_io(view->pipe);
prepare_update_file(struct view *view, const char *name)
	if (view->pipe)
		end_update(view, TRUE);
	return io_open(&view->io, name);
}
static bool
begin_update(struct view *view, bool refresh)
{
	if (view->pipe)
		end_update(view, TRUE);
	if (refresh) {
	/* Clear the view and redraw everything since the tree sorting
	 * might have rearranged things. */
	bool redraw = view->lines == 0;
	bool can_read = TRUE;
	if (!io_can_read(view->pipe)) {
		if (view->lines == 0) {
			time_t secs = time(NULL) - view->start_time;
			if (secs > view->update_secs) {
				if (view->update_secs == 0)
					redraw_view(view);
				update_view_title(view);
				view->update_secs = secs;
			}
		}
		return TRUE;
	}
	for (; (line = io_get(view->pipe, '\n', can_read)); can_read = FALSE) {
			size_t inlen = strlen(line) + 1;
			if (ret != (size_t) -1)
		unsigned long lines = view->lines;
			if (opt_line_number || view == VIEW(REQ_VIEW_BLAME))
				redraw = TRUE;
	if (redraw)
		redraw_view_from(view, 0);
	else
	struct line *line;

	if (!realloc_lines(view, view->lines + 1))
		return NULL;
	line = &view->line[view->lines++];
	line->dirty = 1;
static struct line *
add_line_format(struct view *view, enum line_type type, const char *fmt, ...)
{
	char buf[SIZEOF_STR];
	va_list args;

	va_start(args, fmt);
	if (vsnprintf(buf, sizeof(buf), fmt, args) >= sizeof(buf))
		buf[0] = 0;
	va_end(args);

	return buf[0] ? add_line_text(view, buf, type) : NULL;
}
	open_external_viewer(mergetool_argv, opt_cdup);
		if (!VIEW(REQ_VIEW_PAGER)->pipe && !VIEW(REQ_VIEW_PAGER)->lines) {
		toggle_view_option(&opt_line_number, "line numbers");
		toggle_view_option(&opt_date, "date display");
		toggle_view_option(&opt_author, "author display");
		toggle_view_option(&opt_rev_graph, "revision graph display");
		toggle_view_option(&opt_show_refs, "reference display");
	const char *describe_argv[] = { "git", "describe", commit_id, NULL };
	if (run_io_buf(describe_argv, refbuf, sizeof(refbuf)))
		ref = chomp_string(refbuf);
		add_line_format(view, LINE_DEFAULT, "    %-25s %s",
				key, req_info[i].help);
		add_line_format(view, LINE_DEFAULT, "    %-10s %-14s `%s`",
				keymap_table[req->keymap].name, key, cmd);
static int
tree_compare_entry(struct line *line1, struct line *line2)
{
	if (line1->type != line2->type)
		return line1->type == LINE_TREE_DIR ? -1 : 1;
	return strcmp(tree_path(line1), tree_path(line2));
}

	struct line *entry, *line;
	if (view->lines == 0 &&
	    !add_line_format(view, LINE_DEFAULT, "Directory path /%s", opt_path))
		return FALSE;

		/* Insert "link" to parent directory. */
		if (view->lines == 1 &&
		    !add_line_format(view, LINE_TREE_DIR, TREE_UP_FORMAT, view->ref))
			return FALSE;
	entry = add_line_text(view, text, type);
	if (!entry)
		return FALSE;
	text = entry->data;
	/* Skip "Directory ..." and ".." line. */
	for (line = &view->line[1 + !!*opt_path]; line < entry; line++) {
		if (tree_compare_entry(line, entry) <= 0)
		memmove(line + 1, line, (entry - line) * sizeof(*entry));
		for (; line <= entry; line++)
			line->dirty = line->cleareol = 1;
	if (*opt_ref || !io_open(&view->io, opt_file)) {
static bool
check_blame_commit(struct blame *blame)
{
	if (!blame->commit)
		report("Commit data not loaded yet");
	else if (!strcmp(blame->commit->id, NULL_ID))
		report("No commit exist for the selected line");
	else
		return TRUE;
	return FALSE;
}

		if (check_blame_commit(blame)) {
			string_copy(opt_ref, blame->commit->id);
			open_view(view, REQ_VIEW_BLAME, OPEN_REFRESH);
		break;
	if (bufsize < 98 ||
status_run(struct view *view, const char *argv[], char status, enum line_type type)
	char *buf;
	struct io io = {};
	if (!run_io(&io, argv, NULL, IO_RD))
	while ((buf = io_get(&io, 0, TRUE))) {
		if (!file) {
			file = calloc(1, sizeof(*file));
			if (!file || !add_line_data(view, file, type))
				goto error_out;
		}
		/* Parse diff info part. */
		if (status) {
			file->status = status;
			if (status == 'A')
				string_copy(file->old.rev, NULL_ID);
		} else if (!file->status) {
			if (!status_get_diff(file, buf, strlen(buf)))
				goto error_out;
			buf = io_get(&io, 0, TRUE);
			if (!buf)
				break;
			/* Collapse all 'M'odified entries that follow a
			 * associated 'U'nmerged entry. */
			if (file->status == 'U') {
				unmerged = file;
			} else if (unmerged) {
				int collapse = !strcmp(buf, unmerged->new.name);
				unmerged = NULL;
				if (collapse) {
					free(file);
					view->lines--;
					continue;
		}
		/* Grab the old name for rename/copy. */
		if (!*file->old.name &&
		    (file->status == 'R' || file->status == 'C')) {
			string_ncopy(file->old.name, buf, strlen(buf));
			buf = io_get(&io, 0, TRUE);
			if (!buf)
				break;

		/* git-ls-files just delivers a NUL separated list of
		 * file names similar to the second half of the
		 * git-diff-* output. */
		string_ncopy(file->new.name, buf, strlen(buf));
		if (!*file->old.name)
			string_copy(file->old.name, file->new.name);
		file = NULL;
	if (io_error(&io)) {
		done_io(&io);
	done_io(&io);
static const char *status_diff_index_argv[] = {
	"git", "diff-index", "-z", "--diff-filter=ACDMRTXB",
			     "--cached", "-M", "HEAD", NULL
};
static const char *status_diff_files_argv[] = {
	"git", "diff-files", "-z", NULL
};
static const char *status_list_other_argv[] = {
	"git", "ls-files", "-z", "--others", "--exclude-standard", NULL
};
static const char *status_list_no_head_argv[] = {
	"git", "ls-files", "-z", "--cached", "--exclude-standard", NULL
};

static const char *update_index_argv[] = {
	"git", "update-index", "-q", "--unmerged", "--refresh", NULL
};
	run_io_bg(update_index_argv);
		if (!status_run(view, status_list_no_head_argv, 'A', LINE_STAT_STAGED))
	} else if (!status_run(view, status_diff_index_argv, 0, LINE_STAT_STAGED)) {
	if (!status_run(view, status_diff_files_argv, 0, LINE_STAT_UNSTAGED) ||
	    !status_run(view, status_list_other_argv, '?', LINE_STAT_UNTRACKED))
	const char *oldpath = status ? status->old.name : NULL;
	/* Diffs for unmerged entries are empty when passing the new
	 * path, so leave it empty. */
	const char *newpath = status && status->status != 'U' ? status->new.name : NULL;
	struct view *stage = VIEW(REQ_VIEW_STAGE);
			const char *no_head_diff_argv[] = {
				"git", "diff", "--no-color", "--patch-with-stat",
					"--", "/dev/null", newpath, NULL
			};

			if (!prepare_update(stage, no_head_diff_argv, opt_cdup, FORMAT_DASH))
			const char *index_show_argv[] = {
				"git", "diff-index", "--root", "--patch-with-stat",
					"-C", "-M", "--cached", "HEAD", "--",
					oldpath, newpath, NULL
			};

			if (!prepare_update(stage, index_show_argv, opt_cdup, FORMAT_DASH))
	{
		const char *files_show_argv[] = {
			"git", "diff-files", "--root", "--patch-with-stat",
				"-C", "-M", "--", oldpath, newpath, NULL
		};

		if (!prepare_update(stage, files_show_argv, opt_cdup, FORMAT_DASH))
	}
		if (!newpath) {
		if (!prepare_update_file(stage, newpath))
			return REQ_QUIT;
		if (line->type != type)
			continue;
		if (!pos && (!status || !status->status))
			return TRUE;
		if (pos && !strcmp(status->new.name, pos->new.name))
static bool
status_update_prepare(struct io *io, enum line_type type)
	const char *staged_argv[] = {
		"git", "update-index", "-z", "--index-info", NULL
	};
	const char *others_argv[] = {
		"git", "update-index", "-z", "--add", "--remove", "--stdin", NULL
	};
		return run_io(io, staged_argv, opt_cdup, IO_WR);
		return run_io(io, others_argv, opt_cdup, IO_WR);

		return run_io(io, others_argv, NULL, IO_WR);
		return FALSE;
status_update_write(struct io *io, struct status *status, enum line_type type)
	return io_write(io, buf, bufsize);
	struct io io = {};
	if (!status_update_prepare(&io, type))
	result = status_update_write(&io, status, type);
	done_io(&io);
	struct io io = {};
	if (!status_update_prepare(&io, line->type))
		result = status_update_write(&io, line->data, line->type);
	done_io(&io);
stage_diff_write(struct io *io, struct line *line, struct line *end)
		if (!io_write(io, line->data, strlen(line->data)) ||
		    !io_write(io, "\n", 1))
		line++;
	const char *apply_argv[SIZEOF_ARG] = {
		"git", "apply", "--whitespace=nowarn", NULL
	};
	struct io io = {};
	int argc = 3;
	if (!revert)
		apply_argv[argc++] = "--cached";
	if (revert || stage_line_type == LINE_STAT_STAGED)
		apply_argv[argc++] = "-R";
	apply_argv[argc++] = "-";
	apply_argv[argc++] = NULL;
	if (!run_io(&io, apply_argv, opt_cdup, IO_WR))
	if (!stage_diff_write(&io, diff_hdr, chunk) ||
	    !stage_diff_write(&io, chunk, view->line + view->lines))
	done_io(&io);
	run_io_bg(update_index_argv);
		if (!prepare_update_file(view, stage_status.new.name)) {
			report("Failed to open file: %s", strerror(errno));
			return REQ_NONE;
		}
update_rev_graph(struct view *view, struct rev_graph *graph)
	if (view->lines > 2)
		view->line[view->lines - 3].dirty = 1;
	if (view->lines > 1)
		view->line[view->lines - 2].dirty = 1;
			view->line[view->lines - 1].dirty = 1;
		update_rev_graph(view, graph);
		update_rev_graph(view, graph);
		view->line[view->lines - 1].dirty = 1;
		view->line[view->lines - 1].dirty = 1;
 * Repository properties
static int
git_properties(const char **argv, const char *separators,
	       int (*read_property)(char *, size_t, char *, size_t))
{
	struct io io = {};

	if (init_io_rd(&io, argv, NULL, FORMAT_NONE))
		return read_properties(&io, separators, read_property);
	return ERR;
}

	static const char *ls_remote_argv[SIZEOF_ARG] = {
		"git", "ls-remote", ".", NULL
	};
	static bool init = FALSE;

	if (!init) {
		argv_from_env(ls_remote_argv, "TIG_LS_REMOTE");
		init = TRUE;
	}
	return git_properties(ls_remote_argv, "\t", read_ref);
	const char *config_list_argv[] = { "git", GIT_CONFIG, "--list", NULL };

	return git_properties(config_list_argv, "=", read_repo_config_option);
		string_ncopy(opt_cdup, name, namelen);
	const char *head_argv[] = {
		"git", "symbolic-ref", "HEAD", NULL
	};
	const char *rev_parse_argv[] = {
		"git", "rev-parse", "--git-dir", "--is-inside-work-tree",
			"--show-cdup", NULL
	};
	if (run_io_buf(head_argv, opt_head, sizeof(opt_head))) {
		chomp_string(opt_head);
		if (!prefixcmp(opt_head, "refs/heads/")) {
			char *offset = opt_head + STRING_SIZE("refs/heads/");
			memmove(opt_head, offset, strlen(offset) + 1);
		}
	}
	return git_properties(rev_parse_argv, "=", read_repo_info);
read_properties(struct io *io, const char *separators,
	if (!start_io(io))
	while (state == OK && (name = io_get(io, '\n', TRUE))) {
	if (state != ERR && io_error(io))
	done_io(io);
	const char **run_argv = NULL;
	request = parse_options(argc, argv, &run_argv);
	if (request == REQ_VIEW_PAGER || run_argv) {
		if (request == REQ_VIEW_PAGER)
			io_open(&VIEW(request)->io, "");
		else if (!prepare_update(VIEW(request), run_argv, NULL, FORMAT_NONE))
			die("Failed to format arguments");
		open_view(NULL, request, OPEN_PREPARED);
		request = REQ_NONE;
	}
