/*
 * Copyright (c) 2021 Shlomi Vaknin
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <fs/fs.h>
#include <shell/shell.h>

#define BUF_CNT 64

static int cmd_cat(const struct shell *shell, size_t argc, char **argv)
{
	uint8_t buf[BUF_CNT];
	struct fs_dirent dirent;
	struct fs_file_t file;
	int err;
	ssize_t read;

	fs_file_t_init(&file);

	for (size_t i = 1; i < argc; ++i) {
		err = fs_stat(argv[i], &dirent);
		if (err < 0) {
			shell_error(shell, "Failed to obtain file %s (err: %d)",
					argv[i], err);
			continue;
		}

		if (dirent.type != FS_DIR_ENTRY_FILE) {
			shell_error(shell, "Note a file %s", argv[i]);
			continue;
		}

		err = fs_open(&file, argv[i], FS_O_READ);
		if (err < 0) {
			shell_error(shell, "Failed to open %s (%d)", argv[i], err);
			continue;
		}

		while (true) {
			read = fs_read(&file, buf, sizeof(buf));
			if (read <= 0) {
				break;
			}

			for (int j = 0; j < read; j++) {
				shell_fprintf(shell, SHELL_NORMAL, "%c", buf[j]);
			}
        }

		if (read < 0) {
			shell_error(shell, "Failed to read from file %s (err: %d)",
				argv[i], read);
		}

		fs_close(&file);
	}

	return 0;
}

SHELL_CMD_ARG_REGISTER(cat, NULL,
    "Concatenate files and print on the standard output", cmd_cat, 2, 255);
