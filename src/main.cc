/*
 *	subtitleeditor -- a tool to create or edit subtitle
 *
 *	http://home.gna.org/subtitleeditor/
 *	https://gna.org/projects/subtitleeditor/
 *
 *	Copyright @ 2005-2009, kitone
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation; either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <config.h>
#include <gtkmm/main.h>
#include "gui/application.h"
#include "utility.h"
#include "gtkmm_utility.h"
#include "options.h"

#include <ctime>
#include <gstreamermm.h>
#include <gdk/gdkx.h>
#include <glib.h>

#ifdef ENABLE_GL
	#include <gtkglmm.h>
#endif//ENABLE_GL


#ifdef DEBUG

void parse_debug_options(OptionGroup &op)
{
	int flags = 0;
	if(op.debug_all)
	{
		std::cout << "DEBUG_ALL" << std::endl;

		flags |= SE_DEBUG_ALL;

		se_debug_init(flags);
	}
	else
	{
		if(op.debug_app)
			flags |= SE_DEBUG_APP;
		if(op.debug_view)
			flags |= SE_DEBUG_VIEW;
		if(op.debug_io)
			flags |= SE_DEBUG_IO;
		if(op.debug_search)
			flags |= SE_DEBUG_SEARCH;
		if(op.debug_regex)
			flags |= SE_DEBUG_REGEX;
		if(op.debug_video_player)
			flags |= SE_DEBUG_VIDEO_PLAYER;
		if(op.debug_spell_checking)
			flags |= SE_DEBUG_SPELL_CHECKING;
		if(op.debug_waveform)
			flags |= SE_DEBUG_WAVEFORM;
		if(op.debug_utility)
			flags |= SE_DEBUG_UTILITY;
		if(op.debug_command)
			flags |= SE_DEBUG_COMMAND;
		if(op.debug_plugins)
			flags |= SE_DEBUG_PLUGINS;
		if(op.debug_profiling)
			flags |= SE_DEBUG_PROFILING;

		se_debug_init(flags);
	}
}

#endif//DEBUG


/*
 *
 */
int main(int argc, char *argv[])
{
	if(!g_thread_supported())
		g_thread_init (NULL);

	// DEBUG
	std::clock_t start = std::clock();

	// Bindtextdomain
	bindtextdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);

	// init Gtk+
	Gtk::Main kit(argc, argv);

#ifdef ENABLE_GL
	// init OpenGL
	Gtk::GL::init_check(argc, argv);
#endif//ENABLE_GL

	Glib::set_application_name("subtitleeditor");

	// SubtitleEditor Options
	OptionGroup options;
	try
	{
		Glib::OptionContext context(_(" - edit subtitles files"));
		context.set_main_group(options);
		
		Glib::OptionGroup gst_group(gst_init_get_option_group());
		context.add_group(gst_group);
		
		Glib::OptionGroup gtk_group(gtk_get_option_group(TRUE));
		context.add_group(gtk_group);

		context.parse(argc, argv);
	}
	catch(const Glib::Error &ex)
	{
		std::cerr << "Error loading options : " << ex.what() << std::endl;
	}
	
	// Init Debug
#ifdef DEBUG
	parse_debug_options(options);
	se_debug_message(SE_DEBUG_APP, "Startup subtitle version %s", VERSION);
#endif//DEBUG

	if(!options.profile.empty())
		set_profile_name(options.profile);

	// Init GStreamer 
	se_debug_message(SE_DEBUG_APP, "Init GStreamer");
	
	Gst::init(argc, argv);

	// Run Application
	Application*	application = gtkmm_utility::get_widget_derived<Application>(
			SE_DEV_VALUE(PACKAGE_UI_DIR, PACKAGE_UI_DIR_DEV),
			"subtitleeditor.ui", 
			"window-main");

	if(!application)
		return EXIT_FAILURE;

	application->init(options);

	application->show();

	// DEBUG
	std::cout << "application run: " << double(std::clock() - start) / CLOCKS_PER_SEC << std::endl;
	
	kit.run(*application);

	delete application;

	return EXIT_SUCCESS;
}
