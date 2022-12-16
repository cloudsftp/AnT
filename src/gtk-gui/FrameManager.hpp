#ifndef FRAMEMANAGER_HPP
#define FRAMEMANAGER_HPP

#include <glib.h>
#include <gtk/gtkscrolledwindow.h>
#include <gtk/gtkwidget.h>
#include <gtk/gtkframe.h>

G_BEGIN_DECLS

#define GTK_COLUMN_VIEW_TYPE            (gtk_column_view_get_type ())
#define GTK_COLUMN_VIEW(obj)            (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_COLUMN_VIEW_TYPE, GtkColumnView))
#define GTK_COLUMN_VIEW_CLASS(klass)    (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_COLUMN_VIEW_TYPE, GtkColumnViewClass))
#define IS_GTK_COLUMN_VIEW(obj)         (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_COLUMN_VIEW_TYPE))
#define IS_GTK_COLUMN_VIEW_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_COLUMN_VIEW_TYPE))

typedef struct _GtkColumnView      GtkColumnView;
typedef struct _GtkColumnViewClass GtkColumnViewClass;

struct _GtkColumnView
{
  GtkScrolledWindow scrollWnd;
  
  GtkWidget* hBox;
};

struct _GtkColumnViewClass
{
  GtkScrolledWindowClass parent_class;
  
  void (* gtk_column_view) (GtkColumnView *cv);
};

GType      gtk_column_view_get_type   (void);
GtkWidget* gtk_column_view_new        (void);

// pass-through calls to the encapsulated hbox
void       gtk_column_view_pack_start (GtkColumnView *cv, GtkWidget *child);
void       gtk_column_view_pack_end   (GtkColumnView *cv, GtkWidget *child);

G_END_DECLS

class FrameManager
{
private:
  static GtkWidget* columnView;
  
public:
  static void attach (GtkContainer* container);
  static void reset ();
  
  static GtkWidget* getNewFrame (const char* title);
};

#endif /* FRAMEMANAGER_HPP */
