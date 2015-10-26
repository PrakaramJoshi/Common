#ifndef FILE_UTILS_H
#define FILE_UTILS_H
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <vector>
#include <string>
#include <sstream>
namespace FileUtils{
	//Open a file in binary mode
	inline bool open_binary(const std::string &_path,
					std::ifstream &_ifs){

		_ifs.open(_path.c_str(), std::ios::in | std::ios::binary);
		return !_ifs.is_open();
	}
	
	//Reading a binary file into an existing std::vector
	inline bool read_binary_to_buffer(const std::string &_path,
		std::vector<char> &_buffer){
		std::ifstream ifs;
		if (!open_binary(_path, ifs))
			return false;
		_buffer.assign(std::istreambuf_iterator<char>(ifs), std::istreambuf_iterator<char>());
		ifs.close();
	}	

	//Reading a text file into a string(note the extra parathesis for std::istreambuf_iterator)
	inline bool read_binary_to_str(const std::string &_path, std::string &_str){
		std::ifstream ifs;
		if (!open_binary(_path, ifs))
			return false;
		_str.assign((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		ifs.close();
	}

	//Reading a text file using pure C
	inline int read_file(const char* path, char* buf, int len) {

		/* try to open the file */
		FILE* fp = fopen(path, "r");
		if (!fp) {
			//Error: cannot read file
			return -1;
		}

		/* find size */
		long size = 0;
		fseek(fp, 0, SEEK_END);
		size = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		if (size > len) {
			//Error: file size it too large for given buffer. We need size bytes.
			fclose(fp);
			return -2;
		}

		size_t r = fread(buf, size, 1, fp);
		if (r != 1) {
			//Error: cannot read file into buffer
			fclose(fp);
			return -3;
		}

		return (int)size;
	}

	//Check the file size
	inline size_t file_size(std::ifstream &_ifs){
		_ifs.seekg(0, std::ifstream::end);
		return _ifs.tellg();
	}
		
	//Jump to a position
	inline bool jump_to_position(std::ifstream &_ifs, int _pos){
		_ifs.seekg(_pos, std::ios_base::beg);
		if (!_ifs) {
			//Error trying to jump to a position
			return false;
		}
		return true;
	}
		
	//Get the curent position
	inline size_t get_current_pos(std::ifstream &_ifs){
		return _ifs.tellg();
	}

	//Read some bytes
	inline size_t read_bytes(const size_t &num_of_bytes, std::ifstream &_ifs, char *&_buffer){
		_buffer = new char[num_of_bytes];
		size_t read_size = num_of_bytes;
		size_t fsize = file_size(_ifs);
		if (fsize < read_size) {
			read_size = fsize;
			//The file is smaller then the number buffer.
		}
		//Reading: read_size
		_ifs.read(_buffer, read_size);
		if (!_ifs) {
			//Error trying to into buffer
			delete[] _buffer;
			return 0;
		}

		return _ifs.gcount();
	}
	
	//Write some bytes
	inline bool write_bytes(const std::vector<char> &_buffer, const std::string &_path){
		std::ofstream ofs(_path.c_str(), std::ios::binary | std::ios::out);
		if (ofs.is_open()){
			ofs.write(&_buffer.front(), _buffer.size());
			ofs.close();
			return true;
		}
		
		return false;
	}
		
	//Write the contents of a std::stringstream to a file
	inline bool write_sstream_to_file(std::stringstream &_str, const std::string &_path){
		std::ofstream ofs(_path.c_str(), std::ios::out);
		if (!ofs.is_open()) {
			return false;
		}

		ofs << _str.rdbuf();

		ofs.close();

		return true;
	}
		

}
#endif