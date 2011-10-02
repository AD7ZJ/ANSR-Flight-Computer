class RootDir {
public:

	struct fat16_fs_struct;

	uint8_t print_disk_info(const struct fat16_fs_struct* fs);

	/* returns NULL if error, pointer if file opened */
	struct fat16_file_struct * root_open_new(char* name);

	/* returns 1 if file exists, 0 else */
	int root_file_exists(char* name);

	int openroot(void);

	struct fat16_file_struct * root_open(char* name);

	void root_disk_info(void);
	int rootDirectory_files(char* buf, int len);
	void root_format(void);
	char rootDirectory_files_stream(int reset);
	int root_delete(char* filename);

	struct fat16_dir_entry_struct dir_entry;
	struct fat16_fs_struct* fs;
	struct partition_struct* partition;
	struct fat16_dir_struct* dd;
	struct fat16_file_struct * fd;
};

