/*****************************************************************************
 *
 * test_xsddefault.c - Test configuration loading
 *
 * Program: Icinga Core Testing
 * License: GPL
 * Copyright (c) 1999-2009 Ethan Galstad
 * Copyright (c) 2009-2013 Nagios Core Development Team and Community Contributors
 * Copyright (c) 2009-2013 Icinga Development Team (http://www.icinga.org)
 *
 * First Written:   06-01-2010, based on test_nagios_config.c
 *
 * Description:
 *
 * Tests Icinga status file reading
 *
 * License:
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 *****************************************************************************/

/* Need these to get CGI mode */
#undef NSCORE
#define NSCGI 1
#include "../include/config.h"
#include "../include/common.h"
#include "../include/locations.h"
#include "../include/statusdata.h"
#include "../include/comments.h"
#include "../include/downtime.h"
#include "../include/macros.h"
#include "../include/skiplist.h"
#include "tap.h"

extern comment *comment_list;
extern scheduled_downtime *scheduled_downtime_list;

/* Icinga cgiutils.c special (extern!) */
char alert_message;
char *host_name;
char *host_filter;
char *hostgroup_name;
char *service_desc;
char *servicegroup_name;
char *service_filter;
int host_alert;
int show_all_hosts;
int show_all_hostgroups;
int show_all_servicegroups;
int display_type;
int overview_columns;
int max_grid_width;
int group_style_type;
int navbar_search;
int CGI_ID;

int main(int argc, char **argv) {
	int result;
	int c;
	int last_id;
	time_t last_time;
	comment *temp_comment;
	scheduled_downtime *temp_downtime;

	plan(7);

	chdir("../t");

	ok(system("cat var/status.dat > var/status-generated.dat") == 0, "New status.dat file");
	ok(system("bin/generate_downtimes 10 >> var/status-generated.dat") == 0, "Generated 10 downtimes");

	result = read_all_status_data("etc/cgi-with-generated-status.cfg", READ_ALL_STATUS_DATA);
	ok(result == OK, "Read cgi status data okay");

	temp_comment = comment_list;
	last_id = 0;
	c = 0;
	result = OK;
	while (temp_comment != NULL) {
		c++;
		printf("id: %id\n", temp_comment->comment_id);
		if (temp_comment->comment_id <= last_id) {
			result = ERROR;
			break;
		}
		last_id = temp_comment->comment_id;
		temp_comment = temp_comment->next;
	}
	ok(c == 12, "Got %d comments - expected 12", c);
	ok(result == OK, "All comments in order");

	temp_downtime = scheduled_downtime_list;
	last_time = 0;
	c = 0;
	result = OK;
	while (temp_downtime != NULL) {
		c++;
		if (temp_downtime->start_time < last_time) {
			result = ERROR;
			break;
		}
		last_time = temp_downtime->start_time;
		temp_downtime = temp_downtime->next;
	}
	ok(c == 20, "Got %d downtimes - expected 20", c);
	ok(result == OK, "All downtimes in order");

	return exit_status();
}


