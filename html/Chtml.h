#pragma once
#include <string>
#include	<vector>
#include "htmlTag.h"
namespace HTML{
	class Chtml
	{
		std::string m_text;
		void contents(std::vector<TAGS>& _ignore_tags,
			std::vector<std::string> &_content)const;

		void get_ignore_html_tags(std::vector<TAGS> &_tags)const;

		void content_within(std::vector<TAGS>& _ignore_tags,
			unsigned long _index,
			std::vector<std::string> &_content)const;

		unsigned long get_next_tag_start(const unsigned long &_current_index)const;

		unsigned long get_next_tag_end(const unsigned long &_current_index)const;

		TAGS get_tag_at(const unsigned long &_index)const;

		TAGS get_tag(std::string _tag)const;

		void get_contents_for_tag_at(const unsigned long &_tag_start,
								std::string &_contents)const;

		void remove_reserved_tokens();

	public:
		Chtml();

		Chtml(const std::string &_text);

		void set_text(const std::string &_text);
		
		void plain_text(std::vector<std::string> &_text)const;

		static void remove_html_reserved_tokens(std::string &_str);

		~Chtml();
	};
};


