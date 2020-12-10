// Adapted from "Hello World" demo at
// https://www.gtk.org/docs/getting-started/hello-world/

#include <gtk/gtk.h>
#include "demos.h"

static void
run_drawingarea (GtkWidget *widget,
                 gpointer   data)
{
  do_drawingarea (widget);
}

static void
run_paint (GtkWidget *widget,
           gpointer   data)
{
  do_paint (widget);
}

static void
run_event_axes (GtkWidget *widget,
                gpointer   data)
{
  do_event_axes (widget);
}

static void
activate (GtkApplication *app,
          gpointer        user_data)
{
  GtkWidget *window;
  GtkWidget *button_box;
  GtkWidget *button_area;
  GtkWidget *button_paint;
  GtkWidget *button_axes;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  //gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

  button_box = gtk_button_box_new (GTK_ORIENTATION_VERTICAL);
  gtk_container_add (GTK_CONTAINER (window), button_box);

  button_area = gtk_button_new_with_label ("Run Drawing Area Demo");
  g_signal_connect (button_area, "clicked", G_CALLBACK (run_drawingarea), NULL);
  gtk_container_add (GTK_CONTAINER (button_box), button_area);

  button_paint = gtk_button_new_with_label ("Run Paint Demo");
  g_signal_connect (button_paint, "clicked", G_CALLBACK (run_paint), NULL);
  gtk_container_add (GTK_CONTAINER (button_box), button_paint);

  button_axes = gtk_button_new_with_label ("Run Event Axes Demo");
  g_signal_connect (button_axes, "clicked", G_CALLBACK (run_event_axes), NULL);
  gtk_container_add (GTK_CONTAINER (button_box), button_axes);

  gtk_widget_show_all (window);
}

int
main (int    argc,
      char **argv)
{
  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
}
