cmake_minimum_required(VERSION 3.7)


FOREACH (this_dir 
			src
			test
	)
	ADD_SUBDIRECTORY (${this_dir})
ENDFOREACH (this_dir)
