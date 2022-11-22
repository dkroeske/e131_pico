%code {

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "settings.h"
#include "lex.yy.h"

int yyerror(CONFIG_STRUCT *config, char *msg);

}

%code requires {
	typedef struct {
		char ssid[50];
		char password[50];
		unsigned int universe;
	} CONFIG_STRUCT;
}

%define api.pure true

%parse-param {CONFIG_STRUCT *config}

%union {
	int number;
	char *string;
}

%token<number> NUMBER 
%token<string> ID 
%token WIFI_TOK DMX_TOK SSID_TOK PASSWORD_TOK UNIVERSE_TOK
%token QUOTE OBRAC EBRAC EQUAL

%start config

%%

config:		%empty
		|
		config WIFI_TOK wifi_content
		|
		config DMX_TOK dmx_content
;

wifi_content:	%empty
		|
		wifi_content SSID_TOK EQUAL ID 
		{ 
			strcpy(config->ssid, yylval.string);
		}
		|
		wifi_content PASSWORD_TOK EQUAL ID 
		{ 
			strcpy(config->password, yylval.string);
		}
;

dmx_content:	%empty
		|
		dmx_content UNIVERSE_TOK EQUAL NUMBER 
		{ 
			config->universe = yylval.number;
		}
;

%%

int yyerror(CONFIG_STRUCT *config, char *msg){
	fprintf(stderr, "error: %s\n", msg);
	return 0;
}

