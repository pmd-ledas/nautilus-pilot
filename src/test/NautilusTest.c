/*  -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <glib.h>
#include <nautilus-extension.h>


/*\
|*|
|*| BUILD SETTINGS
|*|
\*/


#ifdef ENABLE_NLS
#include <glib/gi18n-lib.h>
#define I18N_INIT() \
	bindtextdomain(GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
#else
#define _(STRING) ((char * ) STRING)
#define I18N_INIT()
#endif

/*\
|*|
|*| GLOBAL TYPES AND VARIABLES
|*|
\*/

int сurrentEmblem = 0;
static char *emblems[] = {"people", "web"};

typedef struct {
	GObject parent_slot;
} NautilusTest;

typedef struct {
	GObjectClass parent_slot;
} NautilusTestClass;

static GType provider_types[1];
static GType nautilus_test_type;
static GObjectClass * parent_class;

/*\
|*|
|*| FUNCTIONS
|*|
\*/

int ChangeFileEmblem(NautilusFileInfo *file);

static void on_do_stuff_selected_menuitem_activate (
	NautilusMenuItem * const menu_item,
	gpointer const user_data
) {

	//This is the function that is invoked when the user clicks on the "Do something" menu entry.

	gchar * file_uri;

	GList * const file_selection = g_object_get_data(
		G_OBJECT(menu_item),
		"nautilus_test_files"
	);

	for (GList * iter = file_selection; iter; iter = iter->next) 
	{
		// Launch Nautilus from a terminal to see this 
		//file_uri = nautilus_file_info_get_uri(NAUTILUS_FILE_INFO(iter->data));
		//g_message(_("Doing something with %s ..."), file_uri);
		//g_free(file_uri);

		ChangeFileEmblem(NAUTILUS_FILE_INFO(iter->data));
	}
}


int ChangeFileEmblem(NautilusFileInfo *file)
{
	nautilus_file_info_add_emblem(file, emblems[сurrentEmblem]);
	++сurrentEmblem;
	
	if(сurrentEmblem > 1)
		сurrentEmblem = 0;
}

static GList * nautilus_test_get_file_items (
	NautilusMenuProvider * const menu_provider,
	GtkWidget * const nautilus_window,
	GList * const file_selection
) 
{

	/*
	Uncomment the following block if you want the "Do something" menu entry not
	to be shown if a directory is found among the selected files.

	If you want instead to show the menu entry only when a particular mimetype
	is selected, use as condition:
	`!nautilus_file_is_mime_type(NAUTILUS_FILE_INFO(iter->data), "application/x-[MIME TYPE]")`

	For further information see the `NautilusFileInfo` interface at:
	https://developer.gnome.org/libnautilus-extension/stable/NautilusFileInfo.html
	*/

	// Convert files to filenames.
	int file_count = g_list_length(file_selection);

	if (file_count < 1)
		return NULL;

	gchar **paths = g_new0(gchar *, file_count + 1);
	int i = 0;
	GList* elem;

	for (elem = file_selection; elem; elem = elem->next, i++) 
	{
		gchar *uri = nautilus_file_info_get_uri(elem->data);
		gchar *filename_un = uri ? g_filename_from_uri(uri, NULL, NULL) : NULL;
		gchar *filename = filename_un ? g_filename_to_utf8(filename_un, -1, NULL, NULL, NULL) : NULL;

		g_free(uri);
		g_free(filename_un);

		if (filename == NULL) {
		// oooh, filename wasn't correctly encoded, or isn't a local file.
		g_strfreev(paths);
		return NULL;
		}

		paths[i] = filename;
	}

	gchar *filename = (char*) malloc((strlen(paths[0]) + 1 )* sizeof(char));
	strncpy(filename,paths[0],(strlen(paths[0]) + 1 ));
	g_free(paths);
	////////////////////////////////////////////////////////////////////////////////

	for (GList * iter = file_selection; iter; iter = iter->next) {

		if (nautilus_file_info_is_directory(NAUTILUS_FILE_INFO(iter->data))) {

			return NULL;

		}

	}

	NautilusMenuItem * const menu_item = nautilus_menu_item_new(
		"NautilusTest::root_item",
        filename,
		//"NautilusTest::do_stuff_selected",
		//_("Do something 1"),
		_("Some longer description of what you do"),
		NULL // icon name or `NULL`
	);

	g_signal_connect(
		menu_item,
		"activate",
		G_CALLBACK(on_do_stuff_selected_menuitem_activate),
		NULL // `NULL` or any custom user data 
	);

	g_object_set_data_full(
		G_OBJECT(menu_item),
		"nautilus_test_files",
		nautilus_file_info_list_copy(file_selection),
		(GDestroyNotify) nautilus_file_info_list_free
	);

	g_free(filename);
	return g_list_append(NULL, menu_item);
}


static void nautilus_test_menu_provider_iface_init (
	NautilusMenuProviderIface * const iface,
	gpointer const iface_data
) {

	iface->get_file_items = nautilus_test_get_file_items;

}


static void nautilus_test_class_init (
	NautilusTestClass * const nautilus_test_class,
	gpointer class_data
) {

	parent_class = g_type_class_peek_parent(nautilus_test_class);

}


static void nautilus_test_register_type (
	GTypeModule * const module
) {

	static const GTypeInfo info = {
		sizeof(NautilusTestClass),
		(GBaseInitFunc) NULL,
		(GBaseFinalizeFunc) NULL,
		(GClassInitFunc) nautilus_test_class_init,
		(GClassFinalizeFunc) NULL,
		NULL,
		sizeof(NautilusTest),
		0,
		(GInstanceInitFunc) NULL,
		(GTypeValueTable *) NULL
	};

	nautilus_test_type = g_type_module_register_type(
		module,
		G_TYPE_OBJECT,
		"NautilusTest",
		&info,
		0
	);

	static const GInterfaceInfo menu_provider_iface_info = {
		(GInterfaceInitFunc) nautilus_test_menu_provider_iface_init,
		(GInterfaceFinalizeFunc) NULL,
		NULL
	};

	g_type_module_add_interface(
		module,
		nautilus_test_type,
		NAUTILUS_TYPE_MENU_PROVIDER,
		&menu_provider_iface_info
	);

}


GType nautilus_test_get_type (void) {

	return nautilus_test_type;

}


void nautilus_module_shutdown (void) {

	/*  Any module-specific shutdown  */

}


void nautilus_module_list_types (
	const GType ** const types,
	int * const num_types
) {

	*types = provider_types;
	*num_types = G_N_ELEMENTS(provider_types);

}


// Extension initialization 
void nautilus_module_initialize (
	GTypeModule * const module
) {

	I18N_INIT();
	nautilus_test_register_type(module);
	*provider_types = nautilus_test_get_type();

}


/*  EOF  */
