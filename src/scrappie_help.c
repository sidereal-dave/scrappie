#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include "scrappie_subcommands.h"

char * help_options[2] = {NULL, "--help"};

static const char * scrappie_help_header =
"Scrappie is a technology demonstrator for the Oxford Nanopore Technologies\n"
"Limited Research Algorithms group.\n";

static const char * scrappie_help_footer =
"This project began life as a proof (bet) that a base caller could be written\n"
"in a low level language in under 8 hours.  Some of the poor and just plain odd\n"
"design decisions, along with the lack of documentation, are a result of its\n"
"inception. In keeping with ONT's fish naming policy, the project was originally\n"
"called Crappie.\n"
"\n"
"Scrappie's purpose is to demonstrate the next generation of base calling and, as\n"
"such, may change drastically between releases and breaks backwards\n"
"compatibility.  Newer version may drop support older features.\n";

static const char * scrappie_licence_text =
"Proprietary and confidential information of Oxford Nanopore Technologies, Limited\n"
"All rights reserved; (c) 2016, 2017: Oxford Nanopore Technologies, Limited\n"
"\n"
"Distributed under the Nanopore Community terms and conditions.\n"
"\n"
"The vectorised math functions 'sse_mathfun.h' are from\n"
"http://gruntthepeon.free.fr/ssemath/ and the original version of this file is\n"
"under the 'zlib' licence.\n";


int main_help_short(void){
	int ret = fputs(scrappie_help_header, stdout);
	if(EOF != ret){
		fputc('\n', stdout);
		ret = fprint_scrappie_commands(stdout, true);
	}
	return (EOF != ret) ? EXIT_SUCCESS : EXIT_FAILURE;
}

int main_help(int argc, char * argv[]){
	if(argc == 1){
		int ret = fputs(scrappie_help_header, stdout);
		if(EOF != ret){
			fputc('\n', stdout);
			ret = fprint_scrappie_commands(stdout, true);
		}
		if(EOF != ret){
			fputc('\n', stdout);
			ret = fputs(scrappie_help_footer, stdout);
		}
		return (EOF != ret) ? EXIT_SUCCESS : EXIT_FAILURE;
	}

	int ret = EXIT_FAILURE;
	switch(get_scrappie_mode(argv[1])){
		case SCRAPPIE_MODE_HELP:
			printf("Recursively calling scrappie help\n");
			break;
		case SCRAPPIE_MODE_EVENTS:
			help_options[0] = argv[1];
			ret = main_events(2, help_options);
			break;
		case SCRAPPIE_MODE_RAW:
			help_options[0] = argv[1];
			ret = main_raw(2, help_options);
			break;
		default:
			ret = EXIT_FAILURE;
			warnx("Unrecognised subcommand %s\n", argv[1]);
	}

	return ret;
}


int main_licence(int argc, char * argv[]){
	int ret = fputs(scrappie_licence_text, stdout);
	return (EOF != ret) ? EXIT_SUCCESS : EXIT_FAILURE;
}

