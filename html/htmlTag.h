#ifndef HTMLTAGS_H
#define HTMLTAGS_H
#include <string>

// Online reference for the html tags 
//http://www.html-5-tutorial.com/all-html-tags.htm

namespace HTML{
	enum TAGS{		doctype,		a,			abbr,
					acronym,		address,	applet,
					area,			article,	aside,
					audio,			b,			base,
					basefont,		bdi,		bdo,
					big,			blockquote,	body,
					br,				button,		canvas,
					caption,		center,		cite,
					code,			col,		colgroup,
					command,		datalist,	dd,
					del,			details,	dfn,
					dir,			div,		dl,
					dt,				em,			embed,
					fieldset,		figcaption,	figure,
					font,			footer,		form,
					frame,			frameset,	h1,
					h2,				h3,			h4,
					h5,				h6,			head,
					hgroup,			hr,			html,
					i,				iframe,		img,
					input,			ins,		kbd,
					keygen,			label,		legend,
					li,				link,		mup,
					mark,			menu,		meta,
					meter,			nav,		noframes,
					noscript,		object,		ol,
					optgroup,		option,		output,
					p,				param,		pre,
					progress,		q,			rp,
					rt,				ruby,		s,
					samp,			script,		section,
					select,			small,		source,
					span,			strike,		strong,
					style,			sub,		summary,
					sup,			table,		tbody,
					td,				textarea,	tfoot,
					th,				thead,		time,
					title,			tr,			track,
					tt,				u,			ul,
					var,			video,		wbr,
					comment_tag,
					total_tags	};

	const static std::string tags_str[TAGS::total_tags+1] = {"!doctype",		"a",			"abbr",
															"acronym",		"address",		"applet",
															"area",			"article",		"aside",
															"audio",		"b",			"base",
															"basefont",		"bdi",			"bdo",
															"big",			"blockquote",	"body",
															"br",			"button",		"canvas",
															"caption",		"center",		"cite",
															"code",			"col",			"colgroup",
															"command",		"datalist",		"dd",
															"del",			"details",		"dfn",
															"dir",			"div",			"dl",
															"dt",			"em",			"embed",
															"fieldset",		"figcaption",	"figure",
															"font",			"footer",		"form",
															"frame",		"frameset",		"h1",
															"h2",			"h3",			"h4",
															"h5",			"h6",			"head",
															"hgroup",		"hr",			"html",
															"i",			"iframe",		"img",
															"input",		"ins",			"kbd",
															"keygen",		"label",		"legend",
															"li",			"link",			"mup",
															"mark",			"menu",			"meta",
															"meter",		"nav",			"noframes",
															"noscript",		"object",		"ol",
															"optgroup",		"option",		"output",
															"p",			"param",		"pre",
															"progress",		"q",			"rp",
															"rt",			"ruby",			"s",
															"samp",			"script",		"section",
															"select",		"small",		"source",
															"span",			"strike",		"strong",
															"style",		"sub",			"summary",
															"sup",			"table",		"tbody",
															"td",			"textarea",		"tfoot",
															"th",			"thead",		"time",
															"title",		"tr",			"track",
															"tt",			"u",			"ul",
															"var",			"video",		"wbr",
															"!--"
															"unknown_tag" };


	enum RESERVED_CHARS{		NON_BREAKING_SPACE,		LESS_THAN,		GREATER_THAN,
								AMPERSAND,				CENT,			POUND,
								YEN,					EURO,			COPYRIGHT,
								REGISTERED_TRADEMARK,
								TOTAL_RESERVED};

	const static std::string reserved_chars_tokens[RESERVED_CHARS::TOTAL_RESERVED + 1] =
	{ "&nbsp;", "&lt;", "&gt;",
	"&amp;", "&cent;", "&pound;",
	"&yen;", "&euro;", "&copy;",
	"&reg;",
	"&unknown_reserved;"
	};
	
	const static std::string reserved_chars_value[RESERVED_CHARS::TOTAL_RESERVED+1]=
	{
		" ", "<", ">",
		"&", "cent", "pound",
		"yen", "euro", "copyright",
		"registered trademark",
		"unknown reserved"
	};
};



#endif