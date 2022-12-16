#include "FrameManager.hpp"
#include <gtk/gtk.h>

static void gtk_column_view_class_init    (GtkColumnViewClass *klass);
static void gtk_column_view_init          (GtkColumnView *cv);
static void gtk_column_view_scroll_right  (GtkContainer* container, GtkAllocation* allocation, gpointer data);

GType
gtk_column_view_get_type (void)
{
  static GType cv_type = 0;
  
  if (!cv_type)
  {
    static const GTypeInfo cv_info = {
      sizeof (GtkColumnViewClass),
      NULL, // base_init
      NULL, // base_finalize
      (GClassInitFunc) gtk_column_view_class_init,
      NULL, // class_finalize
      NULL, // class_data
      sizeof (GtkColumnView),
      0, // n_preallocs
      (GInstanceInitFunc) gtk_column_view_init,
    };
    
    cv_type = g_type_register_static (GTK_TYPE_SCROLLED_WINDOW,
                                      "GtkColumnView",
                                      &cv_info,
                                      (GTypeFlags) 0);
  }
  
  return cv_type;
}

static void
gtk_column_view_class_init (GtkColumnViewClass *klass)
{
  // TODO: signal setup?
}

static void
gtk_column_view_init (GtkColumnView *cv)
{
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (cv),
                                  GTK_POLICY_ALWAYS,
                                  GTK_POLICY_AUTOMATIC);

  cv->hBox = gtk_hbox_new (FALSE, 5);
  gtk_box_set_homogeneous (GTK_BOX (cv->hBox), FALSE);

  gtk_signal_connect_after (GTK_OBJECT (cv->hBox),
                            "size-allocate",
                            GTK_SIGNAL_FUNC (gtk_column_view_scroll_right),
                            cv);
}

static void
gtk_column_view_scroll_right (GtkContainer* container, GtkAllocation* allocation, gpointer data)
{
  // get the horizontal scrollbar adjustment from the signal data
  GtkAdjustment* hAdj = gtk_scrolled_window_get_hadjustment (GTK_SCROLLED_WINDOW (data));

  // fail-safe
  if (hAdj == NULL) return;

  // scroll to the far right
  hAdj->value = hAdj->upper - hAdj->page_size;

  // notify the toolkit which then redraws the contents if necessary
  gtk_adjustment_value_changed (hAdj);
}

GtkWidget*
gtk_column_view_new (void)
{
  GtkColumnView *cv = GTK_COLUMN_VIEW (g_object_new (GTK_COLUMN_VIEW_TYPE, NULL));
  
  // workaround for well-known bug in gtk+, this should actually be in the init function
  gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (cv), cv->hBox);
  
  return GTK_WIDGET (cv);
}

void
gtk_column_view_pack_start (GtkColumnView *cv, GtkWidget *child)
{
  gtk_box_pack_start (GTK_BOX (cv->hBox), child, FALSE, FALSE, 0);  
}

void
gtk_column_view_pack_end (GtkColumnView *cv, GtkWidget *child)
{
  gtk_box_pack_end (GTK_BOX (cv->hBox), child, FALSE, FALSE, 0);
}

GtkWidget* FrameManager::columnView = NULL;

void
FrameManager::attach (GtkContainer* container)
{
  reset();

  columnView = gtk_column_view_new ();
  gtk_container_add (container, columnView);
}

void
FrameManager::reset ()
{
  if (columnView == NULL) return;
  
  gtk_widget_destroy (columnView);
  columnView = NULL;
}

GtkWidget*
FrameManager::getNewFrame (const char* title)
{
  GtkWidget* frame;
  
  if (columnView == NULL) {
    frame = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    
    gtk_widget_set_name (frame, title);
    gtk_window_set_title (GTK_WINDOW (frame), title);

    gtk_window_set_policy (GTK_WINDOW (frame),
  			  FALSE, /* allow_shrink */
  			  TRUE, /* allow_grow */
  			  TRUE /* auto_shrink */);

    gtk_window_set_position (GTK_WINDOW (frame), GTK_WIN_POS_MOUSE);
  } else {
    frame = gtk_frame_new (title);
    gtk_column_view_pack_start (GTK_COLUMN_VIEW (columnView), frame);
  }
  
  return frame;
}
